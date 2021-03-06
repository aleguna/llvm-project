//===- lib/ReaderWriter/ELF/X86/X86TargetInfo.h ---------------------------===//
//
//                             The LLVM Linker
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLD_READER_WRITER_ELF_X86_TARGETINFO_H
#define LLD_READER_WRITER_ELF_X86_TARGETINFO_H

#include "X86TargetHandler.h"

#include "lld/ReaderWriter/ELFTargetInfo.h"

#include "llvm/Object/ELF.h"
#include "llvm/Support/ELF.h"

namespace lld {
namespace elf {
class X86TargetInfo LLVM_FINAL : public ELFTargetInfo {
public:
  X86TargetInfo(llvm::Triple triple)
      : ELFTargetInfo(triple, std::unique_ptr<TargetHandlerBase>(
                                  new X86TargetHandler(*this))) {}

  /// \brief X86 has only two relative relocation
  /// a) for supporting IFUNC relocs - R_386_IRELATIVE
  /// b) for supporting relative relocs - R_386_RELATIVE
  virtual bool isRelativeReloc(const Reference &r) const {
    switch (r.kind()) {
    case llvm::ELF::R_386_IRELATIVE:
    case llvm::ELF::R_386_RELATIVE:
      return true;
    default:
      return false;
    }
  }
  virtual ErrorOr<Reference::Kind> relocKindFromString(StringRef str) const;
  virtual ErrorOr<std::string> stringFromRelocKind(Reference::Kind kind) const;
};
} // end namespace elf
} // end namespace lld
#endif
