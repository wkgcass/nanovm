#include "memory.internal.h"

// forward declaration
void _free_heap(nvm_ctx_t* ctx, void* ptr);

nvm_hdr_t* _get_null(nvm_ctx_t* ctx) {
  nvm_mem_mgr_t* mgr = ctx->vm->mem_mgr;
  if (mgr->_null_val) {
    return mgr->_null_val;
  }
  NanoVM_lock(&mgr->lock, ctx->tid);
  if (mgr->_null_val) {
    NanoVM_unlock(&mgr->lock, ctx->tid);
    return mgr->_null_val;
  }
  nvm_type_t* null_type = NanoVM_alloc(ctx, sizeof(nvm_type_t));
  if (!null_type) {
    NanoVM_unlock(&mgr->lock, ctx->tid);
    return NULL;
  }
  null_type->cat = NVM_TYPE_NULL;
  nvm_hdr_t* null_value = NanoVM_alloc(ctx, sizeof(nvm_hdr_t));
  if (null_value) {
    null_value->type = null_type;
    null_value->cnt = 0;
    mgr->_null_val = null_value;
  } else {
    NanoVM_free(ctx, null_type);
  }
  NanoVM_unlock(&mgr->lock, ctx->tid);
  return mgr->_null_val;
}

void _release_null(nvm_ctx_t* ctx) {
  nvm_mem_mgr_t* mgr = ctx->vm->mem_mgr;
  if (mgr->_null_val) {
    NanoVM_free(ctx, mgr->_null_val);
    mgr->_null_val = NULL;
  }
}

#include <stdio.h>

nvm_prm_hdr_t* _get_prm_0(nvm_ctx_t* ctx, nvm_prm_type_t* type) {
  nvm_mem_mgr_t* mgr = ctx->vm->mem_mgr;
  nvm_prm_hdr_t** hdr_pos;
  switch (type->prm_type) {
    case NVM_PRM_TYPE_INT:
      hdr_pos = &mgr->_int_0;
      break;
    case NVM_PRM_TYPE_LONG:
      hdr_pos = &mgr->_long_0;
      break;
    case NVM_PRM_TYPE_FLOAT:
      hdr_pos = &mgr->_float_0;
      break;
    case NVM_PRM_TYPE_DOUBLE:
      hdr_pos = &mgr->_double_0;
      break;
    case NVM_PRM_TYPE_BYTE:
      hdr_pos = &mgr->_byte_0;
      break;
    case NVM_PRM_TYPE_CHAR:
      hdr_pos = &mgr->_char_0;
      break;
    case NVM_PRM_TYPE_SHORT:
      hdr_pos = &mgr->_short_0;
      break;
    case NVM_PRM_TYPE_BOOLEAN:
      hdr_pos = &mgr->_bool_0;
      break;
    default:
      NanoVM_error_log1("unknown primitive type %c", type->prm_type);
      return NULL;
  }
  if (*hdr_pos) {
    return *hdr_pos;
  }
  NanoVM_lock(&mgr->lock, ctx->tid);
  if (*hdr_pos) {
    NanoVM_unlock(&mgr->lock, ctx->tid);
    return *hdr_pos;
  }
  nvm_prm_hdr_t* prm = (nvm_prm_hdr_t*)NanoVM_alloc_heap(ctx, (nvm_type_t*)type);
  if (!prm) {
    NanoVM_unlock(&mgr->lock, ctx->tid);
    return NULL;
  }
  *hdr_pos = prm;
  NanoVM_unlock(&mgr->lock, ctx->tid);
  return *hdr_pos;
}

void _release_prm_0(nvm_ctx_t* ctx) {
  nvm_mem_mgr_t* mgr = ctx->vm->mem_mgr;
  if (mgr->_int_0) {
    _free_heap(ctx, mgr->_int_0);
    mgr->_int_0 = NULL;
  }
  if (mgr->_float_0) {
    _free_heap(ctx, mgr->_float_0);
    mgr->_float_0 = NULL;
  }
  if (mgr->_long_0) {
    _free_heap(ctx, mgr->_long_0);
    mgr->_long_0 = NULL;
  }
  if (mgr->_double_0) {
    _free_heap(ctx, mgr->_double_0);
    mgr->_double_0 = NULL;
  }
  if (mgr->_short_0) {
    _free_heap(ctx, mgr->_short_0);
    mgr->_short_0 = NULL;
  }
  if (mgr->_char_0) {
    _free_heap(ctx, mgr->_char_0);
    mgr->_char_0 = NULL;
  }
  if (mgr->_byte_0) {
    _free_heap(ctx, mgr->_byte_0);
    mgr->_byte_0 = NULL;
  }
  if (mgr->_bool_0) {
    _free_heap(ctx, mgr->_bool_0);
    mgr->_bool_0 = NULL;
  }
}

int _is_null_or_prm_0(nvm_ctx_t* ctx, void* ptr) {
  nvm_mem_mgr_t* mgr = ctx->vm->mem_mgr;
  return ptr == mgr->_bool_0   ||
         ptr == mgr->_byte_0   ||
         ptr == mgr->_char_0   ||
         ptr == mgr->_short_0  ||
         ptr == mgr->_double_0 ||
         ptr == mgr->_float_0  ||
         ptr == mgr->_long_0   ||
         ptr == mgr->_int_0    ||
         ptr == mgr->_null_val;
}
