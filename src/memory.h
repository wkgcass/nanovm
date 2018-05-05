#ifndef NANOVM_MEMORY_H
#define NANOVM_MEMORY_H 1

#include "util.h"
#include "nanovm.h"
#include "code.h"

typedef struct _mem_mgr_ {
  nanovm_t* vm;
  lock_t lock;

  long heap_cap;
  long heap_used;

  long mem_cap;
  long mem_used;
} mem_mgr_t;

typedef struct _header_ {
   lock_t lock;
  type_t* type;
      int cnt;
} hdr_t;

typedef hdr_t object_t;

typedef struct _prm_header_ {
  hdr_t super;
} prm_hdr_t;

typedef struct _ref_header_ {
  hdr_t super;
} ref_hdr_t;

typedef struct _arr_header_ {
  ref_hdr_t super;
        int len;
} arr_hdr_t;

mem_mgr_t* NanoVM_create_mem_mgr (nanovm_t* vm, long heap_cap, long mem_cap);
      void NanoVM_release_mem_mgr(mem_mgr_t* mem_mgr);

 object_t* NanoVM_alloc_heap(nanovm_t* vm, type_t* type);
arr_hdr_t* NanoVM_alloc_heap_arr(nanovm_t* vm, arr_type_t* type, int len);
     void* NanoVM_alloc(nanovm_t* vm, size_t size);
      void NanoVM_free (nanovm_t* vm, void*  ptr);

void NanoVM_ref  (nanovm_t* vm, object_t* ref);
void NanoVM_unref(nanovm_t* vm, object_t* ref);

#endif
