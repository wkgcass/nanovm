#ifndef NANOVM_MEMORY_H
#define NANOVM_MEMORY_H 1

#include "util.h"
#include "nanovm.internal.h"
#include "code.h"

typedef struct _nvm_mem_mgr_ nvm_mem_mgr_t;

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

nvm_mem_mgr_t* NanoVM_create_mem_mgr (nvm_ctx_t* ctx, size_t heap_cap, size_t mem_cap);
          void NanoVM_release_mem_mgr(nvm_ctx_t* ctx);

 nvm_object_t* NanoVM_alloc_heap(nvm_ctx_t* ctx, nvm_type_t* type);
nvm_arr_hdr_t* NanoVM_alloc_heap_arr(nvm_ctx_t* ctx, nvm_arr_type_t* type, int len);
         void* NanoVM_alloc(nvm_ctx_t* ctx, size_t size);
          void NanoVM_free (nvm_ctx_t* ctx, void*  ptr);

void NanoVM_ref  (nvm_ctx_t* ctx, nvm_object_t* ref);
void NanoVM_unref(nvm_ctx_t* ctx, nvm_object_t* ref);

nvm_object_t* NanoVM_get_null(nvm_ctx_t* ctx);

size_t NanoVM_get_prm_type_size(char prm_type);

void NanoVM_mem_set_prm_i(nvm_prm_hdr_t* prm, int val);
void NanoVM_mem_set_prm_j(nvm_prm_hdr_t* prm, long val);
void NanoVM_mem_set_prm_f(nvm_prm_hdr_t* prm, float val);
void NanoVM_mem_set_prm_d(nvm_prm_hdr_t* prm, double val);

// put field will add ref for the input object and unref for the original object
void NanoVM_mem_put_field(nvm_ctx_t* ctx, nvm_ref_hdr_t* obj, nvm_field_t* field, nvm_object_t* val);
void NanoVM_mem_set_array(nvm_ctx_t* ctx, nvm_arr_hdr_t* arr, int index, nvm_object_t* val);

void NanoVM_mem_get_prm  (nvm_prm_hdr_t* prm, void* result);
void NanoVM_mem_get_field(nvm_ref_hdr_t* obj, nvm_field_t* field, nvm_object_t** result);
void NanoVM_mem_get_array(nvm_arr_hdr_t* arr, int index, nvm_object_t** result);

size_t NanoVM_get_mem_usage (nanovm_t* vm);
size_t NanoVM_get_heap_usage(nanovm_t* vm);
size_t NanoVM_get_mem_cap   (nanovm_t* vm);
size_t NanoVM_get_heap_cap  (nanovm_t* vm);

#endif
