#ifndef NANOVM_INTERNAL_CODE_H
#define NANOVM_INTERNAL_CODE_H 1

#include "code.h"

typedef struct {
  int opcode;    // OP_
  int insn_type; // INSN_
} nvm_opcode_meta_t;

void NVM_CODE_release_type (nvm_ctx_t* ctx, nvm_type_t* type);
void NVM_CODE_release_field(nvm_ctx_t* ctx, nvm_field_t* field);
void NVM_CODE_release_meth (nvm_ctx_t* ctx, nvm_meth_t* meth);
void NVM_CODE_release_insn (nvm_ctx_t* ctx, nvm_insn_t* insn);
void NVM_CODE_release_ex   (nvm_ctx_t* ctx, nvm_ex_t* ex);

#endif
