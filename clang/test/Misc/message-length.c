// RUN: clang -fsyntax-only -fmessage-length=72 %s 2> %t &&

// RUN: grep -A4 "FILE:23" %t > %t.msg &&
// FIXME: This diagnostic is getting truncated very poorly.
// RUN: grep -e '^  ... ' %t.msg &&
// RUN: grep -e '^     \^' %t.msg &&

// RUN: true

// Hack so we can check things better, force the file name and line.

# 1 "FILE" 1

/* It's tough to verify the results of this test mechanically, since
   the length of the filename (and, therefore, how the word-wrapping
   behaves) changes depending on where the test-suite resides in the
   file system. */
void f(int, float, char, float);

void g() {
      int (*fp1)(int, float, short, float) = f;

  int (*fp2)(int, float, short, float) = f;
}

void a_func_to_call(int, int, int);

void a_very_long_line(int *ip, float *FloatPointer) {
  for (int ALongIndexName = 0; ALongIndexName < 100; ALongIndexName++) if (ip[ALongIndexName] == 17) a_func_to_call(ip == FloatPointer, ip[ALongIndexName], FloatPointer[ALongIndexName]);


  int array0[] = { [3] 3, 5, 7, 4, 2, 7, 6, 3, 4, 5, 6, 7, 8, 9, 12, 345, 14, 345, 789, 234, 678, 345, 123, 765, 234 };
}

#pragma STDC CX_LIMITED_RANGE    // some long comment text and a brace, eh {}
