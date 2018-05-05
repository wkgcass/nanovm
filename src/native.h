#ifndef NANOVM_NATIVE_H
#define NANOVM_NATIVE_H 1

#include "util.h"
#include "nanovm.h"
#include "code.h"
#include "stack.h"

typedef struct _native_mgr_  native_mgr_t;
typedef struct _native_meth_ native_meth_t;

// native_handle_t :: nanovm_t -> frame_t -> native_meth_t -> int
typedef int (*native_handle_t)(nanovm_t*, frame_t*, native_meth_t*);

typedef struct _native_mgr_ {
        nanovm_t* vm;
              int meth_len;
  native_meth_t** meths;
} native_mgr_t;

typedef struct _native_meth_ {
          meth_t* meth;
  native_handle_t handle;
} native_meth_t;

 int NanoVM_GLOBAL_init_native();
void NanoVM_GLOBAL_free_native();

 int NanoVM_create_native_mgr (nanovm_t* vm);
void NanoVM_release_native_mgr(native_mgr_t* native_mgr);

native_meth_t* NanoVM_get_native_meth(nanovm_t* vm, meth_t* meth);

#endif
