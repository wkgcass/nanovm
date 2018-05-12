#include "memory.internal.h"
#include <stdint.h>
#include <string.h>

#include "memory.prm.c"

nvm_mem_mgr_t* NanoVM_create_mem_mgr(nvm_ctx_t* ctx, size_t heap_cap, size_t mem_cap) {
  nvm_mem_mgr_t* mem_mgr = zmalloc(sizeof(nvm_mem_mgr_t));
  if (!mem_mgr) return NULL;

  if (NanoVM_init_lock(&mem_mgr->lock) < 0) {
    zfree(mem_mgr);
    return NULL;
  }
  mem_mgr->heap_cap  = heap_cap;
  mem_mgr->heap_used = 0;
  mem_mgr->mem_cap   = mem_cap;
  mem_mgr->mem_used  = 0;

  mem_mgr->_null_val = NULL;
  mem_mgr->_int_0    = NULL;
  mem_mgr->_long_0   = NULL;
  mem_mgr->_float_0  = NULL;
  mem_mgr->_double_0 = NULL;
  mem_mgr->_short_0  = NULL;
  mem_mgr->_char_0   = NULL;
  mem_mgr->_byte_0   = NULL;
  mem_mgr->_bool_0   = NULL;

  ctx->vm->mem_mgr = mem_mgr;
  return mem_mgr;
}

void NanoVM_release_mem_mgr(nvm_ctx_t* ctx) {
  _release_null(ctx);
  _release_prm_0(ctx);
  nvm_mem_mgr_t* mem_mgr = ctx->vm->mem_mgr;
  if (mem_mgr->heap_used != 0) {
    NanoVM_debug_log1("mem_mgr->heap_used = %zu > 0 when released", mem_mgr->heap_used);
  }
  if (mem_mgr->mem_used != 0) {
    NanoVM_debug_log1("mem_mgr->mem_used = %zu > 0 when released", mem_mgr->mem_used);
  }
  zfree(mem_mgr);
}

// this method return 0 when fail
// only primitive or ref type can be passed into this method
size_t _calc_type_size(nvm_type_t* type) {
  nvm_prm_type_t* prm_type;
  nvm_ref_type_t* ref_type;

  switch (type->cat) {
    case NVM_TYPE_PRM:
      prm_type = (nvm_prm_type_t*)type;
      return sizeof(nvm_prm_hdr_t) + prm_type->size;
    case NVM_TYPE_REF:
      ref_type = (nvm_ref_type_t*)type;
      return sizeof(nvm_ref_hdr_t) + ref_type->size;
    case NVM_TYPE_ARR:
      NanoVM_error_log0("_calc_type_size cannot accept array type");
      break;
    default:
      NanoVM_error_log1("BUG! unknown nvm_type_t#cat value %d", type->cat);
  }
  return 0;
}

// init object lock, and set initial value
// only primitive and ref types can be passed into this method
int _init_object(nvm_ctx_t* ctx, nvm_type_t* type, nvm_object_t* obj) {
  if (NanoVM_init_lock(&obj->lock) < 0) {
    return -1;
  }
  obj->type = type;
  obj->cnt = 0;
  if (type->cat == NVM_TYPE_REF) {
    size_t base_off = sizeof(nvm_ref_hdr_t);
    nvm_ref_type_t* ref_type = (nvm_ref_type_t*)type;

    while (ref_type) {
      for (int i = 0; i < ref_type->field_len; ++i) {
        nvm_field_t* field = ref_type->fields[i];
        size_t off = base_off + field->off;
        nvm_object_t** o = (nvm_object_t**) (((char*) obj) + off);
        nvm_object_t* val_to_set;
        if (field->type->cat == NVM_TYPE_PRM) {
          val_to_set = (nvm_object_t*)_get_prm_0(ctx, (nvm_prm_type_t*)field->type);
        } else {
          // for ref and array types
          // only set null value
          val_to_set = _get_null(ctx);
        }
        if (!val_to_set) {
          return -1;
        }
        *o = val_to_set;
      }
      ref_type = ref_type->parent_type;
    }
  } else if (type->cat == NVM_TYPE_PRM) {
    // simply set the mem to 0
    nvm_prm_type_t* prm_type = (nvm_prm_type_t*)type;
    size_t size = prm_type->size;
    size_t off = sizeof(nvm_prm_hdr_t);
    memset(((char*)obj) + off, 0, size);
  } else {
    NanoVM_error_log1("_init_object can only accept primitive or reference types, but got %d", type->cat);
    return -1;
  }
  return 0;
}

// alloc some memory and increase heap usage
void* _alloc_heap(nvm_ctx_t* ctx, size_t len) {
  nvm_mem_mgr_t* mgr = ctx->vm->mem_mgr;
  NanoVM_lock(&mgr->lock, ctx->tid);

  if (mgr->heap_used + len > mgr->heap_cap) {
    NanoVM_unlock(&mgr->lock, ctx->tid);
    NanoVM_debug_log0("no heap space left");
    return NULL;
  }
  nvm_object_t* ret = zmalloc(len);
  if (!ret) {
    NanoVM_unlock(&mgr->lock, ctx->tid);
    NanoVM_debug_log1("zmalloc %zu failed", len);
    return NULL;
  }

  mgr->heap_used += zmalloc_size(ret);

  NanoVM_unlock(&mgr->lock, ctx->tid);
  return (void*)ret;
}

// release the memory and decrease heap usage
void _free_heap(nvm_ctx_t* ctx, void* ptr) {
  nvm_object_t* obj_p = ptr;
  if (obj_p->cnt > 0) {
    NanoVM_debug_log0("object freed while ptr->cnt > 0");
  }
  nvm_mem_mgr_t* mgr = ctx->vm->mem_mgr;
  NanoVM_lock(&mgr->lock, ctx->tid);
  mgr->heap_used -= zmalloc_size(ptr);
  NanoVM_unlock(&mgr->lock, ctx->tid);
  zfree(ptr);
}

nvm_object_t* NanoVM_alloc_heap(nvm_ctx_t* ctx, nvm_type_t* type) {
  size_t len = _calc_type_size(type);
  if (!len) {
    return NULL;
  }
  nvm_object_t* ret = _alloc_heap(ctx, len);
  if (!ret) {
    return NULL;
  }
  if (_init_object(ctx, type, ret) < 0) {
    _free_heap(ctx, ret);
    return NULL;
  }
  return ret;
}

nvm_arr_hdr_t* NanoVM_alloc_heap_arr(nvm_ctx_t* ctx, nvm_arr_type_t* type, int len) {
  size_t m_len = sizeof(nvm_arr_hdr_t) + sizeof(void*) * len;
  nvm_arr_hdr_t* ret = _alloc_heap(ctx, m_len);
  ret->len = len;

  void* content_p;
  if (type->comp_type->cat == NVM_TYPE_PRM) {
    content_p = _get_prm_0(ctx, (nvm_prm_type_t*) type->comp_type);
  } else {
    content_p = _get_null(ctx);
  }
  if (!content_p) {
    _free_heap(ctx, ret);
    return NULL;
  }
  void** p0 = (void**)(((char*) ret) + sizeof(nvm_arr_hdr_t));
  for (int i = 0; i < len; ++i) {
    void** p = (p0 + i);
    *p = content_p;
  }

  ret->super.super.type = (nvm_type_t*)type;
  ret->super.super.cnt = 0;

  // init lock
  if (NanoVM_init_lock(&ret->super.super.lock) < 0) {
    _free_heap(ctx, ret);
    return NULL;
  }
  return ret;
}

void* NanoVM_alloc(nvm_ctx_t* ctx, size_t size) {
  nvm_mem_mgr_t* mgr = ctx->vm->mem_mgr;
  NanoVM_lock(&mgr->lock, ctx->tid);
  if (mgr->mem_used + size > mgr->mem_cap) {
    NanoVM_unlock(&mgr->lock, ctx->tid);
    NanoVM_debug_log0("no mem left");
    return NULL;
  }
  void* ret = zmalloc(size);
  if (!ret) {
    NanoVM_unlock(&mgr->lock, ctx->tid);
    NanoVM_debug_log1("zmalloc %zu failed", size);
    return NULL;
  }
  mgr->mem_used += zmalloc_size(ret);
  NanoVM_unlock(&mgr->lock, ctx->tid);
  return ret;
}

void NanoVM_free(nvm_ctx_t* ctx, void* ptr) {
  nvm_mem_mgr_t* mgr = ctx->vm->mem_mgr;
  NanoVM_lock(&mgr->lock, ctx->tid);
  mgr->mem_used -= zmalloc_size(ptr);
  NanoVM_unlock(&mgr->lock, ctx->tid);
  zfree(ptr);
}

void NanoVM_ref(nvm_ctx_t* ctx, nvm_object_t* ref) {
  if (_is_null_or_prm_0(ctx, ref)) {
    return;
  }
  NanoVM_lock(&ref->lock, ctx->tid);
  ref->cnt += 1;
  NanoVM_unlock(&ref->lock, ctx->tid);
}

void _unref_dep(nvm_ctx_t* ctx, nvm_ref_hdr_t* ref) {
  nvm_ref_type_t* type = (nvm_ref_type_t*)ref->super.type;

  size_t base_off = sizeof(nvm_ref_hdr_t);
  while (type) {
    for (int i = 0; i < type->field_len; ++i) {
      nvm_field_t* field = type->fields[i];
      size_t off = base_off + field->off;
      nvm_object_t* o = *((nvm_object_t**) (((char*)ref) + off));
      NanoVM_unref(ctx, o);
    }
    type = type->parent_type;
  }
}

void _free_obj(nvm_ctx_t* ctx, nvm_object_t* ref) {
  nvm_arr_hdr_t* arr;
  int len;
  void** p0;
  nvm_object_t* o;

  switch (ref->type->cat) {
    case NVM_TYPE_PRM:
      // directly free for primitives
      _free_heap(ctx, ref);
      return;
    case NVM_TYPE_REF:
      _unref_dep(ctx, (nvm_ref_hdr_t*) ref);
      _free_heap(ctx, ref);
      return;
    case NVM_TYPE_ARR:
      arr = (nvm_arr_hdr_t*)ref;
      len = arr->len;
      p0 = (void**)(((char*)arr) + sizeof(nvm_arr_hdr_t));
      for (int i = 0; i < len; ++i) {
        o = *(p0 + i);
        NanoVM_unref(ctx, o);
      }
      _free_heap(ctx, ref);
      return;
    default:
      NanoVM_error_log1("BUG! unknown nvm_type_t#cat value %d", ref->type->cat);
  }
}

void NanoVM_unref(nvm_ctx_t* ctx, nvm_object_t* ref) {
  if (_is_null_or_prm_0(ctx, ref)) {
    return;
  }
  if (ref->cnt == 0) {
    NanoVM_error_log0("reference count should not be 0 when unreferenced");
  }
  // if ref->cnt is 1, there will be no concurrency on the ref
  // and we should release the ref
  // and 0 is invalid state
  if (ref->cnt == 1 || ref->cnt == 0) {
    ref->cnt = 0;
    _free_obj(ctx, ref);
    return;
  }
  NanoVM_lock(&ref->lock, ctx->tid);
  ref->cnt -= 1;
  NanoVM_unlock(&ref->lock, ctx->tid);
}

nvm_object_t* NanoVM_get_null(nvm_ctx_t* ctx) {
  return _get_null(ctx);
}

size_t NanoVM_get_prm_type_size(char prm_type) {
  switch (prm_type) {
    case NVM_PRM_TYPE_BOOLEAN:
      return sizeof(char);
    case NVM_PRM_TYPE_BYTE:
      return sizeof(char);
    case NVM_PRM_TYPE_CHAR:
      return sizeof(uint16_t);
    case NVM_PRM_TYPE_DOUBLE:
      return sizeof(double);
    case NVM_PRM_TYPE_FLOAT:
      return sizeof(float);
    case NVM_PRM_TYPE_INT:
      return sizeof(int);
    case NVM_PRM_TYPE_LONG:
      return sizeof(long);
    case NVM_PRM_TYPE_SHORT:
      return sizeof(short);
    case NVM_PRM_TYPE_VOID:
      return 0;
    default:
      NanoVM_error_log1("trying to get unknown primitive type %d", prm_type);
      return 0;
  }
}

void NanoVM_mem_set_prm_i(nvm_prm_hdr_t* prm, int val) {
  uint16_t c; short s; char b;
  nvm_prm_type_t* type = (nvm_prm_type_t*)prm->super.type;
  size_t size = type->size;
  void* p;
  switch (type->prm_type) {
    case NVM_PRM_TYPE_INT:
      p = &val;
      break;
    case NVM_PRM_TYPE_CHAR:
      c = val;
      p = &c;
      break;
    case NVM_PRM_TYPE_SHORT:
      s = val;
      p = &s;
      break;
    case NVM_PRM_TYPE_BYTE:
    case NVM_PRM_TYPE_BOOLEAN:
      b = val;
      p = &b;
      break;
    default:
      NanoVM_error_log1("set integer type primitive unsupported type %c", type->prm_type);
      // do nothing
      return;
  }
  void* dest = ((char*)prm) + sizeof(nvm_prm_hdr_t);
  memcpy(dest, p, size);
}

void NanoVM_mem_set_prm_j(nvm_prm_hdr_t* prm, long val) {
  long* dest = (long*) (((char*)prm) + sizeof(nvm_prm_hdr_t));
  *dest = val;
}

void NanoVM_mem_set_prm_f(nvm_prm_hdr_t* prm, float val) {
  float* dest = (float*) (((char*)prm) + sizeof(nvm_prm_hdr_t));
  *dest = val;
}

void NanoVM_mem_set_prm_d(nvm_prm_hdr_t* prm, double val) {
  double* dest = (double*) (((char*)prm) + sizeof(nvm_prm_hdr_t));
  *dest = val;
}

void NanoVM_mem_put_field(nvm_ctx_t* ctx, nvm_ref_hdr_t* obj, nvm_field_t* field, nvm_object_t* val) {
  size_t off = field->off;
  nvm_object_t** p = (nvm_object_t**)((char*)obj + sizeof(nvm_ref_hdr_t) + off);
  nvm_object_t* old = *p;
  *p = val;
  NanoVM_ref(ctx, val);
  NanoVM_unref(ctx, old);
}

void NanoVM_mem_set_array(nvm_ctx_t* ctx, nvm_arr_hdr_t* arr, int index, nvm_object_t* val) {
  nvm_object_t** p = ((nvm_object_t**) (((char*) arr) + sizeof(nvm_arr_hdr_t) + index * sizeof(void*)));
  nvm_object_t* old = *p;
  *p = val;
  NanoVM_ref(ctx, val);
  NanoVM_unref(ctx, old);
}

void NanoVM_mem_get_prm(nvm_prm_hdr_t* prm, void* result) {
  nvm_prm_type_t* type = (nvm_prm_type_t*)prm->super.type;
  size_t size = type->size;
  memcpy(result, (((char*) prm) + sizeof(nvm_prm_hdr_t)), size);
}

void NanoVM_mem_get_field(nvm_ref_hdr_t* obj, nvm_field_t* field, nvm_object_t** result) {
  nvm_object_t* field_ref = * ((nvm_object_t**) (((char*) obj) + sizeof(nvm_ref_hdr_t) + field->off));
  *result = field_ref;
}

void NanoVM_mem_get_array(nvm_arr_hdr_t* arr, int index, nvm_object_t** result) {
  nvm_object_t* element = * ((nvm_object_t**) (((char*) arr) + sizeof(nvm_arr_hdr_t) + index * sizeof(void*)));
  *result = element;
}

size_t NanoVM_get_mem_usage(nanovm_t* vm) {
  return vm->mem_mgr->mem_used;
}

size_t NanoVM_get_heap_usage(nanovm_t* vm) {
  return vm->mem_mgr->heap_used;
}

size_t NanoVM_get_mem_cap(nanovm_t* vm) {
  return vm->mem_mgr->mem_cap;
}

size_t NanoVM_get_heap_cap(nanovm_t* vm) {
  return vm->mem_mgr->heap_cap;
}
