#ifndef NANOVM_INTERNAL_CODE_H
#define NANOVM_INTERNAL_CODE_H 1

#include "code.h"

typedef struct {
  int opcode;    // OP_
  int insn_type; // INSN_
} opcode_meta_t;

void CODE_release_type (type_t* type);
void CODE_release_field(field_t* field);
void CODE_release_meth (meth_t* meth);
void CODE_release_insn (insn_t* insn);
void CODE_release_ex   (ex_t* ex);

#endif
