#ifndef NANOVM_INTERNAL_STACK_H
#define NANOVM_INTERNAL_STACK_H 1

#include "stack.h"

// insn_handle_t :: ctx_t -> frame_t -> insn_t -> int
typedef int (*nvm_insn_handle_t)(nvm_ctx_t* ctx, nvm_frame_t*, nvm_insn_t*);

nvm_insn_handle_t NVM_STACK_get_insn_handle(nvm_ctx_t* ctx, nvm_insn_t* insn);

void NVM_STACK_release_stack(nvm_ctx_t* ctx, nvm_stack_t* stack);
void NVM_STACK_release_frame(nvm_ctx_t* ctx, nvm_frame_t* frame);

#endif
