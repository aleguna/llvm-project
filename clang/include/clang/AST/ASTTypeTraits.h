//===--- ASTTypeTraits.h ----------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  Provides a dynamic type identifier and a dynamically typed node container
//  that can be used to store an AST base node at runtime in the same storage in
//  a type safe way.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_AST_AST_TYPE_TRAITS_H
#define LLVM_CLANG_AST_AST_TYPE_TRAITS_H

#include "clang/AST/ASTFwd.h"
#include "clang/AST/Decl.h"
#include "clang/AST/Stmt.h"
#include "clang/AST/TypeLoc.h"
#include "clang/Basic/LLVM.h"
#include "llvm/Support/AlignOf.h"

namespace clang {
namespace ast_type_traits {

/// \brief Kind identifier.
///
/// It can be constructed from any node kind and allows for runtime type
/// hierarchy checks.
/// Use getFromNodeKind<T>() to construct them.
class ASTNodeKind {
public:
  /// \brief Empty identifier. It matches nothing.
  ASTNodeKind() : KindId(NKI_None) {}

  /// \brief Construct an identifier for T.
  template <class T>
  static ASTNodeKind getFromNodeKind() {
    return ASTNodeKind(KindToKindId<T>::Id);
  }

  /// \brief Returns \c true if \c this and \c Other represent the same kind.
  bool isSame(ASTNodeKind Other) const;

  /// \brief Returns \c true if \c this is a base kind of (or same as) \c Other
  bool isBaseOf(ASTNodeKind Other) const;

  /// \brief String representation of the kind.
  StringRef asStringRef() const;

private:
  /// \brief Kind ids.
  ///
  /// Includes all possible base and derived kinds.
  enum NodeKindId {
    NKI_None,
    NKI_NestedNameSpecifier,
    NKI_NestedNameSpecifierLoc,
    NKI_QualType,
    NKI_TypeLoc,
    NKI_Decl,
#define DECL(DERIVED, BASE) NKI_##DERIVED##Decl,
#include "clang/AST/DeclNodes.inc"
    NKI_Stmt,
#define STMT(DERIVED, BASE) NKI_##DERIVED,
#include "clang/AST/StmtNodes.inc"
    NKI_Type,
#define TYPE(DERIVED, BASE) NKI_##DERIVED##Type,
#include "clang/AST/TypeNodes.def"
    NKI_NumberOfKinds
  };

  /// \brief Use getFromNodeKind<T>() to construct the kind.
  ASTNodeKind(NodeKindId KindId) : KindId(KindId) {}

  /// \brief Returns \c true if \c Base is a base kind of (or same as) \c
  ///   Derived
  static bool isBaseOf(NodeKindId Base, NodeKindId Derived);

  /// \brief Helper meta-function to convert a kind T to its enum value.
  ///
  /// This struct is specialized below for all known kinds.
  template <class T> struct KindToKindId {
    static const NodeKindId Id = NKI_None;
  };

  /// \brief Per kind info.
  struct KindInfo {
    /// \brief The id of the parent kind, or None if it has no parent.
    NodeKindId ParentId;
    /// \brief Name of the kind.
    const char *Name;
  };
  static const KindInfo AllKindInfo[NKI_NumberOfKinds];

  NodeKindId KindId;
};

#define KIND_TO_KIND_ID(Class)                                                 \
  template <> struct ASTNodeKind::KindToKindId<Class> {                        \
    static const NodeKindId Id = NKI_##Class;                                  \
  };
KIND_TO_KIND_ID(NestedNameSpecifier)
KIND_TO_KIND_ID(NestedNameSpecifierLoc)
KIND_TO_KIND_ID(QualType)
KIND_TO_KIND_ID(TypeLoc)
KIND_TO_KIND_ID(Decl)
KIND_TO_KIND_ID(Stmt)
KIND_TO_KIND_ID(Type)
#define DECL(DERIVED, BASE) KIND_TO_KIND_ID(DERIVED##Decl)
#include "clang/AST/DeclNodes.inc"
#define STMT(DERIVED, BASE) KIND_TO_KIND_ID(DERIVED)
#include "clang/AST/StmtNodes.inc"
#define TYPE(DERIVED, BASE) KIND_TO_KIND_ID(DERIVED##Type)
#include "clang/AST/TypeNodes.def"
#undef KIND_TO_KIND_ID

/// \brief A dynamically typed AST node container.
///
/// Stores an AST node in a type safe way. This allows writing code that
/// works with different kinds of AST nodes, despite the fact that they don't
/// have a common base class.
///
/// Use \c create(Node) to create a \c DynTypedNode from an AST node,
/// and \c get<T>() to retrieve the node as type T if the types match.
///
/// See \c NodeTypeTag for which node base types are currently supported;
/// You can create DynTypedNodes for all nodes in the inheritance hierarchy of
/// the supported base types.
class DynTypedNode {
public:
  /// \brief Creates a \c DynTypedNode from \c Node.
  template <typename T>
  static DynTypedNode create(const T &Node) {
    return BaseConverter<T>::create(Node);
  }

  /// \brief Retrieve the stored node as type \c T.
  ///
  /// Returns NULL if the stored node does not have a type that is
  /// convertible to \c T.
  ///
  /// For types that have identity via their pointer in the AST
  /// (like \c Stmt, \c Decl, \c Type and \c NestedNameSpecifier) the returned
  /// pointer points to the referenced AST node.
  /// For other types (like \c QualType) the value is stored directly
  /// in the \c DynTypedNode, and the returned pointer points at
  /// the storage inside DynTypedNode. For those nodes, do not
  /// use the pointer outside the scope of the DynTypedNode.
  template <typename T>
  const T *get() const {
    return BaseConverter<T>::get(NodeKind, Storage.buffer);
  }

  /// \brief Returns a pointer that identifies the stored AST node.
  ///
  /// Note that this is not supported by all AST nodes. For AST nodes
  /// that don't have a pointer-defined identity inside the AST, this
  /// method returns NULL.
  const void *getMemoizationData() const;

  /// @{
  /// \brief Imposes an order on \c DynTypedNode.
  ///
  /// Supports comparison of nodes that support memoization.
  /// FIXME: Implement comparsion for other node types (currently
  /// only Stmt, Decl, Type and NestedNameSpecifier return memoization data).
  bool operator<(const DynTypedNode &Other) const {
    assert(getMemoizationData() && Other.getMemoizationData());
    return getMemoizationData() < Other.getMemoizationData();
  }
  bool operator==(const DynTypedNode &Other) const {
    // Nodes with different types cannot be equal.
    if (!NodeKind.isSame(Other.NodeKind))
      return false;

    // FIXME: Implement for other types.
    if (ASTNodeKind::getFromNodeKind<QualType>().isBaseOf(NodeKind)) {
      return *get<QualType>() == *Other.get<QualType>();
    }
    assert(getMemoizationData() && Other.getMemoizationData());
    return getMemoizationData() == Other.getMemoizationData();
  }
  bool operator!=(const DynTypedNode &Other) const {
    return !operator==(Other);
  }
  /// @}

private:
  /// \brief Takes care of converting from and to \c T.
  template <typename T, typename EnablerT = void> struct BaseConverter;

  ASTNodeKind NodeKind;

  /// \brief Stores the data of the node.
  ///
  /// Note that we can store \c Decls, \c Stmts, \c Types and
  /// \c NestedNameSpecifiers by pointer as they are guaranteed to be unique
  /// pointers pointing to dedicated storage in the AST. \c QualTypes on the
  /// other hand do not have storage or unique pointers and thus need to be
  /// stored by value.
  llvm::AlignedCharArrayUnion<Decl *, Stmt *, Type *, NestedNameSpecifier *,
                              NestedNameSpecifierLoc, QualType, TypeLoc>
      Storage;
};

// FIXME: Pull out abstraction for the following.
template<typename T> struct DynTypedNode::BaseConverter<T,
    typename llvm::enable_if<llvm::is_base_of<Decl, T> >::type> {
  static const T *get(ASTNodeKind NodeKind, const char Storage[]) {
    if (ASTNodeKind::getFromNodeKind<Decl>().isBaseOf(NodeKind))
      return dyn_cast<T>(*reinterpret_cast<Decl*const*>(Storage));
    return NULL;
  }
  static DynTypedNode create(const Decl &Node) {
    DynTypedNode Result;
    Result.NodeKind = ASTNodeKind::getFromNodeKind<T>();
    new (Result.Storage.buffer) const Decl*(&Node);
    return Result;
  }
};
template<typename T> struct DynTypedNode::BaseConverter<T,
    typename llvm::enable_if<llvm::is_base_of<Stmt, T> >::type> {
  static const T *get(ASTNodeKind NodeKind, const char Storage[]) {
    if (ASTNodeKind::getFromNodeKind<Stmt>().isBaseOf(NodeKind))
      return dyn_cast<T>(*reinterpret_cast<Stmt*const*>(Storage));
    return NULL;
  }
  static DynTypedNode create(const Stmt &Node) {
    DynTypedNode Result;
    Result.NodeKind = ASTNodeKind::getFromNodeKind<T>();
    new (Result.Storage.buffer) const Stmt*(&Node);
    return Result;
  }
};
template<typename T> struct DynTypedNode::BaseConverter<T,
    typename llvm::enable_if<llvm::is_base_of<Type, T> >::type> {
  static const T *get(ASTNodeKind NodeKind, const char Storage[]) {
    if (ASTNodeKind::getFromNodeKind<Type>().isBaseOf(NodeKind))
      return dyn_cast<T>(*reinterpret_cast<Type*const*>(Storage));
    return NULL;
  }
  static DynTypedNode create(const Type &Node) {
    DynTypedNode Result;
    Result.NodeKind = ASTNodeKind::getFromNodeKind<T>();
    new (Result.Storage.buffer) const Type*(&Node);
    return Result;
  }
};
template<> struct DynTypedNode::BaseConverter<NestedNameSpecifier, void> {
  static const NestedNameSpecifier *get(ASTNodeKind NodeKind,
                                        const char Storage[]) {
    if (ASTNodeKind::getFromNodeKind<NestedNameSpecifier>().isBaseOf(NodeKind))
      return *reinterpret_cast<NestedNameSpecifier*const*>(Storage);
    return NULL;
  }
  static DynTypedNode create(const NestedNameSpecifier &Node) {
    DynTypedNode Result;
    Result.NodeKind = ASTNodeKind::getFromNodeKind<NestedNameSpecifier>();
    new (Result.Storage.buffer) const NestedNameSpecifier*(&Node);
    return Result;
  }
};
template<> struct DynTypedNode::BaseConverter<NestedNameSpecifierLoc, void> {
  static const NestedNameSpecifierLoc *get(ASTNodeKind NodeKind,
                                           const char Storage[]) {
    if (ASTNodeKind::getFromNodeKind<NestedNameSpecifierLoc>().isBaseOf(
            NodeKind))
      return reinterpret_cast<const NestedNameSpecifierLoc*>(Storage);
    return NULL;
  }
  static DynTypedNode create(const NestedNameSpecifierLoc &Node) {
    DynTypedNode Result;
    Result.NodeKind = ASTNodeKind::getFromNodeKind<NestedNameSpecifierLoc>();
    new (Result.Storage.buffer) NestedNameSpecifierLoc(Node);
    return Result;
  }
};
template<> struct DynTypedNode::BaseConverter<QualType, void> {
  static const QualType *get(ASTNodeKind NodeKind, const char Storage[]) {
    if (ASTNodeKind::getFromNodeKind<QualType>().isBaseOf(NodeKind))
      return reinterpret_cast<const QualType*>(Storage);
    return NULL;
  }
  static DynTypedNode create(const QualType &Node) {
    DynTypedNode Result;
    Result.NodeKind = ASTNodeKind::getFromNodeKind<QualType>();
    new (Result.Storage.buffer) QualType(Node);
    return Result;
  }
};
template<> struct DynTypedNode::BaseConverter<TypeLoc, void> {
  static const TypeLoc *get(ASTNodeKind NodeKind, const char Storage[]) {
    if (ASTNodeKind::getFromNodeKind<TypeLoc>().isBaseOf(NodeKind))
      return reinterpret_cast<const TypeLoc*>(Storage);
    return NULL;
  }
  static DynTypedNode create(const TypeLoc &Node) {
    DynTypedNode Result;
    Result.NodeKind = ASTNodeKind::getFromNodeKind<TypeLoc>();
    new (Result.Storage.buffer) TypeLoc(Node);
    return Result;
  }
};
// The only operation we allow on unsupported types is \c get.
// This allows to conveniently use \c DynTypedNode when having an arbitrary
// AST node that is not supported, but prevents misuse - a user cannot create
// a DynTypedNode from arbitrary types.
template <typename T, typename EnablerT> struct DynTypedNode::BaseConverter {
  static const T *get(ASTNodeKind NodeKind, const char Storage[]) {
    return NULL;
  }
};

inline const void *DynTypedNode::getMemoizationData() const {
  if (ASTNodeKind::getFromNodeKind<Decl>().isBaseOf(NodeKind)) {
    return BaseConverter<Decl>::get(NodeKind, Storage.buffer);
  } else if (ASTNodeKind::getFromNodeKind<Stmt>().isBaseOf(NodeKind)) {
    return BaseConverter<Stmt>::get(NodeKind, Storage.buffer);
  } else if (ASTNodeKind::getFromNodeKind<Type>().isBaseOf(NodeKind)) {
    return BaseConverter<Type>::get(NodeKind, Storage.buffer);
  } else if (ASTNodeKind::getFromNodeKind<NestedNameSpecifier>().isBaseOf(NodeKind)) {
    return BaseConverter<NestedNameSpecifier>::get(NodeKind, Storage.buffer);
  }
  return NULL;
}

} // end namespace ast_type_traits
} // end namespace clang

#endif // LLVM_CLANG_AST_AST_TYPE_TRAITS_H
