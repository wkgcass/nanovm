#ifndef NANOVM_CODE_H
#define NANOVM_CODE_H 1

#include <stddef.h>
#include "util.h"
#include "nanovm.h"

// we only consider these acc flags
#define ACC_STATIC   0x0008
#define ACC_NATIVE   0x0100

#define TYPE_PRM 1
#define TYPE_REF 2
#define TYPE_ARR 3

#define PRM_TYPE_NULL    0
#define PRM_TYPE_INT     'I'
#define PRM_TYPE_FLOAT   'F'
#define PRM_TYPE_LONG    'J'
#define PRM_TYPE_DOUBLE  'D'
#define PRM_TYPE_SHORT   'S'
#define PRM_TYPE_BYTE    'B'
#define PRM_TYPE_CHAR    'C'
#define PRM_TYPE_BOOLEAN 'Z'
#define PRM_TYPE_VOID    'V'
// though void is not a primitive but it behaves
// the same as primitives when processing the type system

// forward declaration
// memory.h
typedef struct _nvm_header_ nvm_hdr_t;
typedef nvm_hdr_t nvm_object_t;

typedef struct _nvm_code_mgr_  nvm_code_mgr_t;
typedef struct _nvm_type_      nvm_type_t;
typedef struct _nvm_prm_type_  nvm_prm_type_t;
typedef struct _nvm_ref_type_  nvm_ref_type_t;
typedef struct _nvm_arr_type_  nvm_arr_type_t;
typedef struct _nvm_field_     nvm_field_t;
typedef struct _nvm_meth_      nvm_meth_t;
typedef struct _nvm_insn_      nvm_insn_t;
typedef struct _nvm_ex_        nvm_ex_t;

typedef struct _nvm_code_mgr_ {
           int type_cap;
           int type_len;
  nvm_type_t** types;
} nvm_code_mgr_t;

typedef struct _nvm_type_ {
  char cat; // category
} nvm_type_t;

typedef struct _nvm_prm_type_ {
  nvm_type_t super;
        char prm_type;
      size_t size;
} nvm_prm_type_t;

typedef struct _nvm_ref_type_ {
     nvm_type_t super;

    nvm_type_t* parent_type;
          char* name;
            int field_len;
  nvm_field_t** fields;
            int meth_len;
   nvm_meth_t** meths;

         size_t size;
} nvm_ref_type_t;

typedef struct _nvm_arr_type_ {
   nvm_type_t super;

  nvm_type_t* comp_type;
} nvm_arr_type_t;

typedef struct _nvm_field_ {
    nvm_type_t* dec_type;

            int acc;
          char* name;
    nvm_type_t* type;
  nvm_object_t* static_v;
} field_t;

typedef struct _nvm_meth_ {
   nvm_type_t* dec_type;

           int acc;
         char* name;
   nvm_type_t* ret_type;
           int param_len;
  nvm_type_t** param_types;
           int max_stack;  // op stack size
           int max_locals; // local variable table size
           int insn_len;
  nvm_insn_t** insns;
           int ex_len;
    nvm_ex_t** exs;
} nvm_meth_t;

typedef struct _nvm_insn_ {
          int idx; // index of the insn in meth_t
          int off; // offset of code attribute

  nvm_meth_t* meth;
          int line; // source code line
          int opcode;
          int type; // defined in code.internal.h
} nvm_insn_t;

typedef struct _nvm_ex_ {
  nvm_meth_t* meth;
  nvm_insn_t* start; // inclusive
  nvm_insn_t* end;   // exclusive, maybe null (which means the end)
  nvm_insn_t* handler;
  nvm_type_t* catch_type;
} nvm_ex_t;

            int NanoVM_GLOBAL_init_code();
           void NanoVM_GLOBAL_free_code();
nvm_code_mgr_t* NanoVM_create_code_mgr (nvm_ctx_t* ctx, int type_cap);
           void NanoVM_release_code_mgr(nvm_ctx_t* ctx, nvm_code_mgr_t* code_mgr);

         int NanoVM_parse_code0(nvm_ctx_t* ctx, char** bytecode);
 nvm_meth_t* NanoVM_get_meth   (nvm_ctx_t* ctx, nvm_ref_type_t* ref_type, char* name, nvm_type_t* ret_type, int param_len, nvm_type_t** param_types);
nvm_field_t* NanoVM_get_field  (nvm_ctx_t* ctx, nvm_ref_type_t* ref_type, char* name);

nvm_prm_type_t* NanoVM_get_prm_type(nvm_ctx_t* ctx, char prm); // e.g. Z -> boolean
nvm_ref_type_t* NanoVM_get_ref_type(nvm_ctx_t* ctx, char* name);
nvm_arr_type_t* NanoVM_get_arr_type(nvm_ctx_t* ctx, nvm_type_t* comp_type);

// -----BEGIN instructions category-----
// same as java asm lib
// but we don't support invokedynamic

#define INSN_PLAIN  0
#define INSN_INT    1
#define INSN_VAR    2
#define INSN_TYPE   3
#define INSN_FIELD  4
#define INSN_METH   5
#define INSN_JMP    6
#define INSN_LDC    7
#define INSN_IINC   8
#define INSN_TBL    9
#define INSN_LKUP  10
#define INSN_MAARR 11

typedef nvm_insn_t nvm_insn_plain_t;

typedef struct {
  nvm_insn_t super;
         int operand;
} nvm_insn_int_t;

typedef struct {
  nvm_insn_t super;
         int var;
} nvm_insn_var_t;

typedef struct {
   nvm_insn_t super;
  nvm_type_t* type;
} nvm_insn_type_t;

typedef struct {
    nvm_insn_t super;
  nvm_field_t* field;
} nvm_insn_field_t;

typedef struct {
   nvm_insn_t super;
  nvm_meth_t* meth;
} nvm_insn_meth_t;

typedef struct {
   nvm_insn_t super;
  nvm_insn_t* target;
} nvm_insn_jmp_t;

typedef struct {
     nvm_insn_t super;
  nvm_object_t* obj;
} nvm_insn_ldc_t;

typedef struct {
  nvm_insn_t super;
         int var;
         int incr;
} nvm_insn_iinc_t;

typedef struct {
    nvm_insn_t super;
           int low;
           int high;
  nvm_insn_t** targets;
} nvm_insn_tbl_t; // table switch

typedef struct {
          int key;
  nvm_insn_t* target;
} nvm_lkup_pair_t;

typedef struct {
        nvm_insn_t super;
               int pair_len;
  nvm_lkup_pair_t* pairs;
} nvm_insn_lkup_t; // lookup switch

typedef struct {
       nvm_insn_t super;
  nvm_arr_type_t* arr_type;
              int dim; //dimensions
} nvm_insn_m_a_arr_t; // multi a new array

// -----END instructions category-----

// -----BEGIN opcode-----
// formatted from zxh0/classpy com.github.zxh.classpy.classfile.jvm.Opcode
// 2018-05-05
// using the following script (Node.js)
// var fs = require('fs');
// var lines = fs.readFileSync('tmp', 'utf8').split('\n').map(v=>v.trim()).filter(v=>!!v && !v.startsWith('//'));
// var res = lines.map(s => [s.substring(0, s.indexOf(' ')), s.substring(s.indexOf('(') + 1)]).map(v => [v[0], v[1].substring(0, v[1].indexOf(','))]).map(v => '#define OP_' + v[0].toUpperCase()+ ' ' + v[1]).join('\n');
// console.log(res);

#define OP_NOP 0x00
#define OP_ACONST_NULL 0x01
#define OP_ICONST_M1 0x02
#define OP_ICONST_0 0x03
#define OP_ICONST_1 0x04
#define OP_ICONST_2 0x05
#define OP_ICONST_3 0x06
#define OP_ICONST_4 0x07
#define OP_ICONST_5 0x08
#define OP_LCONST_0 0x09
#define OP_LCONST_1 0x0a
#define OP_FCONST_0 0x0b
#define OP_FCONST_1 0x0c
#define OP_FCONST_2 0x0d
#define OP_DCONST_0 0x0e
#define OP_DCONST_1 0x0f
#define OP_BIPUSH 0x10
#define OP_SIPUSH 0x11
#define OP_LDC 0x12
#define OP_LDC_W 0x13
#define OP_LDC2_W 0x14
#define OP_ILOAD 0x15
#define OP_LLOAD 0x16
#define OP_FLOAD 0x17
#define OP_DLOAD 0x18
#define OP_ALOAD 0x19
#define OP_ILOAD_0 0x1a
#define OP_ILOAD_1 0x1b
#define OP_ILOAD_2 0x1c
#define OP_ILOAD_3 0x1d
#define OP_LLOAD_0 0x1e
#define OP_LLOAD_1 0x1f
#define OP_LLOAD_2 0x20
#define OP_LLOAD_3 0x21
#define OP_FLOAD_0 0x22
#define OP_FLOAD_1 0x23
#define OP_FLOAD_2 0x24
#define OP_FLOAD_3 0x25
#define OP_DLOAD_0 0x26
#define OP_DLOAD_1 0x27
#define OP_DLOAD_2 0x28
#define OP_DLOAD_3 0x29
#define OP_ALOAD_0 0x2a
#define OP_ALOAD_1 0x2b
#define OP_ALOAD_2 0x2c
#define OP_ALOAD_3 0x2d
#define OP_IALOAD 0x2e
#define OP_LALOAD 0x2f
#define OP_FALOAD 0x30
#define OP_DALOAD 0x31
#define OP_AALOAD 0x32
#define OP_BALOAD 0x33
#define OP_CALOAD 0x34
#define OP_SALOAD 0x35
#define OP_ISTORE 0x36
#define OP_LSTORE 0x37
#define OP_FSTORE 0x38
#define OP_DSTORE 0x39
#define OP_ASTORE 0x3a
#define OP_ISTORE_0 0x3b
#define OP_ISTORE_1 0x3c
#define OP_ISTORE_2 0x3d
#define OP_ISTORE_3 0x3e
#define OP_LSTORE_0 0x3f
#define OP_LSTORE_1 0x40
#define OP_LSTORE_2 0x41
#define OP_LSTORE_3 0x42
#define OP_FSTORE_0 0x43
#define OP_FSTORE_1 0x44
#define OP_FSTORE_2 0x45
#define OP_FSTORE_3 0x46
#define OP_DSTORE_0 0x47
#define OP_DSTORE_1 0x48
#define OP_DSTORE_2 0x49
#define OP_DSTORE_3 0x4a
#define OP_ASTORE_0 0x4b
#define OP_ASTORE_1 0x4c
#define OP_ASTORE_2 0x4d
#define OP_ASTORE_3 0x4e
#define OP_IASTORE 0x4f
#define OP_LASTORE 0x50
#define OP_FASTORE 0x51
#define OP_DASTORE 0x52
#define OP_AASTORE 0x53
#define OP_BASTORE 0x54
#define OP_CASTORE 0x55
#define OP_SASTORE 0x56
#define OP_POP 0x57
#define OP_POP2 0x58
#define OP_DUP 0x59
#define OP_DUP_X1 0x5a
#define OP_DUP_X2 0x5b
#define OP_DUP2 0x5c
#define OP_DUP2_X1 0x5d
#define OP_DUP2_X2 0x5e
#define OP_SWAP 0x5f
#define OP_IADD 0x60
#define OP_LADD 0x61
#define OP_FADD 0x62
#define OP_DADD 0x63
#define OP_ISUB 0x64
#define OP_LSUB 0x65
#define OP_FSUB 0x66
#define OP_DSUB 0x67
#define OP_IMUL 0x68
#define OP_LMUL 0x69
#define OP_FMUL 0x6a
#define OP_DMUL 0x6b
#define OP_IDIV 0x6c
#define OP_LDIV 0x6d
#define OP_FDIV 0x6e
#define OP_DDIV 0x6f
#define OP_IREM 0x70
#define OP_LREM 0x71
#define OP_FREM 0x72
#define OP_DREM 0x73
#define OP_INEG 0x74
#define OP_LNEG 0x75
#define OP_FNEG 0x76
#define OP_DNEG 0x77
#define OP_ISHL 0x78
#define OP_LSHL 0x79
#define OP_ISHR 0x7a
#define OP_LSHR 0x7b
#define OP_IUSHR 0x7c
#define OP_LUSHR 0x7d
#define OP_IAND 0x7e
#define OP_LAND 0x7f
#define OP_IOR 0x80
#define OP_LOR 0x81
#define OP_IXOR 0x82
#define OP_LXOR 0x83
#define OP_IINC 0x84
#define OP_I2L 0x85
#define OP_I2F 0x86
#define OP_I2D 0x87
#define OP_L2I 0x88
#define OP_L2F 0x89
#define OP_L2D 0x8a
#define OP_F2I 0x8b
#define OP_F2L 0x8c
#define OP_F2D 0x8d
#define OP_D2I 0x8e
#define OP_D2L 0x8f
#define OP_D2F 0x90
#define OP_I2B 0x91
#define OP_I2C 0x92
#define OP_I2S 0x93
#define OP_LCMP 0x94
#define OP_FCMPL 0x95
#define OP_FCMPG 0x96
#define OP_DCMPL 0x97
#define OP_DCMPG 0x98
#define OP_IFEQ 0x99
#define OP_IFNE 0x9a
#define OP_IFLT 0x9b
#define OP_IFGE 0x9c
#define OP_IFGT 0x9d
#define OP_IFLE 0x9e
#define OP_IF_ICMPEQ 0x9f
#define OP_IF_ICMPNE 0xa0
#define OP_IF_ICMPLT 0xa1
#define OP_IF_ICMPGE 0xa2
#define OP_IF_ICMPGT 0xa3
#define OP_IF_ICMPLE 0xa4
#define OP_IF_ACMPEQ 0xa5
#define OP_IF_ACMPNE 0xa6
#define OP__GOTO 0xa7
#define OP_JSR 0xa8
#define OP_RET 0xa9
#define OP_TABLESWITCH 0xaa
#define OP_LOOKUPSWITCH 0xab
#define OP_IRETURN 0xac
#define OP_LRETURN 0xad
#define OP_FRETURN 0xae
#define OP_DRETURN 0xaf
#define OP_ARETURN 0xb0
#define OP__RETURN 0xb1
#define OP_GETSTATIC 0xb2
#define OP_PUTSTATIC 0xb3
#define OP_GETFIELD 0xb4
#define OP_PUTFIELD 0xb5
#define OP_INVOKEVIRTUAL 0xb6
#define OP_INVOKESPECIAL 0xb7
#define OP_INVOKESTATIC 0xb8
#define OP_INVOKEINTERFACE 0xb9
#define OP_INVOKEDYNAMIC 0xba
#define OP__NEW 0xbb
#define OP_NEWARRAY 0xbc
#define OP_ANEWARRAY 0xbd
#define OP_ARRAYLENGTH 0xbe
#define OP_ATHROW 0xbf
#define OP_CHECKCAST 0xc0
#define OP__INSTANCEOF 0xc1
#define OP_MONITORENTER 0xc2
#define OP_MONITOREXIT 0xc3
#define OP_WIDE 0xc4
#define OP_MULTIANEWARRAY 0xc5
#define OP_IFNULL 0xc6
#define OP_IFNONNULL 0xc7
#define OP_GOTO_W 0xc8
#define OP_JSR_W 0xc9
#define OP_BREAKPOINT 0xca
#define OP_IMPDEP1 0xfe
#define OP_IMPDEP2 0xff
// -----END opcode-----

#endif
