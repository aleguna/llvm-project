; RUN: llc < %s  -fast-isel -O0 -regalloc=fast -asm-verbose=0 | FileCheck %s

target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64"
target triple = "x86_64-apple-darwin10.0.0"

; Make sure that fast-isel folds the immediate into the binop even though it
; is non-canonical.
define i32 @test1(i32 %i) nounwind ssp {
  %and = and i32 8, %i
  ret i32 %and
}

; CHECK: test1:
; CHECK: andl	$8, 


; rdar://9289512 - The load should fold into the compare.
define void @test2(i64 %x) nounwind ssp {
entry:
  %x.addr = alloca i64, align 8
  store i64 %x, i64* %x.addr, align 8
  %tmp = load i64* %x.addr, align 8
  %cmp = icmp sgt i64 %tmp, 42
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  ret void
; CHECK: test2:
; CHECK: movq	%rdi, -8(%rsp)
; CHECK: cmpq	$42, -8(%rsp)
}




@G = external global i32
define i64 @test3() nounwind {
  %A = ptrtoint i32* @G to i64
  ret i64 %A
; CHECK: test3:
; CHECK: movq _G@GOTPCREL(%rip), %rax
; CHECK-NEXT: ret
}



; rdar://9289558
@rtx_length = external global [153 x i8]

define i32 @test4(i64 %idxprom9) nounwind {
  %arrayidx10 = getelementptr inbounds [153 x i8]* @rtx_length, i32 0, i64 %idxprom9
  %tmp11 = load i8* %arrayidx10, align 1
  %conv = zext i8 %tmp11 to i32
  ret i32 %conv

; CHECK: test4:
; CHECK: movq	_rtx_length@GOTPCREL(%rip), %rax
; CHECK-NEXT: movzbl	(%rax,%rdi), %eax
; CHECK-NEXT: ret
}


; PR3242 - Out of range shifts should not be folded by fastisel.
define void @test5(i32 %x, i32* %p) nounwind {
  %y = ashr i32 %x, 50000
  store i32 %y, i32* %p
  ret void

; CHECK: test5:
; CHECK: movl	$50000, %ecx
; CHECK: sarl	%cl, %edi
; CHECK: ret
}

; rdar://9289501 - fast isel should fold trivial multiplies to shifts.
define i64 @test6(i64 %x) nounwind ssp {
entry:
  %mul = mul nsw i64 %x, 8
  ret i64 %mul

; CHECK: test6:
; CHECK: leaq	(,%rdi,8), %rax
}

define i32 @test7(i32 %x) nounwind ssp {
entry:
  %mul = mul nsw i32 %x, 8
  ret i32 %mul
; CHECK: test7:
; CHECK: leal	(,%rdi,8), %eax
}

