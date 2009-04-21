//===--- ExternalSemaSource.h - External Sema Interface ---------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file defines the ExternalSemaSource interface.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_CLANG_SEMA_EXTERNAL_SEMA_SOURCE_H
#define LLVM_CLANG_SEMA_EXTERNAL_SEMA_SOURCE_H

#include "clang/AST/ExternalASTSource.h"

namespace clang {

class Sema;

/// \brief An abstract interface that should be implemented by
/// external AST sources that also provide information for semantic
/// analysis.
class ExternalSemaSource : public ExternalASTSource {
public:
  ExternalSemaSource() {
    ExternalASTSource::SemaSource = true;
  }

  /// \brief Initialize the semantic source with the Sema instance
  /// being used to perform semantic analysis on the abstract syntax
  /// tree.
  virtual void InitializeSema(Sema &S) {}
  
  // isa/cast/dyn_cast support
  static bool classof(const ExternalASTSource *Source) { 
    return Source->SemaSource;
  }
  static bool classof(const ExternalSemaSource *) { return true; }
};

} // end namespace clang

#endif
