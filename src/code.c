#include "code.internal.h"
nvm_opcode_meta_t* opcode_meta;

int NanoVM_GLOBAL_init_code() {
  NanoVM_debug_log0("NanoVM_GLOBAL_init_code");
  opcode_meta = (nvm_opcode_meta_t*)zmalloc(sizeof(nvm_opcode_meta_t) * NVM_OPCODE_SIZE);
  // TODO Fill in the opcode
  return 0;
}

void NanoVM_GLOBAL_free_code() {
  zfree(opcode_meta);
  NanoVM_debug_log0("NanoVM_GLOBAL_free_code");
}

nvm_code_mgr_t* NanoVM_create_code_mgr(nvm_ctx_t* ctx, int type_cap) {
  nvm_code_mgr_t* code_mgr = (nvm_code_mgr_t*)NanoVM_alloc(ctx, sizeof(nvm_code_mgr_t));
  if(!code_mgr){
    return NULL;
  }
  code_mgr->type_cap = type_cap;
  code_mgr->type_len = 0;
  code_mgr->types = (nvm_type_t**)NanoVM_alloc(ctx, sizeof(void*)*type_cap);
  // TODO Fill in the primitives
  NanoVM_debug_log0("NanoVM_create_code_mgr");
  return code_mgr;
}

void NanoVM_release_code_mgr(nvm_ctx_t* ctx, nvm_code_mgr_t* code_mgr) {
  NanoVM_free(ctx,code_mgr);
  NanoVM_debug_log0("NanoVM_release_code_mgr");
}

int NanoVM_parse_code0(nvm_ctx_t* ctx, int bytecode_len, nvm_bytecode_t* bytecodes) {
  Class* nvm_class = (Class*)NanoVM_alloc(ctx,bytecode_len * sizeof(Class));
  if(!nvm_class){
    return 0;
  }
  int ref_type_len = 0;
  for (int i = 0; i < bytecode_len; i++) {
       Bytecode bytecode = {
              .data = bytecodes[i].bytecode,
              .len = bytecodes[i].len,
              .idx = 0
      };
      // parse bytecodes and put in Class array
      nvm_class[i] = *read_class(&bytecode);
       if(&nvm_class[i]){
         ref_type_len++;
       }
  }
  if(ref_type_len == 0){
    return 0;
  }

  nvm_type_t** types = ctx->vm->code_mgr->types;
  int type_cap = ctx->vm->code_mgr->type_cap;
  int type_len = ctx->vm->code_mgr->type_len;
  // If it is larger than the type capacity, it fails
  if(ref_type_len + type_len > type_cap){
    return 0;
  }
  for(int i = 0; i < type_len; i++){
    nvm_ref_type_t* ref_type  = (nvm_ref_type_t*)NanoVM_alloc(ctx, sizeof(nvm_ref_type_t));
    Item* cl_str = get_class_string(&nvm_class[i],nvm_class[i].this_class);
    Item* sup_cl_str = get_class_string(&nvm_class[i],nvm_class[i].super_class);
    ref_type->name = cl_str->value.string.value;
    ref_type->parent_type = (nvm_type_t*)NanoVM_get_ref_type(ctx,sup_cl_str->value.string.value);
    ref_type->super.cat = NVM_TYPE_REF;
    ref_type->field_len = (int)nvm_class[i].fields_count;
    ref_type->meth_len = (int)nvm_class[i].methods_count;
    if((ref_type->field_len > 0)){
      ref_type->fields = (nvm_field_t**)NanoVM_alloc(ctx,sizeof(void*)*ref_type->field_len);
      if(!ref_type){
        return 0;
      }
      Item* name;
      Item* desc;
      nvm_field_t* field;
      for (int j = 0; j < ref_type->field_len; j++) {
          *(ref_type->fields+j) = (nvm_field_t*)NanoVM_alloc(ctx,sizeof(nvm_field_t));
          field = *(ref_type->fields+j);
        if(!field){
          return 0;
        }
        name = get_item(&nvm_class[i],nvm_class[i].fields[j].name_idx);
        desc = get_item(&nvm_class[i],nvm_class[i].fields[j].desc_idx);
        field->name = name->value.string.value;
        field->dec_type->cat = desc->value.string.value[0];
        // TODO put in type;  static_v; acc;

      }
    }
    if(ref_type->meth_len > 0){
      ref_type->meths = (nvm_meth_t**)NanoVM_alloc(ctx,sizeof(void*)*ref_type->meth_len);
      if(!ref_type->meths){
        return 0;
      }
      nvm_meth_t* meth;
      for (int j = 0; j < ref_type->meth_len; j++) {
        *(ref_type->meths+j) = (nvm_meth_t*)NanoVM_alloc(ctx,sizeof(nvm_meth_t));
        meth = *(ref_type->meths+j);
        if(!meth){
          return 0;
        }
          // TODO fill in attribute code

      }

    }
    // TODO fill in size
    *(types + type_len + i) = (nvm_type_t*)ref_type;
    ctx->vm->code_mgr->type_len++;
  }


  NanoVM_debug_log0("NanoVM_parse_code0");
  return 0;
}

nvm_meth_t* NanoVM_get_meth(nvm_ctx_t* ctx, nvm_ref_type_t* ref_type, char* name, nvm_type_t* ret_type, int param_len, nvm_type_t** param_types) {
  nvm_meth_t** meths = ref_type->meths;
  nvm_meth_t* meth;
  int len = ref_type->meth_len;
  for (int i = 0; i < len; i++) {
    meth = *(meths+i);
    if(meth->name == name && meth->param_len == param_len &&
       meth->ret_type == ret_type && param_types == meth->param_types){
      return *(meths+i);
    }

  }
  NanoVM_debug_log0("NanoVM_get_meth");
  return NULL;
}

nvm_field_t* NanoVM_get_field(nvm_ctx_t* ctx, nvm_ref_type_t* ref_type, char* name) {
  int len = ref_type->field_len;
  nvm_field_t** fields = ref_type->fields;
  for(int i = 0; i < len; i++){
    if((*(fields+i))->name == name){
      return *(fields+i);
    }
  }
  NanoVM_debug_log0("NanoVM_get_field");
  return NULL;
}

nvm_prm_type_t* NanoVM_get_prm_type(nvm_ctx_t* ctx, char prm) {
  int len = ctx->vm->code_mgr->type_len;
  nvm_type_t** types = ctx->vm->code_mgr->types;
  nvm_prm_type_t* prm_type;
  for (int i = 0; i < len; i++) {
    if((*(types+i))->cat == NVM_TYPE_PRM){
      prm_type = (nvm_prm_type_t*)*(types+i);
      if(prm_type->prm_type == prm){
        return prm_type;
      }
    }
  }
  NanoVM_debug_log0("NanoVM_get_prm_type");
  return NULL;
}

nvm_ref_type_t* NanoVM_get_ref_type(nvm_ctx_t* ctx, char* name) {
  int len = ctx->vm->code_mgr->type_len;
  nvm_type_t** types = ctx->vm->code_mgr->types;
  nvm_ref_type_t* ref_type;
  for(int i = 0; i < len; i++){
    if((*(types+i))->cat == NVM_TYPE_REF){
      ref_type = (nvm_ref_type_t*)*(types+i);
      return ref_type;
    }
  }
  NanoVM_debug_log0("NanoVM_get_ref_type");
  return NULL;
}

nvm_arr_type_t* NanoVM_get_arr_type(nvm_ctx_t* ctx, nvm_type_t* comp_type) {
  int len = ctx->vm->code_mgr->type_len;
  nvm_type_t** types = ctx->vm->code_mgr->types;
  nvm_arr_type_t* arr_type;
  for (int i = 0; i < len; i++) {
    if((*(types+i))->cat == NVM_TYPE_ARR){
      arr_type = (nvm_arr_type_t*)*(types+i);
      if(arr_type->comp_type == comp_type){
        return arr_type;
      }
    }
  }
  NanoVM_debug_log0("NanoVM_get_arr_type");
  return NULL;
}

void NVM_CODE_release_type(nvm_ctx_t* ctx, nvm_type_t* type) {
  NanoVM_free(ctx,type);
  NanoVM_debug_log0("CODE_release_type");
}

void NVM_CODE_release_field(nvm_ctx_t* ctx, nvm_field_t* field) {
  NanoVM_free(ctx,field);
  NanoVM_debug_log0("CODE_release_field");
}

void NVM_CODE_release_meth(nvm_ctx_t* ctx, nvm_meth_t* meth) {
  NanoVM_free(ctx,meth);
  NanoVM_debug_log0("CODE_release_meth");
}

void NVM_CODE_release_insn(nvm_ctx_t* ctx, nvm_insn_t* insn) {
  NanoVM_free(ctx,insn);
  NanoVM_debug_log0("CODE_release_insn");
}

void NVM_CODE_release_ex(nvm_ctx_t* ctx, nvm_ex_t* ex) {
  NanoVM_free(ctx,ex);
  NanoVM_debug_log0("CODE_release_ex");
}
