// RUN: clang -fsyntax-only -verify %s

// FIXME: This is a horrible error message here. Fix.
int @"s" = 5;  // expected-error {{prefix attribute must be}}
