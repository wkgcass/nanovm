#ifndef NANOVM_MEMORY_H
#define NANOVM_MEMORY_H 1

#include "util.h"
#include "nanovm.h"
#include "code.h"

typedef struct _nvm_mem_mgr_ {
  nvm_lock_t lock;

  long heap_cap;
  long heap_used;

  long mem_cap;
  long mem_used;
} nvm_mem_mgr_t;

typedef struct _nvm_header_ {
   nvm_lock_t lock;
  nvm_type_t* type;
          int cnt;
} nvm_hdr_t;

typedef nvm_hdr_t nvm_object_t;

typedef struct _nvm_prm_header_ {
  nvm_hdr_t super;
} nvm_prm_hdr_t;

typedef struct _nvm_ref_header_ {
  nvm_hdr_t super;
} nvm_ref_hdr_t;

typedef struct _nvm_arr_header_ {
  nvm_ref_hdr_t super;
            int len;
} nvm_arr_hdr_t;

nvm_mem_mgr_t* NanoVM_create_mem_mgr (nvm_ctx_t* ctx, long heap_cap, long mem_cap);
          void NanoVM_release_mem_mgr(nvm_ctx_t* ctx, nvm_mem_mgr_t* mem_mgr);

 nvm_object_t* NanoVM_alloc_heap(nvm_ctx_t* ctx, nvm_type_t* type);
nvm_arr_hdr_t* NanoVM_alloc_heap_arr(nvm_ctx_t* ctx, nvm_arr_type_t* type, int len);
         void* NanoVM_alloc(nvm_ctx_t* ctx, size_t size);
          void NanoVM_free (nvm_ctx_t* ctx, void*  ptr);

void NanoVM_ref  (nvm_ctx_t* ctx, nvm_object_t* ref);
void NanoVM_unref(nvm_ctx_t* ctx, nvm_object_t* ref);

#endif
