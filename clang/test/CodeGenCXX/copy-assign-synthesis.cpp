// RUN: clang-cc -emit-llvm -o %t %s &&
// RUN: grep "_ZN1XaSERK1X" %t | count 0

extern "C" int printf(...);

struct B {
  B() : B1(3.14), B2(3.15) {} 
  float B1;
  float B2;
  void pr() {
    printf("B1 = %f B2 = %f\n", B1, B2);
  }
};

struct M {
  M() : M1(10), M2(11) {} 
  int M1;
  int M2;
  void pr() {
    printf("M1 = %d M2 = %d\n", M1, M2);
  }
};

struct N  : B {
  N() : N1(20), N2(21) {} 
  int N1;
  int N2;
  void pr() {
    printf("N1 = %d N2 = %d\n", N1, N2);
    B::pr();
  }
};

struct Q {
  Q() : Q1(30), Q2(31) {} 
  int Q1;
  int Q2;
  void pr() {
    printf("Q1 = %d Q2 = %d\n", Q1, Q2);
  }
};


struct X : M , N { 
  X() : d(0.0), d1(1.1), d2(1.2), d3(1.3) {}
  double d;
  double d1;
  double d2;
  double d3;
  void pr() {
    printf("d = %f d1 = %f d2 = %f d3 = %f\n", d, d1,d2,d3);
    M::pr(); N::pr();
    q1.pr(); q2.pr();
  }

 Q q1, q2;
}; 


X srcX; 
X dstX; 
X dstY; 

int main() {
  dstY = dstX = srcX;
  srcX.pr();
  dstX.pr();
  dstY.pr();
}

