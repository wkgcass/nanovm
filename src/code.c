#include "code.internal.h"
#include "string.h"
#include "nanovm.internal.h"
#include "memory.h"

//TODO 1.Fill in opcode  2. fill in field and meth attr
nvm_opcode_meta_t* opcode_meta = NULL;

int NanoVM_GLOBAL_init_code() {
  opcode_meta = (nvm_opcode_meta_t*) zmalloc(sizeof(nvm_opcode_meta_t) * NVM_OPCODE_SIZE);
  if (!opcode_meta) {
    NanoVM_debug_log0("init_code failed to apply memory ");
    return -1;
  }
  // TODO Fill in the opcode

  return 0;
}

void NanoVM_GLOBAL_free_code() {
  zfree(opcode_meta);
}

nvm_code_mgr_t* NanoVM_create_code_mgr(nvm_ctx_t* ctx, int type_cap) {
  nvm_code_mgr_t* code_mgr = (nvm_code_mgr_t*) NanoVM_alloc(ctx, sizeof(nvm_code_mgr_t));
  if (!code_mgr) {
    NanoVM_debug_log0("code_manager failed to apply for memory");
    return NULL;
  }
  ctx->vm->code_mgr = code_mgr;
  code_mgr->type_cap = type_cap;
  code_mgr->type_len = 0;
  code_mgr->types = (nvm_type_t**) NanoVM_alloc(ctx, sizeof(void*) * type_cap);
  if (!code_mgr->types) {
    NanoVM_release_code_mgr(ctx);
    NanoVM_debug_log0("types failed to apply for memory");
    return NULL;
  }
  if (_build_prm_types(ctx) == -1) {
    NanoVM_release_code_mgr(ctx);
    return NULL;
  }
  if (_build_ref_types(ctx) == -1) {
    NanoVM_release_code_mgr(ctx);
  }
  return code_mgr;
}

void NanoVM_release_code_mgr(nvm_ctx_t* ctx) {
  for (int i = 0; i < ctx->vm->code_mgr->type_len; i++) {
    NVM_CODE_release_type(ctx, ctx->vm->code_mgr->types[i]);
  }
  NanoVM_free(ctx, ctx->vm->code_mgr->types);
  NanoVM_free(ctx, ctx->vm->code_mgr);
}

void _release_class(nvm_node_t* head) {
  nvm_node_t* node = head;
  while (node->next) {
    if (!node->addr) {
      zfree(node->addr);
    }
    node = node->next;
  }
  if (!node->addr) {
    zfree(node->addr);
  }
  NanoVM_del_all(head);
}

int NanoVM_parse_code0(nvm_ctx_t* ctx, int bytecode_len, nvm_bytecode_t* bytecodes) {
  if (bytecode_len == 0) {
    return 0;
  }
  nvm_type_t** types = ctx->vm->code_mgr->types;
  int type_cap = ctx->vm->code_mgr->type_cap;
  int type_len = ctx->vm->code_mgr->type_len;
  // If it is larger than the type capacity, it fails
  if (bytecode_len + type_len > type_cap) {
    NanoVM_error_log0("the parsed bytecode exceeds the maximum size of the type");
    return -1;
  }
  ClassFile* cf = (ClassFile*) NanoVM_alloc(ctx, bytecode_len * sizeof(ClassFile));
  if (!cf) {
    NanoVM_debug_log0("class failed to apply for memory");
    return -1;
  }
  nvm_node_t** class_list = (nvm_node_t**) NanoVM_alloc(ctx, sizeof(void*) * bytecode_len);
  if (!class_list) {
    return -1;
  }
  // parse bytecodes and put in Class array
  for (int i = 0; i < bytecode_len; i++) {
    Bytecode bytecode = {
            .data = bytecodes[i].bytecode,
            .len = bytecodes[i].len,
            .idx = 0
    };
    class_list[i] = readClass(&bytecode, &cf[i]);
    if (!class_list[i]) {
      for (int j = 0; j < i; j++) {
        free_ref(class_list[j]);
      }
      NanoVM_free(ctx, class_list);
      NanoVM_free(ctx, cf);
      NanoVM_error_log0("failed to parse bytecode");
      return -1;
    }
  }
  // Generate a type_t object and nitialize basic information
  for (int i = 0; i < bytecode_len; i++) {
    nvm_ref_type_t* ref_type = (nvm_ref_type_t*) NanoVM_alloc(ctx, sizeof(nvm_ref_type_t));
    if (!ref_type) {
      for (int k = 0; k < i; k++) {
        NVM_CODE_release_type(ctx, types[type_len + i]);
      }
      for (int j = 0; j < bytecode_len; j++) {
        free_ref(class_list[j]);
      }
      NanoVM_free(ctx, class_list);
      NanoVM_free(ctx, cf);
      NanoVM_debug_log0("ref_type failed to apply for memory");
      return -1;
    }
    types[type_len + i] = (nvm_type_t*) ref_type;
    ctx->vm->code_mgr->type_len++;
    // fill in field length，name and method length as early as possible
    ref_type->name = NULL;
    ref_type->field_len = 0;
    ref_type->meth_len = 0;
    ref_type->super.cat = NVM_TYPE_REF;
    CP_Info* class_index = &cf[i].c_pool[cf[i].this_class];
    CP_Info* str = &cf[i].c_pool[class_index->c_detail.c_info.index];
    ref_type->name = (char*) NanoVM_alloc(ctx, sizeof(char) * (str->c_detail.u8_info.len + 1));
    if (!ref_type->name) {
      for (int k = 0; k < i + 1; k++) {
        NVM_CODE_release_type(ctx, types[type_len + i]);
      }
      for (int j = 0; j < bytecode_len; j++) {
        free_ref(class_list[j]);
      }
      NanoVM_free(ctx, class_list);
      NanoVM_free(ctx, cf);
      return -1;
    }
    strcpy(ref_type->name, (char*) str->c_detail.u8_info.bytes);
    ref_type->name[str->c_detail.u8_info.len] = '\0';
  }
  // fill in fields and methods
  for (int i = 0; i < bytecode_len; i++) {
    nvm_ref_type_t* ref_type = (nvm_ref_type_t*) types[type_len + i];
    CP_Info* superc_index = &cf->c_pool[cf->super_class];
    CP_Info* str = &cf->c_pool[superc_index->c_detail.c_info.index];
    char* name = NanoVM_alloc(ctx, sizeof(char) * (str->c_detail.u8_info.len + 1));
    if (!name) {
      for (int j = 0; j < bytecode_len; j++) {
        NVM_CODE_release_type(ctx, types[type_len + j]);
        free_ref(class_list[j]);
      }
      NanoVM_free(ctx, class_list);
      NanoVM_free(ctx, cf);
      return -1;
    }
    strcpy(name, (char*) str->c_detail.u8_info.bytes);
    name[str->c_detail.u8_info.len] = '\0';
    ref_type->parent_type = (nvm_ref_type_t*) NanoVM_get_ref_type(ctx, name);
    NanoVM_free(ctx, name);
    if (!ref_type->parent_type) {
      for (int j = 0; j < bytecode_len; j++) {
        NVM_CODE_release_type(ctx, types[type_len + j]);
        free_ref(class_list[j]);
      }
      NanoVM_free(ctx, class_list);
      NanoVM_free(ctx, cf);
      NanoVM_debug_log0("can't find parent_type");
      return -1;
    }
    // begin fill in fields
    int field_cnt = (int) cf[i].field_count;
    if (field_cnt != 0) {
      if (_build_fields(ctx, &cf[i], ref_type) == -1) {
        for (int j = 0; j < bytecode_len; j++) {
          NVM_CODE_release_type(ctx, types[type_len + j]);
          free_ref(class_list[j]);
        }
        NanoVM_free(ctx, class_list);
        NanoVM_free(ctx, cf);
        return -1;
      }
    }
    // begin fill in methods
    int meth_cnt = cf->method_count;
    if (meth_cnt != 0) {
      if (_build_meths(ctx, &cf[i], ref_type) == -1) {
        for (int j = 0; j < bytecode_len; j++) {
          NVM_CODE_release_type(ctx, types[type_len + j]);
          free_ref(class_list[j]);
        }
        NanoVM_free(ctx, class_list);
        NanoVM_free(ctx, cf);
        return -1;
      }
    }

  }
  // finally free memory
  for (int l = 0; l < bytecode_len; l++) {
    free_ref(class_list[l]);
  }
  NanoVM_free(ctx, class_list);
  NanoVM_free(ctx, cf);
  return 0;
}

nvm_meth_t* NanoVM_get_meth(nvm_ctx_t* ctx, nvm_ref_type_t* ref_type, char* name, nvm_type_t* ret_type, int param_len,
                            nvm_type_t** param_types) {
  nvm_meth_t** meths = ref_type->meths;
  nvm_meth_t* meth;
  int len = ref_type->meth_len;
  for (int i = 0; i < len; i++) {
    meth = meths[i];
    meth = meths[i];
    if (meth->name == name && meth->param_len == param_len &&
        meth->ret_type == ret_type && param_types == meth->param_types) {
      return meths[i];
    }

  }
  return NULL;
}

nvm_field_t* NanoVM_get_field(nvm_ctx_t* ctx, nvm_ref_type_t* ref_type, char* name) {
  int len = ref_type->field_len;
  nvm_field_t** fields = ref_type->fields;
  for (int i = 0; i < len; i++) {
    if (strcmp(fields[i]->name, name) == 0) {
      return fields[i];
    }
  }
  return NULL;
}

nvm_prm_type_t* NanoVM_get_prm_type(nvm_ctx_t* ctx, char prm) {
  int len = ctx->vm->code_mgr->type_len;
  nvm_type_t** types = ctx->vm->code_mgr->types;
  nvm_prm_type_t* prm_type;
  for (int i = 0; i < len; i++) {
    if (types[i]->cat == NVM_TYPE_PRM) {
      prm_type = (nvm_prm_type_t*) types[i];
      if (prm_type->prm_type == prm) {
        return prm_type;
      }
    }
  }
  return NULL;
}

nvm_ref_type_t* NanoVM_get_ref_type(nvm_ctx_t* ctx, char* name) {
  int len = ctx->vm->code_mgr->type_len;
  nvm_type_t** types = ctx->vm->code_mgr->types;
  for (int i = 0; i < len; i++) {
    if (types[i]->cat == NVM_TYPE_REF && strcmp(((nvm_ref_type_t*) types[i])->name, name) == 0) {
      return (nvm_ref_type_t*) types[i];
    }
  }
  return NULL;
}

nvm_arr_type_t* NanoVM_get_arr_type(nvm_ctx_t* ctx, nvm_type_t* comp_type) {
  int len = ctx->vm->code_mgr->type_len;
  nvm_type_t** types = ctx->vm->code_mgr->types;
  nvm_arr_type_t* arr_type;
  for (int i = 0; i < len; i++) {
    if (types[i]->cat == NVM_TYPE_ARR) {
      arr_type = (nvm_arr_type_t*) types[i];
      if (arr_type->comp_type == comp_type) {
        return arr_type;
      }
    }
  }
  //if not found arr type. should generate a arr type
  nvm_arr_type_t* new_arr_type = NanoVM_alloc(ctx, sizeof(nvm_arr_type_t));
  new_arr_type->super.cat = NVM_TYPE_ARR;
  new_arr_type->comp_type = comp_type;
  types[len] = (nvm_type_t*) new_arr_type;
  ctx->vm->code_mgr->type_len++;
  return new_arr_type;
}

// build_prm_type usually does not fail, so check at the end
int _build_prm_types(nvm_ctx_t* ctx) {
  return _create_prm_type(ctx, NVM_PRM_TYPE_INT) | _create_prm_type(ctx, NVM_PRM_TYPE_FLOAT) |
         _create_prm_type(ctx, NVM_PRM_TYPE_LONG) | _create_prm_type(ctx, NVM_PRM_TYPE_DOUBLE) |
         _create_prm_type(ctx, NVM_PRM_TYPE_SHORT) | _create_prm_type(ctx, NVM_PRM_TYPE_BYTE) |
         _create_prm_type(ctx, NVM_PRM_TYPE_CHAR) | _create_prm_type(ctx, NVM_PRM_TYPE_BOOLEAN) |
         _create_prm_type(ctx, NVM_PRM_TYPE_VOID);
}

int _build_ref_types(nvm_ctx_t* ctx) {
  nvm_type_t** types = ctx->vm->code_mgr->types;
  nvm_ref_type_t* java_lang_Object = (nvm_ref_type_t*) NanoVM_alloc(ctx, sizeof(nvm_ref_type_t));
  if (!java_lang_Object) {
    NanoVM_debug_log0("build_ref_types apply memory failed");
    return -1;
  }
  types[ctx->vm->code_mgr->type_len] = (nvm_type_t*) java_lang_Object;
  java_lang_Object->super.cat = NVM_TYPE_REF;
  java_lang_Object->field_len = 0;
  java_lang_Object->meth_len = 0;
  java_lang_Object->name = NanoVM_alloc(ctx, sizeof("java/lang/Object"));
  memcpy(java_lang_Object->name, "java/lang/Object", sizeof("java/lang/Object"));
  // TODO fill in method info
  java_lang_Object->parent_type = NULL;
  java_lang_Object->size = 0;
  ctx->vm->code_mgr->type_len++;
  return 0;
}

int _create_prm_type(nvm_ctx_t* ctx, char prm_type) {
  nvm_prm_type_t* _prm_type = (nvm_prm_type_t*) NanoVM_alloc(ctx, sizeof(_prm_type));
  ctx->vm->code_mgr->types[ctx->vm->code_mgr->type_len] = (nvm_type_t*) _prm_type;
  ctx->vm->code_mgr->type_len++;
  if (!_prm_type) {
    NanoVM_debug_log0("failed to apply for memory when creating a primitive type");
    return -1;
  }
  _prm_type->prm_type = prm_type;
  _prm_type->super.cat = NVM_TYPE_PRM;
  _prm_type->size = NanoVM_get_prm_type_size(prm_type);
  return 0;
}

int _build_fields(nvm_ctx_t* ctx, ClassFile* cf, nvm_ref_type_t* ref_type) {
  int field_cnt = cf->field_count;
  nvm_field_t* field;
  ref_type->fields = (nvm_field_t**) NanoVM_alloc(ctx, sizeof(nvm_field_t*) * field_cnt);
  if (!ref_type->fields) {
    NanoVM_debug_log0("fields* failed to apply for memory");
    return -1;
  }
  for (int i = 0; i < field_cnt; i++) {
    ref_type->fields[i] = (nvm_field_t*) NanoVM_alloc(ctx, sizeof(nvm_field_t));
    if (!ref_type->fields[i]) {
      NanoVM_debug_log0("field failed to apply for memory");
      return -1;
    }
    field = ref_type->fields[i];
    ref_type->field_len++;
    field->name = NULL;
    field->static_v = NULL;
    int nindex = cf->fields[i].name_index;
    int dindex = cf->fields[i].d_index;
    field->acc = cf->access_flag;
    field->dec_type = (nvm_type_t*) ref_type;
    field->name = (char*) NanoVM_alloc(ctx, sizeof(char) * (cf->c_pool[nindex].c_detail.u8_info.len + 1));
    if (!field->name) {
      return -1;
    }
    strcpy(field->name, (char*) cf->c_pool[nindex].c_detail.u8_info.bytes);
    field->name[cf->c_pool[nindex].c_detail.u8_info.len] = '\0';
    field->off =
            ref_type->parent_type->size + (i == 0 ? 0 : ref_type->fields[i - 1]->off + sizeof(void*));
    char* desc = NanoVM_alloc(ctx, sizeof(char) * (cf->c_pool[dindex].c_detail.u8_info.len + 1));
    strcpy(desc, (char*) cf->c_pool[dindex].c_detail.u8_info.bytes);
    desc[cf->c_pool[dindex].c_detail.u8_info.len] = '\0';
    field->type = _get_type(ctx, desc);;
    // if is a unknown type
    if (!field->type) {
      NanoVM_debug_log0("it is a unknow type for field");
      return -1;
    }
    field->static_v = NULL;
  }
  return 0;
}

int _build_meths(nvm_ctx_t* ctx, ClassFile* cf, nvm_ref_type_t* ref_type) {
  int meth_cnt = cf->method_count;
  nvm_meth_t* meth;
  Method_Detail* m_detail;
  Attribute_Info* attrib;
  UTF8_Info* utf8_info;
  ref_type->meths = (nvm_meth_t**) NanoVM_alloc(ctx, sizeof(nvm_meth_t*) * meth_cnt);
  if (!ref_type->meths) {
    NanoVM_debug_log0("methods* failed to apply for memory");
    return -1;
  }
  for (int i = 0; i < meth_cnt; i++) {
    ref_type->meths[i] = (nvm_meth_t*) NanoVM_alloc(ctx, sizeof(nvm_meth_t));
    if (!ref_type->meths[i]) {
      NanoVM_debug_log0("method failed to apply for memory");
      return -1;
    }
    meth = ref_type->meths[i];
    m_detail = &cf->methods[i];
    ref_type->meth_len++;
    meth->name = NULL;
    int nindex = m_detail->name_index;
    meth->name = (char*) NanoVM_alloc(ctx, sizeof(char) * (cf->c_pool[nindex].c_detail.u8_info.len + 1));
    if (!meth->name) {
      return -1;
    }
    strcpy(meth->name, (char*) cf->c_pool[nindex].c_detail.u8_info.bytes);
    meth->name[cf->c_pool[nindex].c_detail.u8_info.len] = '\0';
    meth->acc = m_detail->access_flags;
    meth->dec_type = (nvm_type_t*) ref_type;
    for (int j = 0; j < m_detail->a_count; j++) {
      attrib = &m_detail->attributes[j];
      utf8_info = &cf->c_pool[attrib->name_index].c_detail.u8_info;
      int idx = 0;
      if (strcmp("Code", (char*) utf8_info->bytes) == 0) {
        meth->max_stack = convertShort(attrib->info);
        idx += sizeof(u2);
        meth->max_locals = convertShort(attrib->info + idx);
        idx += sizeof(u2);
        meth->insn_len = 0;//convertInt(attrib->info + idx);
        idx += sizeof(u4);
        //TODO opcode
        idx += sizeof(u1) * meth->insn_len;
        meth->ex_len = 0;//convertShort(attrib->info + idx);
        //TODO exception
      }

    }

  }
  return 0;
}

nvm_type_t* _get_type(nvm_ctx_t* ctx, char* fld_type) {
  if (!fld_type) {
    return NULL;
  }
  switch (fld_type[0]) {
    case 'B':
      return (nvm_type_t*) NanoVM_get_prm_type(ctx, NVM_PRM_TYPE_BYTE);
    case 'C':
      return (nvm_type_t*) NanoVM_get_prm_type(ctx, NVM_PRM_TYPE_CHAR);
    case 'D':
      return (nvm_type_t*) NanoVM_get_prm_type(ctx, NVM_PRM_TYPE_DOUBLE);
    case 'F':
      return (nvm_type_t*) NanoVM_get_prm_type(ctx, NVM_PRM_TYPE_FLOAT);
    case 'I':
      return (nvm_type_t*) NanoVM_get_prm_type(ctx, NVM_PRM_TYPE_INT);
    case 'J':
      return (nvm_type_t*) NanoVM_get_prm_type(ctx, NVM_PRM_TYPE_LONG);
    case 'S':
      return (nvm_type_t*) NanoVM_get_prm_type(ctx, NVM_PRM_TYPE_SHORT);
    case 'V':
      return (nvm_type_t*) NanoVM_get_prm_type(ctx, NVM_PRM_TYPE_VOID);
    case 'Z':
      return (nvm_type_t*) NanoVM_get_prm_type(ctx, NVM_PRM_TYPE_BOOLEAN);
    case 'L':
      for (int i = 0;; i++) {
        if (fld_type[i] == ';') {
          fld_type[i] = '\0';
          break;
        }
      }
      return (nvm_type_t*) NanoVM_get_ref_type(ctx, &fld_type[1]);
    case '[':
      return (nvm_type_t*) NanoVM_get_arr_type(ctx, _get_type(ctx, &fld_type[1]));
    default:
      NanoVM_error_log0("trying to get unknown type %s\", fld_type[0]");
      return NULL;
  }
}

void NVM_CODE_release_type(nvm_ctx_t* ctx, nvm_type_t* type) {
  if (!type) {
    return;
  }
  nvm_ref_type_t* ref_type;
  if (type->cat == NVM_TYPE_REF) {
    ref_type = (nvm_ref_type_t*) type;
    NanoVM_free(ctx, ref_type->name);
    // free meths
    for (int i = 0; i < ref_type->meth_len; i++) {
      NVM_CODE_release_meth(ctx, ref_type->meths[i]);
    }
    // free fields
    for (int j = 0; j < ref_type->field_len; j++) {
      NVM_CODE_release_field(ctx, ref_type->fields[j]);
    }

  } else {
    //if is arr type or primit tpye
    NanoVM_free(ctx, type);
  }

}

void NVM_CODE_release_field(nvm_ctx_t* ctx, nvm_field_t* field) {
  if (field->name) {
    NanoVM_free(ctx, field->name);
  }
  if (field->static_v) {
    NanoVM_free(ctx, field->static_v);
  }
  NanoVM_free(ctx, field);
}

void NVM_CODE_release_meth(nvm_ctx_t* ctx, nvm_meth_t* meth) {
  if (meth->name) {
    NanoVM_free(ctx, meth->name);
  }
  // free exs
  for (int i = 0; i < meth->ex_len; i++) {
    NVM_CODE_release_ex(ctx, meth->exs[i]);
  }
  NanoVM_free(ctx, meth->exs);
  // free insns
  for (int j = 0; j < meth->insn_len; j++) {
    NVM_CODE_release_insn(ctx, meth->insns[j]);
  }
  NanoVM_free(ctx, meth->insns);
  NanoVM_free(ctx, meth);
}

void NVM_CODE_release_insn(nvm_ctx_t* ctx, nvm_insn_t* insn) {
  NanoVM_free(ctx, insn);
}

void NVM_CODE_release_ex(nvm_ctx_t* ctx, nvm_ex_t* ex) {
  NanoVM_free(ctx, ex);
}
