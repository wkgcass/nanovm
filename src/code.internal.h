#ifndef NANOVM_INTERNAL_CODE_H
#define NANOVM_INTERNAL_CODE_H 1

#include "code.h"

typedef struct {
  int opcode;    // OP_
  int insn_type; // INSN_
} opcode_meta_t;

void CODE_release_type (ctx_t* ctx, type_t* type);
void CODE_release_field(ctx_t* ctx, field_t* field);
void CODE_release_meth (ctx_t* ctx, meth_t* meth);
void CODE_release_insn (ctx_t* ctx, insn_t* insn);
void CODE_release_ex   (ctx_t* ctx, ex_t* ex);

#endif
