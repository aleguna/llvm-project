//===-- PathV2.cpp - Implement OS Path Concept ------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file implements the operating system PathV2 API.
//
//===----------------------------------------------------------------------===//

#include "llvm/Support/PathV2.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/ErrorHandling.h"
#include <cctype>

namespace {
  using llvm::StringRef;

  bool is_separator(const char value) {
    switch(value) {
#ifdef LLVM_ON_WIN32
    case '\\': // fall through
#endif
    case '/': return true;
    default: return false;
    }
  }

#ifdef LLVM_ON_WIN32
  const StringRef separators = "\\/";
  const char      prefered_separator = '\\';
#else
  const StringRef separators = "/";
  const char      prefered_separator = '/';
#endif

  const llvm::error_code success;

  StringRef find_first_component(const StringRef  &path) {
    // Look for this first component in the following order.
    // * empty (in this case we return an empty string)
    // * either C: or {//,\\}net.
    // * {/,\}
    // * {.,..}
    // * {file,directory}name

    if (path.empty())
      return path;

#ifdef LLVM_ON_WIN32
    // C:
    if (path.size() >= 2 && std::isalpha(path[0]) && path[1] == ':')
      return StringRef(path.begin(), 2);
#endif

    // //net
    if ((path.size() > 2) &&
        is_separator(path[0]) &&
        path[0] == path[1] &&
        !is_separator(path[2])) {
      // Find the next directory separator.
      size_t end = path.find_first_of(separators, 2);
      if (end == StringRef::npos)
        return path;
      else
        return StringRef(path.begin(), end);
    }

    // {/,\}
    if (is_separator(path[0]))
      return StringRef(path.begin(), 1);

    if (path.startswith(".."))
      return StringRef(path.begin(), 2);

    if (path[0] == '.')
      return StringRef(path.begin(), 1);

    // * {file,directory}name
    size_t end = path.find_first_of(separators, 2);
    if (end == StringRef::npos)
      return path;
    else
      return StringRef(path.begin(), end);

    return StringRef();
  }

  size_t filename_pos(const StringRef &str) {
    if (str.size() == 2 &&
        is_separator(str[0]) &&
        str[0] == str[1])
      return 0;

    if (str.size() > 0 && is_separator(str[str.size() - 1]))
      return str.size() - 1;

    size_t pos = str.find_last_of(separators, str.size() - 1);

#ifdef LLVM_ON_WIN32
    if (pos == StringRef::npos)
      pos = str.find_last_of(':', str.size() - 2);
#endif

    if (pos == StringRef::npos ||
        (pos == 1 && is_separator(str[0])))
      return 0;

    return pos + 1;
  }

  size_t root_dir_start(const StringRef &str) {
    // case "c:/"
#ifdef LLVM_ON_WIN32
    if (str.size() > 2 &&
        str[1] == ':' &&
        is_separator(str[2]))
      return 2;
#endif

    // case "//"
    if (str.size() == 2 &&
        is_separator(str[0]) &&
        str[0] == str[1])
      return StringRef::npos;

    // case "//net"
    if (str.size() > 3 &&
        is_separator(str[0]) &&
        str[0] == str[1] &&
        !is_separator(str[2])) {
      return str.find_first_of(separators, 2);
    }

    // case "/"
    if (str.size() > 0 && is_separator(str[0]))
      return 0;

    return StringRef::npos;
  }

  size_t parent_path_end(const StringRef &path) {
    size_t end_pos = filename_pos(path);

    bool filename_was_sep = path.size() > 0 && is_separator(path[end_pos]);

    // Skip separators except for root dir.
    size_t root_dir_pos = root_dir_start(StringRef(path.begin(), end_pos));

    while(end_pos > 0 &&
          (end_pos - 1) != root_dir_pos &&
          is_separator(path[end_pos - 1]))
      --end_pos;

    if (end_pos == 1 && root_dir_pos == 0 && filename_was_sep)
      return StringRef::npos;

    return end_pos;
  }
}

namespace llvm {
namespace sys  {
namespace path {

const_iterator begin(const StringRef &path) {
  const_iterator i;
  i.Path      = path;
  i.Component = find_first_component(path);
  i.Position  = 0;
  return i;
}

const_iterator end(const StringRef &path) {
  const_iterator i;
  i.Path      = path;
  i.Position  = path.size();
  return i;
}

const_iterator &const_iterator::operator++() {
  assert(Position < Path.size() && "Tried to increment past end!");

  // Increment Position to past the current component
  Position += Component.size();

  // Check for end.
  if (Position == Path.size()) {
    Component = StringRef();
    return *this;
  }

  // Both POSIX and Windows treat paths that begin with exactly two separators
  // specially.
  bool was_net = Component.size() > 2 &&
    is_separator(Component[0]) &&
    Component[1] == Component[0] &&
    !is_separator(Component[2]);

  // Handle separators.
  if (is_separator(Path[Position])) {
    // Root dir.
    if (was_net
#ifdef LLVM_ON_WIN32
        // c:/
        || Component.endswith(":")
#endif
        ) {
      Component = StringRef(Path.begin() + Position, 1);
      return *this;
    }

    // Skip extra separators.
    while (Position != Path.size() &&
           is_separator(Path[Position])) {
      ++Position;
    }

    // Treat trailing '/' as a '.'.
    if (Position == Path.size()) {
      --Position;
      Component = ".";
      return *this;
    }
  }

  // Find next component.
  size_t end_pos = Path.find_first_of(separators, Position);
  if (end_pos == StringRef::npos)
    end_pos = Path.size();
  Component = StringRef(Path.begin() + Position, end_pos - Position);

  return *this;
}

const_iterator &const_iterator::operator--() {
  // If we're at the end and the previous char was a '/', return '.'.
  if (Position == Path.size() &&
      Path.size() > 1 &&
      is_separator(Path[Position - 1])
#ifdef LLVM_ON_WIN32
      && Path[Position - 2] != ':'
#endif
      ) {
    --Position;
    Component = ".";
    return *this;
  }

  // Skip separators unless it's the root directory.
  size_t root_dir_pos = root_dir_start(Path);
  size_t end_pos = Position;

  while(end_pos > 0 &&
        (end_pos - 1) != root_dir_pos &&
        is_separator(Path[end_pos - 1]))
    --end_pos;

  // Find next separator.
  size_t start_pos = filename_pos(StringRef(Path.begin(), end_pos));
  Component = StringRef(Path.begin() + start_pos, end_pos - start_pos);
  Position = start_pos;
  return *this;
}

bool const_iterator::operator==(const const_iterator &RHS) const {
  return Path.begin() == RHS.Path.begin() &&
         Position == RHS.Position;
}

bool const_iterator::operator!=(const const_iterator &RHS) const {
  return !(*this == RHS);
}

ptrdiff_t const_iterator::operator-(const const_iterator &RHS) const {
  return Position - RHS.Position;
}

void root_path(const StringRef &path, StringRef &result) {
  const_iterator b = begin(path),
                 pos = b,
                 e = end(path);
  if (b != e) {
    bool has_net = b->size() > 2 && is_separator((*b)[0]) && (*b)[1] == (*b)[0];
    bool has_drive =
#ifdef LLVM_ON_WIN32
      b->endswith(":");
#else
      false;
#endif

    if (has_net || has_drive) {
      if ((++pos != e) && is_separator((*pos)[0])) {
        // {C:/,//net/}, so get the first two components.
        result = StringRef(path.begin(), b->size() + pos->size());
        return;
      } else {
        // just {C:,//net}, return the first component.
        result = *b;
        return;
      }
    }

    // POSIX style root directory.
    if (is_separator((*b)[0])) {
      result = *b;
      return;
    }
  }

  result = StringRef();
}

void root_name(const StringRef &path, StringRef &result) {
  const_iterator b = begin(path),
                 e = end(path);
  if (b != e) {
    bool has_net = b->size() > 2 && is_separator((*b)[0]) && (*b)[1] == (*b)[0];
    bool has_drive =
#ifdef LLVM_ON_WIN32
      b->endswith(":");
#else
      false;
#endif

    if (has_net || has_drive) {
      // just {C:,//net}, return the first component.
      result = *b;
      return;
    }
  }

  // No path or no name.
  result = StringRef();
  return;
}

void root_directory(const StringRef &path, StringRef &result) {
  const_iterator b = begin(path),
                 pos = b,
                 e = end(path);
  if (b != e) {
    bool has_net = b->size() > 2 && is_separator((*b)[0]) && (*b)[1] == (*b)[0];
    bool has_drive =
#ifdef LLVM_ON_WIN32
      b->endswith(":");
#else
      false;
#endif

    if ((has_net || has_drive) &&
        // {C:,//net}, skip to the next component.
        (++pos != e) && is_separator((*pos)[0])) {
      result = *pos;
      return;
    }

    // POSIX style root directory.
    if (!has_net && is_separator((*b)[0])) {
      result = *b;
      return;
    }
  }

  // No path or no root.
  result = StringRef();
  return;
}

void relative_path(const StringRef &path, StringRef &result) {
  StringRef root;
  root_path(path, root);
  result = StringRef(path.begin() + root.size(), path.size() - root.size());
  return;
}

void append(SmallVectorImpl<char> &path, const Twine &a,
                                         const Twine &b,
                                         const Twine &c,
                                         const Twine &d) {
  SmallString<32> a_storage;
  SmallString<32> b_storage;
  SmallString<32> c_storage;
  SmallString<32> d_storage;

  SmallVector<StringRef, 4> components;
  if (!a.isTriviallyEmpty()) components.push_back(a.toStringRef(a_storage));
  if (!b.isTriviallyEmpty()) components.push_back(b.toStringRef(b_storage));
  if (!c.isTriviallyEmpty()) components.push_back(c.toStringRef(c_storage));
  if (!d.isTriviallyEmpty()) components.push_back(d.toStringRef(d_storage));

  for (SmallVectorImpl<StringRef>::const_iterator i = components.begin(),
                                                  e = components.end();
                                                  i != e; ++i) {
    bool path_has_sep = !path.empty() && is_separator(path[path.size() - 1]);
    bool component_has_sep = !i->empty() && is_separator((*i)[0]);
    bool is_root_name = false;
    has_root_name(*i, is_root_name);

    if (path_has_sep) {
      // Strip separators from beginning of component.
      size_t loc = i->find_first_not_of(separators);
      StringRef c = StringRef(i->begin() + loc, i->size() - loc);

      // Append it.
      path.append(c.begin(), c.end());
      continue;
    }

    if (!component_has_sep && !(path.empty() || is_root_name)) {
      // Add a separator.
      path.push_back(prefered_separator);
    }

    path.append(i->begin(), i->end());
  }
}

void parent_path(const StringRef &path, StringRef &result) {
  size_t end_pos = parent_path_end(path);
  if (end_pos == StringRef::npos)
    result = StringRef();
  else
    result = StringRef(path.data(), end_pos);
}

void remove_filename(SmallVectorImpl<char> &path) {
  size_t end_pos = parent_path_end(StringRef(path.begin(), path.size()));
  if (end_pos != StringRef::npos)
    path.set_size(end_pos);
}

void replace_extension(SmallVectorImpl<char> &path,
                             const Twine &extension) {
  StringRef p(path.begin(), path.size());
  SmallString<32> ext_storage;
  StringRef ext = extension.toStringRef(ext_storage);

  // Erase existing extension.
  size_t pos = p.find_last_of('.');
  if (pos != StringRef::npos && pos >= filename_pos(p))
    path.set_size(pos);

  // Append '.' if needed.
  if (ext.size() > 0 && ext[0] != '.')
    path.push_back('.');

  // Append extension.
  path.append(ext.begin(), ext.end());
}

void native(const Twine &path, SmallVectorImpl<char> &result) {
  // Clear result.
  result.clear();
#ifdef LLVM_ON_WIN32
  SmallString<128> path_storage;
  StringRef p = path.toStringRef(path_storage);
  result.reserve(p.size());
  for (StringRef::const_iterator i = p.begin(),
                                 e = p.end();
                                 i != e;
                                 ++i) {
    if (*i == '/')
      result.push_back('\\');
    else
      result.push_back(*i);
  }
#else
  path.toVector(result);
#endif
}

void filename(const StringRef &path, StringRef &result) {
  result = *(--end(path));
}

void stem(const StringRef &path, StringRef &result) {
  StringRef fname;
  filename(path, fname);
  size_t pos = fname.find_last_of('.');
  if (pos == StringRef::npos)
    result = fname;
  else
    if ((fname.size() == 1 && fname == ".") ||
        (fname.size() == 2 && fname == ".."))
      result = fname;
    else
      result = StringRef(fname.begin(), pos);
}

void extension(const StringRef &path, StringRef &result) {
  StringRef fname;
  filename(path, fname);
  size_t pos = fname.find_last_of('.');
  if (pos == StringRef::npos)
    result = StringRef();
  else
    if ((fname.size() == 1 && fname == ".") ||
        (fname.size() == 2 && fname == ".."))
      result = StringRef();
    else
      result = StringRef(fname.begin() + pos, fname.size() - pos);
}

void has_root_name(const Twine &path, bool &result) {
  SmallString<128> path_storage;
  StringRef p = path.toStringRef(path_storage);

  root_name(p, p);

  result = !p.empty();
}

void has_root_directory(const Twine &path, bool &result) {
  SmallString<128> path_storage;
  StringRef p = path.toStringRef(path_storage);

  root_directory(p, p);

  result = !p.empty();
}

void has_root_path(const Twine &path, bool &result) {
  SmallString<128> path_storage;
  StringRef p = path.toStringRef(path_storage);

  root_path(p, p);

  result = !p.empty();
}

void has_filename(const Twine &path, bool &result) {
  SmallString<128> path_storage;
  StringRef p = path.toStringRef(path_storage);

  filename(p, p);

  result = !p.empty();
}

void has_parent_path(const Twine &path, bool &result) {
  SmallString<128> path_storage;
  StringRef p = path.toStringRef(path_storage);

  parent_path(p, p);

  result = !p.empty();
}

void has_stem(const Twine &path, bool &result) {
  SmallString<128> path_storage;
  StringRef p = path.toStringRef(path_storage);

  stem(p, p);

  result = !p.empty();
}

void has_extension(const Twine &path, bool &result) {
  SmallString<128> path_storage;
  StringRef p = path.toStringRef(path_storage);

  extension(p, p);

  result = !p.empty();
}

void is_absolute(const Twine &path, bool &result) {
  SmallString<128> path_storage;
  StringRef p = path.toStringRef(path_storage);

  bool rootDir = false,
       rootName = false;
  has_root_directory(p, rootDir);
#ifdef LLVM_ON_WIN32
  has_root_name(p, rootName);
#else
  rootName = true;
#endif

  result = rootDir && rootName;
}

void is_relative(const Twine &path, bool &result) {
  bool res;
  is_absolute(path, res);
  result = !res;
}

} // end namespace path

namespace fs {

error_code make_absolute(SmallVectorImpl<char> &path) {
  StringRef p(path.data(), path.size());

  bool rootName = false, rootDirectory = false;
  path::has_root_name(p, rootName);
  path::has_root_directory(p, rootDirectory);

  // Already absolute.
  if (rootName && rootDirectory)
    return success;

  // All of the following conditions will need the current directory.
  SmallString<128> current_dir;
  if (error_code ec = current_path(current_dir)) return ec;

  // Relative path. Prepend the current directory.
  if (!rootName && !rootDirectory) {
    // Append path to the current directory.
    path::append(current_dir, p);
    // Set path to the result.
    path.swap(current_dir);
    return success;
  }

  if (!rootName && rootDirectory) {
    StringRef cdrn;
    path::root_name(current_dir, cdrn);
    SmallString<128> curDirRootName(cdrn.begin(), cdrn.end());
    path::append(curDirRootName, p);
    // Set path to the result.
    path.swap(curDirRootName);
    return success;
  }

  if (rootName && !rootDirectory) {
    StringRef pRootName;
    StringRef bRootDirectory;
    StringRef bRelativePath;
    StringRef pRelativePath;
    path::root_name(p, pRootName);
    path::root_directory(current_dir, bRootDirectory);
    path::relative_path(current_dir, bRelativePath);
    path::relative_path(p, pRelativePath);

    SmallString<128> res;
    path::append(res, pRootName, bRootDirectory, bRelativePath, pRelativePath);
    path.swap(res);
    return success;
  }

  llvm_unreachable("All rootName and rootDirectory combinations should have "
                   "occurred above!");
}

error_code create_directories(const Twine &path, bool &existed) {
  SmallString<128> path_storage;
  StringRef p = path.toStringRef(path_storage);

  StringRef parent;
  bool parent_exists;
  path::parent_path(p, parent);
  if (error_code ec = fs::exists(parent, parent_exists)) return ec;

  if (!parent_exists)
    return create_directories(parent, existed);

  return create_directory(p, existed);
}

void directory_entry::replace_filename(const Twine &filename, file_status st,
                                       file_status symlink_st) {
  SmallString<128> path(Path.begin(), Path.end());
  path::remove_filename(path);
  path::append(path, filename);
  Path = path.str();
  Status = st;
  SymlinkStatus = symlink_st;
}

} // end namespace fs
} // end namespace sys
} // end namespace llvm

// Include the truly platform-specific parts.
#if defined(LLVM_ON_UNIX)
#include "Unix/PathV2.inc"
#endif
#if defined(LLVM_ON_WIN32)
#include "Windows/PathV2.inc"
#endif
