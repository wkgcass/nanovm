#ifndef NANOVM_INTERNAL_STACK_H
#define NANOVM_INTERNAL_STACK_H 1

#include "stack.h"

// insn_handle_t :: ctx_t -> frame_t -> insn_t -> int
typedef int (*insn_handle_t)(ctx_t* ctx, frame_t*, insn_t*);

insn_handle_t STACK_get_insn_handle(ctx_t* ctx, insn_t* insn);

void STACK_release_stack(ctx_t* ctx, stack_t* stack);
void STACK_release_frame(ctx_t* ctx, frame_t* frame);

#endif
