; NOTE: Assertions have been autogenerated by utils/update_test_checks.py UTC_ARGS: --function-signature
; RUN: llvm-as --opaque-pointers < %s | llvm-dis --opaque-pointers | FileCheck %s
; RUN: llvm-as < %s | llvm-dis --opaque-pointers | FileCheck %s
; RUN: llvm-as --opaque-pointers < %s | llvm-dis | FileCheck %s
; RUN: opt --opaque-pointers < %s -S | FileCheck %s
; RUN: verify-uselistorder --opaque-pointers < %s

%ty = type i32*

; CHECK: @g = external global i16
@g = external global i16

; CHECK: @llvm.global_ctors = appending global [1 x { i32, ptr, ptr }] [{ i32, ptr, ptr } { i32 1, ptr null, ptr null }]
@llvm.global_ctors = appending global [1 x { i32, void ()*, i8* }] [{ i32, void ()*, i8* } { i32 1, void ()* null, i8* null }]

; CHECK: @ga = alias i18, ptr @g2
@g2 = global i18 0
@ga = alias i18, i18* @g2

; CHECK: @ga2 = alias i19, ptr @g2
@ga2 = alias i19, i19* bitcast (i18* @g2 to i19*)

; CHECK: @gi = ifunc i20 (), ptr @f
@gi = ifunc i20 (), i20 ()* ()* bitcast (void (i32*)* @f to i20 ()* ()*)

define void @f(i32* %p) {
; CHECK-LABEL: define {{[^@]+}}@f
; CHECK-SAME: (ptr [[P:%.*]]) {
; CHECK-NEXT:    [[A:%.*]] = alloca i17, align 4
; CHECK-NEXT:    call void @fn.fwd(i32 0)
; CHECK-NEXT:    store i32 0, ptr @g.fwd, align 4
; CHECK-NEXT:    ret void
;
  %a = alloca i17
  call void @fn.fwd(i32 0)
  store i32 0, i32* @g.fwd
  ret void
}

@g.fwd = global i32 0
declare void @fn.fwd(i32)

define void @f2(i32** %p) {
; CHECK-LABEL: define {{[^@]+}}@f2
; CHECK-SAME: (ptr [[P:%.*]]) {
; CHECK-NEXT:    unreachable
;
  unreachable
}

define void @f3(i32 addrspace(1)* addrspace(2)* %p) {
; CHECK-LABEL: define {{[^@]+}}@f3
; CHECK-SAME: (ptr addrspace(2) [[P:%.*]]) {
; CHECK-NEXT:    unreachable
;
  unreachable
}

define void @f4(%ty* %p) {
; CHECK-LABEL: define {{[^@]+}}@f4
; CHECK-SAME: (ptr [[P:%.*]]) {
; CHECK-NEXT:    unreachable
;
  unreachable
}

define void @remangle_intrinsic() {
; CHECK-LABEL: define {{[^@]+}}@remangle_intrinsic() {
; CHECK-NEXT:    [[A:%.*]] = alloca ptr, align 8
; CHECK-NEXT:    [[TMP1:%.*]] = call ptr @llvm.stacksave.p0()
; CHECK-NEXT:    call void @llvm.stackprotector.p0(ptr null, ptr [[A]])
; CHECK-NEXT:    [[TMP2:%.*]] = call <2 x i64> @llvm.masked.expandload.v2i64(ptr null, <2 x i1> zeroinitializer, <2 x i64> zeroinitializer)
; CHECK-NEXT:    ret void
;
  %a = alloca i8*
  call i8* @llvm.stacksave()
  call void @llvm.stackprotector(i8* null, i8** %a)
  call <2 x i64> @llvm.masked.expandload.v2i64(i64* null, <2 x i1> zeroinitializer, <2 x i64> zeroinitializer)
  ret void
}

define i32* @constexpr_gep() {
; CHECK-LABEL: define {{[^@]+}}@constexpr_gep() {
; CHECK-NEXT:    ret ptr getelementptr (i32, ptr getelementptr (i8, ptr null, i64 4), i64 1)
;
  ret i32* getelementptr(i32, i32* bitcast (i8* getelementptr (i8, i8* null, i64 4) to i32*), i64 1)
}

declare i8* @llvm.stacksave()
declare void @llvm.stackprotector(i8*, i8**)
declare <2 x i64> @llvm.masked.expandload.v2i64(i64*, <2 x i1>, <2 x i64>)
