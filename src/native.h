#ifndef NANOVM_NATIVE_H
#define NANOVM_NATIVE_H 1

#include "util.h"
#include "nanovm.h"
#include "code.h"
#include "stack.h"

typedef struct _nvm_native_mgr_  nvm_native_mgr_t;
typedef struct _nvm_native_meth_ nvm_native_meth_t;

// native_handle_t :: nanovm_t -> frame_t -> native_meth_t -> int
typedef int (*nvm_native_handle_t)(nvm_ctx_t*, nvm_frame_t*, nvm_native_meth_t*);

typedef struct _nvm_native_mgr_ {
                  int meth_len;
  nvm_native_meth_t** meths;
} nvm_native_mgr_t;

typedef struct _nvm_native_meth_ {
          nvm_meth_t* meth;
  nvm_native_handle_t handle;
} nvm_native_meth_t;

 int NanoVM_GLOBAL_init_native();
void NanoVM_GLOBAL_free_native();

 int NanoVM_create_native_mgr (nvm_ctx_t* ctx);
void NanoVM_release_native_mgr(nvm_ctx_t* ctx, nvm_native_mgr_t* native_mgr);

nvm_native_meth_t* NanoVM_get_native_meth(nvm_ctx_t* ctx, nvm_meth_t* meth);

#endif
