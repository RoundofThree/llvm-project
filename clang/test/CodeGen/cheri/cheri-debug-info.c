// REQUIRES: asserts, mips-registered-target
// RUN: %cheri_purecap_cc1 -mllvm -cheri-cap-table-abi=pcrel "-debug-info-kind=standalone" "-dwarf-version=2" "-debugger-tuning=gdb"  -emit-llvm -O0 -o - %s 
// RUN: %cheri_purecap_cc1 -mllvm -cheri-cap-table-abi=pcrel "-debug-info-kind=standalone" "-dwarf-version=2" "-debugger-tuning=gdb"  -emit-llvm -O0 -o - %s | FileCheck %s -check-prefix CHECK-IR
// RUN: %cheri_cc1 -mrelocation-model pic "-debug-info-kind=standalone" "-dwarf-version=2" "-debugger-tuning=gdb"  -emit-llvm -O0 -o - %s | FileCheck %s -check-prefix CHECK-IR

// RUN: %cheri_purecap_cc1 -mllvm -cheri-cap-table-abi=pcrel "-debug-info-kind=standalone" "-dwarf-version=2" "-debugger-tuning=gdb"  -emit-obj -O0 -o %t-cheri.o %s
// RUN: %cheri_cc1 -mllvm -cheri-cap-table-abi=pcrel "-debug-info-kind=standalone" "-dwarf-version=2" "-debugger-tuning=gdb"  -emit-obj -O0 -o %t-cheri.o %s

// Generate a .o file and compare the debug info:
// RUN: %cheri_purecap_cc1 -mllvm -cheri-cap-table-abi=pcrel "-debug-info-kind=standalone" "-dwarf-version=2" "-debugger-tuning=gdb"  -emit-obj -O0 -o %t-cheri.o %s -mllvm -debug-only=dwarfdebug
// RUN: %clang_cc1 -triple=mips64-unknown-freebsd "-debug-info-kind=standalone" "-dwarf-version=2" "-debugger-tuning=gdb"  -emit-obj -O0 -o %t-mips.o %s
// RUN: %cheri_cc1 "-debug-info-kind=standalone" "-dwarf-version=2" "-debugger-tuning=gdb"  -emit-obj -O0 -o %t-hybrid.o %s

// Check that hybrid and MIPS generated asm is the same:
// RUN: %clang_cc1 -triple=mips64-unknown-freebsd "-debug-info-kind=standalone" "-dwarf-version=2" "-debugger-tuning=gdb" -mstack-alignment=32  -S -O0 -o %t-mips.s %s
// RUN: %cheri_cc1 "-debug-info-kind=standalone" "-dwarf-version=2" "-debugger-tuning=gdb" -mstack-alignment=32 -S -O0 -o %t-hybrid.s %s
// Should be the same other than some ELF flags:
// RUN: diff -u %t-mips.s %t-hybrid.s

int foo(int* i) { // CHECK-IR:  call void @llvm.dbg.declare(metadata ptr{{( addrspace\(200\))?}} %i.addr, metadata !14, metadata !DIExpression()), !dbg !15
	if (i) {
		int j = 2; // CHECK-IR: call void @llvm.dbg.declare(metadata ptr{{( addrspace\(200\))?}} %j, metadata !19, metadata !DIExpression()), !dbg !21
	}
	else {
		int j = 3;  // CHECK-IR: call void @llvm.dbg.declare(metadata ptr{{( addrspace\(200\))?}} %j1, metadata !23, metadata !DIExpression()), !dbg !25
	}
	return (int)i;
// CHECK-IR: ret i32 %{{.+}}, !dbg !28
// Both MIPS and purecap should have the same number of metadata nodes:
// CHECK-IR: !12 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !3, size: {{64|128|256}})
// CHECK-IR: !14 = !DILocalVariable(name: "i", arg: 1, scope: !8, file: !9, line: [[#FOO_DECL_LINE:]], type: !12)
// check that we have both j variables listed:
// CHECK-IR: !19 = !DILocalVariable(name: "j", scope: !20, file: !9, line: [[#FOO_DECL_LINE + 2]], type: !3)
// CHECK-IR: !20 = distinct !DILexicalBlock(scope: !17, file: !9, line: [[#FOO_DECL_LINE + 1]], column: 9)
// CHECK-IR: !23 = !DILocalVariable(name: "j", scope: !24, file: !9, line: [[#FOO_DECL_LINE + 5]], type: !3)
// CHECK-IR: !24 = distinct !DILexicalBlock(scope: !17, file: !9, line: [[#FOO_DECL_LINE + 4]], column: 7)
// CHECK-IR: !28 = !DILocation(line: [[#FOO_DECL_LINE + 7]], column: 2, scope: !8)
}

// Previoulsy llvm-dwarfdump would not handle MIPS relocations for a CHERI triple:
// RUN: llvm-dwarfdump -verify %t-mips.o 2>&1 | FileCheck %s -check-prefix DWARFDUMP-VERIFY
// RUN: llvm-dwarfdump -verify %t-hybrid.o 2>&1 | FileCheck %s -check-prefix DWARFDUMP-VERIFY
// RUN: llvm-dwarfdump -verify %t-cheri.o 2>&1 | FileCheck %s -check-prefix DWARFDUMP-VERIFY
// DWARFDUMP-VERIFY-NOT: failed to compute relocation
// DWARFDUMP-VERIFY: No errors.
// DWARFDUMP-VERIFY-NOT: failed to compute relocation

// Now comapre the dwarfdump output for CHERI and MIPS:
// RUN: llvm-dwarfdump -debug-line -debug-info %t-mips.o | FileCheck %s -check-prefixes DEBUG-INFO,MIPS-DEBUG-INFO
// RUN: llvm-dwarfdump -debug-line -debug-info %t-cheri.o | FileCheck %s -check-prefixes DEBUG-INFO,CHERI-DEBUG-INFO

// Previously the CHERI debug info didn't include either of the J variables:

// DEBUG-INFO-LABEL: .debug_info contents:
// DEBUG-INFO-NEXT: 0x00000000: Compile Unit:
// DEBUG-INFO-EMPTY:
// DEBUG-INFO-NEXT: DW_TAG_compile_unit
// DEBUG-INFO-NEXT:               DW_AT_producer	("clang version
// DEBUG-INFO-NEXT:               DW_AT_language	(DW_LANG_C99)
// DEBUG-INFO-NEXT:               DW_AT_name	("
// DEBUG-INFO-NEXT:               DW_AT_stmt_list	(0x00000000)
// DEBUG-INFO-NEXT:               DW_AT_comp_dir	("
// DEBUG-INFO-NEXT:               DW_AT_low_pc	(0x0000000000000000)
// DEBUG-INFO-NEXT:               DW_AT_high_pc	(0x00000000000{{.+}})
// DEBUG-INFO-EMPTY:
// DEBUG-INFO-NEXT: [[INT_TYPE_INFO_ADDR:0x0000002e]]: DW_TAG_base_type
// DEBUG-INFO-NEXT:                 DW_AT_name	("int")
// DEBUG-INFO-NEXT:                 DW_AT_encoding	(DW_ATE_signed)
// DEBUG-INFO-NEXT:                 DW_AT_byte_size	(0x04)
// DEBUG-INFO-EMPTY:
// DEBUG-INFO-NEXT: DW_TAG_subprogram
// DEBUG-INFO-NEXT:                 DW_AT_low_pc	(0x0000000000000000)
// DEBUG-INFO-NEXT:                 DW_AT_high_pc	(0x00000000000000{{.+}})
// MIPS-DEBUG-INFO-NEXT:            DW_AT_frame_base	(DW_OP_reg29 SP_64)
// CHERI-DEBUG-INFO-NEXT:           DW_AT_frame_base	(DW_OP_regx C11)
// DEBUG-INFO-NEXT:                 DW_AT_name	("foo")
// DEBUG-INFO-NEXT:                 DW_AT_decl_file	("{{.+}}/test/CodeGen/cheri/cheri-debug-info.c")
// DEBUG-INFO-NEXT:                 DW_AT_decl_line	([[#FOO_DECL_LINE:]])
// DEBUG-INFO-NEXT:                 DW_AT_prototyped	(0x01)
// DEBUG-INFO-NEXT:                 DW_AT_type	([[INT_TYPE_INFO_ADDR]] "int")
// DEBUG-INFO-NEXT:                 DW_AT_external	(0x01)
// DEBUG-INFO-EMPTY:
// DEBUG-INFO-NEXT: DW_TAG_formal_parameter
// DEBUG-INFO-NEXT:                   DW_AT_location	(DW_OP_fbreg +{{.+}})
// DEBUG-INFO-NEXT:                   DW_AT_name	("i")
// DEBUG-INFO-NEXT:                   DW_AT_decl_file	("{{.+}}/test/CodeGen/cheri/cheri-debug-info.c")
// DEBUG-INFO-NEXT:                   DW_AT_decl_line	([[#FOO_DECL_LINE]])
// DEBUG-INFO-NEXT:                   DW_AT_type	([[INTPTR_TYPE_INFO_ADDR:0x000.+]] "int *")
// DEBUG-INFO-EMPTY:
// DEBUG-INFO-NEXT: DW_TAG_lexical_block
// DEBUG-INFO-NEXT:                   DW_AT_low_pc	(0x00000000000000{{.+}})
// DEBUG-INFO-NEXT:                   DW_AT_high_pc	(0x00000000000000{{.+}})
// DEBUG-INFO-EMPTY:
// DEBUG-INFO-NEXT: DW_TAG_variable
// DEBUG-INFO-NEXT:                     DW_AT_location	(DW_OP_fbreg +{{.+}})
// DEBUG-INFO-NEXT:                     DW_AT_name	("j")
// DEBUG-INFO-NEXT:                     DW_AT_decl_file	("{{.+}}/test/CodeGen/cheri/cheri-debug-info.c")
// DEBUG-INFO-NEXT:                     DW_AT_decl_line	([[#FOO_DECL_LINE + 2]])
// DEBUG-INFO-NEXT:                     DW_AT_type	([[INT_TYPE_INFO_ADDR]] "int")
// DEBUG-INFO-EMPTY:
// DEBUG-INFO-NEXT:  NULL
// DEBUG-INFO-EMPTY:
// DEBUG-INFO-NEXT:  DW_TAG_lexical_block
// DEBUG-INFO-NEXT:                   DW_AT_low_pc	(0x00000000000000{{.+}})
// DEBUG-INFO-NEXT:                   DW_AT_high_pc	(0x00000000000000{{.+}})
// DEBUG-INFO-EMPTY:
// DEBUG-INFO-NEXT:  DW_TAG_variable
// DEBUG-INFO-NEXT:                     DW_AT_location	(DW_OP_fbreg +{{.+}})
// DEBUG-INFO-NEXT:                     DW_AT_name	("j")
// DEBUG-INFO-NEXT:                     DW_AT_decl_file	("{{.+}}/CodeGen/cheri/cheri-debug-info.c")
// DEBUG-INFO-NEXT:                     DW_AT_decl_line	([[#FOO_DECL_LINE + 5]])
// DEBUG-INFO-NEXT:                     DW_AT_type	([[INT_TYPE_INFO_ADDR]] "int")
// DEBUG-INFO-EMPTY:
// DEBUG-INFO-NEXT:   NULL
// DEBUG-INFO-EMPTY:
// DEBUG-INFO-NEXT:   NULL
// DEBUG-INFO-EMPTY:
// DEBUG-INFO-NEXT:  [[INTPTR_TYPE_INFO_ADDR]]:  DW_TAG_pointer_type
// DEBUG-INFO-NEXT:                 DW_AT_type	([[INT_TYPE_INFO_ADDR]] "int")
// CHERI includes the type size since it is non-default:
// CHERI-DEBUG-INFO-NEXT:           DW_AT_byte_size ({{0x10|0x20}})
// DEBUG-INFO-EMPTY:
// DEBUG-INFO-NEXT:   NULL
// DEBUG-INFO-EMPTY:


// Check the optimized debug info (should no longer have the two j variables but should have a register for i:

// RUN: %cheri_purecap_cc1 -mllvm -cheri-cap-table-abi=pcrel "-debug-info-kind=standalone" "-dwarf-version=2" "-debugger-tuning=gdb" \
// RUN: -emit-obj -O2 -o - %s | llvm-dwarfdump -debug-info - | FileCheck %s -check-prefixes DEBUG-INFO-OPT,CHERI-DEBUG-INFO-OPT
// RUN: %cheri_cc1 -mllvm -cheri-cap-table-abi=pcrel "-debug-info-kind=standalone" "-dwarf-version=2" "-debugger-tuning=gdb" \
// RUN: -emit-obj -O2 -o - %s | llvm-dwarfdump -debug-info - | FileCheck %s -check-prefixes DEBUG-INFO-OPT,MIPS-DEBUG-INFO-OPT

// DEBUG-INFO-OPT-LABEL: .debug_info contents:
// DEBUG-INFO-OPT-NEXT: 0x00000000: Compile Unit:
// DEBUG-INFO-OPT-EMPTY:
// DEBUG-INFO-OPT-NEXT: 0x0000000b: DW_TAG_compile_unit
// DEBUG-INFO-OPT-NEXT:               DW_AT_producer	("clang
// DEBUG-INFO-OPT-NEXT:               DW_AT_language	(DW_LANG_C99)
// DEBUG-INFO-OPT-NEXT:               DW_AT_name	("{{.+}}/test/CodeGen/cheri/<stdin>")
// DEBUG-INFO-OPT-NEXT:               DW_AT_stmt_list	(0x00000000)
// DEBUG-INFO-OPT-NEXT:               DW_AT_comp_dir	("{{.+}}")
// DEBUG-INFO-OPT-NEXT:               DW_AT_low_pc	(0x0000000000000000)
// MIPS has one instruction less
// CHERI-DEBUG-INFO-OPT-NEXT:         DW_AT_high_pc	(0x000000000000000c)
// MIPS-DEBUG-INFO-OPT-NEXT:          DW_AT_high_pc	(0x0000000000000008)
// DEBUG-INFO-OPT-EMPTY:
// DEBUG-INFO-OPT-NEXT: [[INT_ADDR:0x0000002e]]:   DW_TAG_base_type
// DEBUG-INFO-OPT-NEXT:                 DW_AT_name	("int")
// DEBUG-INFO-OPT-NEXT:                 DW_AT_encoding	(DW_ATE_signed)
// DEBUG-INFO-OPT-NEXT:                 DW_AT_byte_size	(0x04)
// DEBUG-INFO-OPT-EMPTY:
// DEBUG-INFO-OPT-NEXT: 0x00000035:   DW_TAG_subprogram
// DEBUG-INFO-OPT-NEXT:                 DW_AT_low_pc	(0x0000000000000000)
// MIPS has one instruction less
// CHERI-DEBUG-INFO-OPT-NEXT:           DW_AT_high_pc	(0x000000000000000c)
// MIPS-DEBUG-INFO-OPT-NEXT:            DW_AT_high_pc	(0x0000000000000008)
// CHERI-DEBUG-INFO-OPT-NEXT:           DW_AT_frame_base	(DW_OP_regx C11)
// MIPS-DEBUG-INFO-OPT-NEXT:            DW_AT_frame_base	(DW_OP_reg29 SP_64)
// DEBUG-INFO-OPT-NEXT:                 DW_AT_name	("foo")
// DEBUG-INFO-OPT-NEXT:                 DW_AT_decl_file	("{{.+}}cheri-debug-info.c")
// DEBUG-INFO-OPT-NEXT:                 DW_AT_decl_line	([[#FOO_DECL_LINE:]])
// DEBUG-INFO-OPT-NEXT:                 DW_AT_prototyped	(0x01)
// DEBUG-INFO-OPT-NEXT:                 DW_AT_type	([[INT_ADDR]] "int")
// DEBUG-INFO-OPT-NEXT:                 DW_AT_external	(0x01)
// DEBUG-INFO-OPT-EMPTY:
// DEBUG-INFO-OPT-NEXT: 0x000000{{.+}}: DW_TAG_formal_parameter
// MIPS-DEBUG-INFO-OPT-NEXT:                    DW_AT_location  (DW_OP_reg4 A0_64)
// CHERI-DEBUG-INFO-OPT-NEXT:                   DW_AT_location	(DW_OP_regx C3)
// DEBUG-INFO-OPT-NEXT:                         DW_AT_name	("i")
// DEBUG-INFO-OPT-NEXT:                         DW_AT_decl_file	("{{.+}}cheri-debug-info.c")
// DEBUG-INFO-OPT-NEXT:                         DW_AT_decl_line	([[#FOO_DECL_LINE]])
// DEBUG-INFO-OPT-NEXT:                         DW_AT_type	([[INT_PTR_INFO_LOC:0x0000006(2|4)]] "int *")
// DEBUG-INFO-OPT-EMPTY:
// DEBUG-INFO-OPT-NEXT: 0x000000{{.+}}:     NULL
// DEBUG-INFO-OPT-EMPTY:
// DEBUG-INFO-OPT-NEXT: [[INT_PTR_INFO_LOC]]:   DW_TAG_pointer_type
// DEBUG-INFO-OPT-NEXT:                 DW_AT_type	([[INT_ADDR]] "int")
// CHERI-DEBUG-INFO-OPT-NEXT:           DW_AT_byte_size	(0x{{10|20}})
// DEBUG-INFO-OPT-EMPTY:
// DEBUG-INFO-OPT-NEXT: 0x0000006{{.+}}:   NULL
