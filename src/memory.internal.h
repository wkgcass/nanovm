#ifndef NANOVM_MEMORY_INTERNAL_H
#define NANOVM_MEMORY_INTERNAL_H 1

#include "memory.h"

typedef struct _nvm_mem_mgr_ {
  nvm_lock_t lock;

  size_t heap_cap;
  size_t heap_used;

  size_t mem_cap;
  size_t mem_used;

  nvm_hdr_t* _null_val;
  nvm_prm_hdr_t* _int_0;
  nvm_prm_hdr_t* _long_0;
  nvm_prm_hdr_t* _float_0;
  nvm_prm_hdr_t* _double_0;
  nvm_prm_hdr_t* _short_0;
  nvm_prm_hdr_t* _byte_0;
  nvm_prm_hdr_t* _char_0;
  nvm_prm_hdr_t* _bool_0;
} nvm_mem_mgr_t;

#endif
