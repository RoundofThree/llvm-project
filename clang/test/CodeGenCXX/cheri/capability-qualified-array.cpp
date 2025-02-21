// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py
// RUN: %cheri_cc1 -disable-O0-optnone -emit-llvm %s -o - \
// RUN:   | opt -S -mem2reg | FileCheck %s

typedef __SIZE_TYPE__ size_t;

template <typename T>
struct dependent_type {
  T * __capability decay_incomplete(T p[__capability]) {
    return p + 1;
  }

  T * __capability member_incomplete(T p[__capability]) {
    return &p[1];
  }

  T sub_incomplete(T p[__capability]) {
    return p[1];
  }

  T * __capability decay_constant(T p[__capability 2]) {
    return p + 1;
  }

  T * __capability member_constant(T p[__capability 2]) {
    return &p[1];
  }

  T sub_constant(T p[__capability 2]) {
    return p[1];
  }

  T * __capability decay_variable(size_t n, T p[__capability n]) {
    return p + 1;
  }

  T * __capability member_variable(size_t n, T p[__capability n]) {
    return &p[1];
  }

  T sub_variable(size_t n, T p[__capability n]) {
    return p[1];
  }
};
// CHECK-LABEL: @_ZN14dependent_typeIcE16decay_incompleteEU12__capabilityPc(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[ADD_PTR:%.*]] = getelementptr inbounds i8, ptr addrspace(200) [[P:%.*]], i64 1
// CHECK-NEXT:    ret ptr addrspace(200) [[ADD_PTR]]
//
// CHECK-LABEL: @_ZN14dependent_typeIcE17member_incompleteEU12__capabilityPc(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[ARRAYIDX:%.*]] = getelementptr inbounds i8, ptr addrspace(200) [[P:%.*]], i64 1
// CHECK-NEXT:    ret ptr addrspace(200) [[ARRAYIDX]]
//
// CHECK-LABEL: @_ZN14dependent_typeIcE14sub_incompleteEU12__capabilityPc(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[ARRAYIDX:%.*]] = getelementptr inbounds i8, ptr addrspace(200) [[P:%.*]], i64 1
// CHECK-NEXT:    [[TMP0:%.*]] = load i8, ptr addrspace(200) [[ARRAYIDX]], align 1
// CHECK-NEXT:    ret i8 [[TMP0]]
//
// CHECK-LABEL: @_ZN14dependent_typeIcE14decay_constantEU12__capabilityPc(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[ADD_PTR:%.*]] = getelementptr inbounds i8, ptr addrspace(200) [[P:%.*]], i64 1
// CHECK-NEXT:    ret ptr addrspace(200) [[ADD_PTR]]
//
// CHECK-LABEL: @_ZN14dependent_typeIcE15member_constantEU12__capabilityPc(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[ARRAYIDX:%.*]] = getelementptr inbounds i8, ptr addrspace(200) [[P:%.*]], i64 1
// CHECK-NEXT:    ret ptr addrspace(200) [[ARRAYIDX]]
//
// CHECK-LABEL: @_ZN14dependent_typeIcE12sub_constantEU12__capabilityPc(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[ARRAYIDX:%.*]] = getelementptr inbounds i8, ptr addrspace(200) [[P:%.*]], i64 1
// CHECK-NEXT:    [[TMP0:%.*]] = load i8, ptr addrspace(200) [[ARRAYIDX]], align 1
// CHECK-NEXT:    ret i8 [[TMP0]]
//
// CHECK-LABEL: @_ZN14dependent_typeIcE14decay_variableEmPc(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[ADD_PTR:%.*]] = getelementptr inbounds i8, ptr addrspace(200) [[P:%.*]], i64 1
// CHECK-NEXT:    ret ptr addrspace(200) [[ADD_PTR]]
//
// CHECK-LABEL: @_ZN14dependent_typeIcE15member_variableEmPc(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[ARRAYIDX:%.*]] = getelementptr inbounds i8, ptr addrspace(200) [[P:%.*]], i64 1
// CHECK-NEXT:    ret ptr addrspace(200) [[ARRAYIDX]]
//
// CHECK-LABEL: @_ZN14dependent_typeIcE12sub_variableEmPc(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[ARRAYIDX:%.*]] = getelementptr inbounds i8, ptr addrspace(200) [[P:%.*]], i64 1
// CHECK-NEXT:    [[TMP0:%.*]] = load i8, ptr addrspace(200) [[ARRAYIDX]], align 1
// CHECK-NEXT:    ret i8 [[TMP0]]
//
template struct dependent_type<char>;

template <size_t n>
struct dependent_size {
  char * __capability decay(char p[__capability n]) {
    return p + 1;
  }

  char * __capability member(char p[__capability n]) {
    return &p[1];
  }

  char sub(char p[__capability n]) {
    return p[1];
  }
};
// CHECK-LABEL: @_ZN14dependent_sizeILm2EE5decayEU12__capabilityPc(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[ADD_PTR:%.*]] = getelementptr inbounds i8, ptr addrspace(200) [[P:%.*]], i64 1
// CHECK-NEXT:    ret ptr addrspace(200) [[ADD_PTR]]
//
// CHECK-LABEL: @_ZN14dependent_sizeILm2EE6memberEU12__capabilityPc(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[ARRAYIDX:%.*]] = getelementptr inbounds i8, ptr addrspace(200) [[P:%.*]], i64 1
// CHECK-NEXT:    ret ptr addrspace(200) [[ARRAYIDX]]
//
// CHECK-LABEL: @_ZN14dependent_sizeILm2EE3subEU12__capabilityPc(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[ARRAYIDX:%.*]] = getelementptr inbounds i8, ptr addrspace(200) [[P:%.*]], i64 1
// CHECK-NEXT:    [[TMP0:%.*]] = load i8, ptr addrspace(200) [[ARRAYIDX]], align 1
// CHECK-NEXT:    ret i8 [[TMP0]]
//
template struct dependent_size<2>;
