#include "code.internal.h"
#include "nanovm.internal.h"

nvm_opcode_meta_t* opcode_meta;

int NanoVM_GLOBAL_init_code() {
    opcode_meta = (nvm_opcode_meta_t*) zmalloc(sizeof(nvm_opcode_meta_t) * NVM_OPCODE_SIZE);
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
    code_mgr->type_cap = type_cap;
    code_mgr->type_len = 0;
    code_mgr->types = (nvm_type_t**) NanoVM_alloc(ctx, sizeof(void*) * type_cap);
    if (!code_mgr->types) {
        NanoVM_free(ctx, code_mgr);
        NanoVM_debug_log0("types failed to apply for memory");
        return NULL;
    }
    if (_build_prm_types(ctx) == -1) {
        NanoVM_free(ctx, *(code_mgr->types));
        NanoVM_free(ctx, code_mgr->types);
        NanoVM_free(ctx, code_mgr);
        return NULL;
    }
    code_mgr->type_len = code_mgr->type_len + NVM_PREDEF_TYPE_CNT;

}

void NanoVM_release_code_mgr(nvm_ctx_t* ctx) {
    NanoVM_free(ctx, ctx->vm->code_mgr);
}

int NanoVM_parse_code0(nvm_ctx_t* ctx, int bytecode_len, nvm_bytecode_t* bytecodes) {
    if (bytecode_len == 0) {
        return 0;
    }
    nvm_type_t** types = ctx->vm->code_mgr->types;
    int type_cap = ctx->vm->code_mgr->type_cap;
    int type_len = ctx->vm->code_mgr->type_len;
    // If it is larger than the type capacity, it fails
    if (bytecode_len + NVM_PREDEF_TYPE_CNT > type_cap) {
        NanoVM_error_log0("the parsed bytecode exceeds the maximum size of the type");
        return -1;
    }
    Class* nvm_class = (Class*) NanoVM_alloc(ctx, bytecode_len * sizeof(Class));
    if (!nvm_class) {
        NanoVM_debug_log0("class failed to apply for memory");
        return -1;
    }
    // parse bytecodes and put in Class array
    for (int i = 0; i < bytecode_len; i++) {
        Bytecode bytecode = {
                .data = bytecodes[i].bytecode,
                .len = bytecodes[i].len,
                .idx = 0
        };
        nvm_class[i] = *read_class(&bytecode);
        if (!&nvm_class[i]) {
            NanoVM_free(ctx, nvm_class);
            NanoVM_error_log0("failed to parse bytecode");
            return -1;
        }
    }
    // Generate a type_t object and nitialize basic information
    for (int i = 0; i < bytecode_len; i++) {
        nvm_ref_type_t* ref_type = (nvm_ref_type_t*) NanoVM_alloc(ctx, sizeof(nvm_ref_type_t));
        if (!ref_type) {
            NanoVM_free(ctx, nvm_class);
            NanoVM_debug_log0("ref_type failed to apply for memory");
            return -1;
        }
        Item* cl_str = get_class_string(&nvm_class[i], nvm_class[i].this_class);
        ref_type->name = cl_str->value.string.value;
        ref_type->super.cat = NVM_TYPE_REF;
        types[NVM_PREDEF_TYPE_CNT + i] = (nvm_type_t*) ref_type;
    }

    // fill in fields and methods
    for (int i = 0; i < bytecode_len; i++) {
        nvm_ref_type_t* ref_type = (nvm_ref_type_t*) types[NVM_PREDEF_TYPE_CNT + i];
        Item* sup_cl_str = get_class_string(&nvm_class[i], nvm_class[i].super_class);
        ref_type->parent_type = (nvm_ref_type_t*) NanoVM_get_ref_type(ctx, sup_cl_str->value.string.value);
        ref_type->field_len = (int) nvm_class[i].fields_count;
        ref_type->meth_len = (int) nvm_class[i].methods_count;
        // begin fill in fields
        if (ref_type->field_len != 0) {
            if (_build_fields(ctx, &nvm_class[i], ref_type) == -1) {
                NanoVM_free(ctx, *(types + NVM_PREDEF_TYPE_CNT));
                NanoVM_free(ctx, nvm_class);
                return -1;
            }
        }
        // begin fill in methods
        if (ref_type->meth_len != 0) {
            if (_build_meths(ctx, &nvm_class[i], ref_type) == -1) {
                NanoVM_free(ctx, *(types + NVM_PREDEF_TYPE_CNT));
                NanoVM_free(ctx, nvm_class);
                return -1;
            }
        }
        type_len++;

    }
    // finally free memory and type_len increase
    ctx->vm->code_mgr->type_len = type_len;
    NanoVM_free(ctx, nvm_class);
    return 0;
}

nvm_meth_t* NanoVM_get_meth(nvm_ctx_t* ctx, nvm_ref_type_t* ref_type, char* name, nvm_type_t* ret_type, int param_len,
                            nvm_type_t** param_types) {
    nvm_meth_t** meths = ref_type->meths;
    nvm_meth_t* meth;
    int len = ref_type->meth_len;
    for (int i = 0; i < len; i++) {
        meth = *(meths + i);
        if (meth->name == name && meth->param_len == param_len &&
            meth->ret_type == ret_type && param_types == meth->param_types) {
            return *(meths + i);
        }

    }
    return NULL;
}

nvm_field_t* NanoVM_get_field(nvm_ctx_t* ctx, nvm_ref_type_t* ref_type, char* name) {
    int len = ref_type->field_len;
    nvm_field_t** fields = ref_type->fields;
    for (int i = 0; i < len; i++) {
        if ((*(fields + i))->name == name) {
            return *(fields + i);
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
            prm_type = (nvm_prm_type_t*) *(types + i);
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
    nvm_ref_type_t* ref_type;
    for (int i = 0; i < len; i++) {
        if (types[i]->cat == NVM_TYPE_REF) {
            ref_type = (nvm_ref_type_t*) *(types + i);
            return ref_type;
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
            arr_type = (nvm_arr_type_t*) *(types + i);
            if (arr_type->comp_type == comp_type) {
                return arr_type;
            }
        }
    }
    return NULL;
}

int _build_prm_types(nvm_ctx_t* ctx) {
    nvm_prm_type_t* prm_type;
    nvm_type_t** types = ctx->vm->code_mgr->types;
    // int
    prm_type = _create_prm_type(ctx, NVM_PRM_TYPE_INT);
    if (!prm_type) {
        return -1;
    }
    *(types) = (nvm_type_t*) prm_type;
    // float
    prm_type = _create_prm_type(ctx, NVM_PRM_TYPE_FLOAT);
    if (!prm_type) {
        return -1;
    }
    *(types + 1) = (nvm_type_t*) prm_type;
    // long
    prm_type = _create_prm_type(ctx, NVM_PRM_TYPE_LONG);
    if (!prm_type) {
        return -1;
    }
    *(types + 2) = (nvm_type_t*) prm_type;
    // double
    prm_type = _create_prm_type(ctx, NVM_PRM_TYPE_DOUBLE);
    if (!prm_type) {
        return -1;
    }
    *(types + 3) = (nvm_type_t*) prm_type;
    // short
    prm_type = _create_prm_type(ctx, NVM_PRM_TYPE_SHORT);
    if (!prm_type) {
        return -1;
    }
    *(types + 4) = (nvm_type_t*) prm_type;
    // byte
    prm_type = _create_prm_type(ctx, NVM_PRM_TYPE_BYTE);
    if (!prm_type) {
        return -1;
    }
    *(types + 5) = (nvm_type_t*) prm_type;
    // char
    prm_type = _create_prm_type(ctx, NVM_PRM_TYPE_CHAR);
    if (!prm_type) {
        return -1;
    }
    *(types + 6) = (nvm_type_t*) prm_type;
    // bool
    prm_type = _create_prm_type(ctx, NVM_PRM_TYPE_BOOLEAN);
    if (!prm_type) {
        return -1;
    }
    *(types + 7) = (nvm_type_t*) prm_type;
    // void
    prm_type = _create_prm_type(ctx, NVM_PRM_TYPE_VOID);
    if (!prm_type) {
        return -1;
    }
    *(types + 8) = (nvm_type_t*) prm_type;
    return 0;

}

nvm_prm_type_t* _create_prm_type(nvm_ctx_t* ctx, char prm_type) {
    nvm_prm_type_t* nvm_prm_type = (nvm_prm_type_t*) NanoVM_alloc(ctx, sizeof(nvm_prm_type));
    if (!nvm_prm_type) {
        NanoVM_debug_log0("failed to apply for memory when creating a primitive type");
        return NULL;
    }
    nvm_prm_type->prm_type = prm_type;
    nvm_prm_type->super.cat = NVM_TYPE_PRM;
    nvm_prm_type->size = NanoVM_get_prm_type_size(prm_type);
}

int _build_fields(nvm_ctx_t* ctx, Class* class, nvm_ref_type_t* ref_type) {
    ref_type->fields = (nvm_field_t**) NanoVM_alloc(ctx, sizeof(nvm_field_t*) * ref_type->field_len);
    if (!ref_type->fields) {
        NanoVM_debug_log0("fields* failed to apply for memory");
        return -1;
    }
    for (int i = 0; i < ref_type->field_len; i++) {
        ref_type->fields[i] = (nvm_field_t*) NanoVM_alloc(ctx, sizeof(nvm_field_t));
        if (!ref_type->fields[i]) {
            NanoVM_free(ctx, *(ref_type->fields));
            NanoVM_free(ctx, ref_type->fields);
            NanoVM_debug_log0("field failed to apply for memory");
            return -1;
        }
        Item * field_name = get_item(class, class->fields[i].name_idx);
        Item * field_desc = get_item(class, class->fields[i].desc_idx);
        ref_type->fields[i]->acc = (int) class->flags;
        ref_type->fields[i]->dec_type = (nvm_type_t*) ref_type;
        ref_type->fields[i]->name = field_name->value.string.value;
        ref_type->fields[i]->off =
                ref_type->parent_type->size + (i == 0 ? 0 : ref_type->fields[i - 1]->off + sizeof(void*));
        ref_type->fields[i]->type = _get_type(ctx, field_desc->value.string.value);
        // if is a unknown type
        if (!ref_type->fields[i]->type){
            NanoVM_free(ctx, *(ref_type->fields));
            NanoVM_free(ctx, ref_type->fields);
            return -1;
        }
        // TODO  parse and fill in static_v
        ref_type->fields[i]->static_v = NULL;
    }
}

int _build_meths(nvm_ctx_t* ctx, Class* class, nvm_ref_type_t* ref_type) {
    ref_type->meths = (nvm_meth_t**) NanoVM_alloc(ctx, sizeof(nvm_meth_t*) * ref_type->meth_len);
    if (!ref_type->meths) {
        NanoVM_free(ctx, *(ref_type->fields));
        NanoVM_free(ctx, ref_type->fields);
        NanoVM_debug_log0("methods* failed to apply for memory");
        return -1;
    }
    for (int i = 0; i < ref_type->meth_len; i++) {
        ref_type->meths[i] = (nvm_meth_t*) NanoVM_alloc(ctx, sizeof(nvm_meth_t));
        if (!ref_type->meths[i]) {
            NanoVM_free(ctx, *(ref_type->meths));
            NanoVM_free(ctx, ref_type->meths);
            NanoVM_free(ctx, *(ref_type->fields));
            NanoVM_free(ctx, ref_type->fields);
            NanoVM_debug_log0("method failed to apply for memory");
            return -1;
        }
        Item * meth_name = get_item(class, class->methods[i].name_idx);
        // Item* meth_desc = get_item(&nvm_class[i], nvm_class[i].methods[j].desc_idx);
        ref_type->meths[i]->name = meth_name->value.string.value;
        ref_type->meths[i]->acc = (int) class->methods[i].flags;
        ref_type->meths[i]->dec_type = (nvm_type_t*) ref_type;
        // TODO
    }
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
            return (nvm_type_t*) NanoVM_get_ref_type(ctx, &fld_type[1]);
        case '[':
            return (nvm_type_t*) NanoVM_get_arr_type(ctx, _get_type(ctx, &fld_type[1]));
        default:
            NanoVM_error_log0("trying to get unknown type %s\", fld_type[0]");
            return NULL;
    }
}

void NVM_CODE_release_type(nvm_ctx_t* ctx, nvm_type_t* type) {
    NanoVM_free(ctx, type);
}

void NVM_CODE_release_field(nvm_ctx_t* ctx, nvm_field_t* field) {
    NanoVM_free(ctx, field);
}

void NVM_CODE_release_meth(nvm_ctx_t* ctx, nvm_meth_t* meth) {
    NanoVM_free(ctx, meth);
}

void NVM_CODE_release_insn(nvm_ctx_t* ctx, nvm_insn_t* insn) {
    NanoVM_free(ctx, insn);
}

void NVM_CODE_release_ex(nvm_ctx_t* ctx, nvm_ex_t* ex) {
    NanoVM_free(ctx, ex);
}
