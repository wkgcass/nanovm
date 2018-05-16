#include "code.internal.h"
#include "string.h"
#include "nanovm.internal.h"
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

}

void NanoVM_release_code_mgr(nvm_ctx_t* ctx) {
    for (int i = 0; i < ctx->vm->code_mgr->type_len; i++) {
        NVM_CODE_release_type(ctx, ctx->vm->code_mgr->types[i]);
    }
    NanoVM_free(ctx, ctx->vm->code_mgr->types);
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
            for (int j = 0; j < i; j++) {
                NanoVM_free(ctx, &nvm_class[i]);
            }
            NanoVM_error_log0("failed to parse bytecode");
            return -1;
        }
    }
    // Generate a type_t object and nitialize basic information
    for (int i = 0; i < bytecode_len; i++) {
        nvm_ref_type_t* ref_type = (nvm_ref_type_t*) NanoVM_alloc(ctx, sizeof(nvm_ref_type_t));
        if (!ref_type) {
            for (int k = 0; k < i; k++) {
                NVM_CODE_release_type(ctx, ref_type[NVM_PREDEF_TYPE_CNT + i]);
            }
            for (int j = 0; j < bytecode_len; j++) {
                NanoVM_free(ctx, &nvm_class[j]);
            }
            NanoVM_debug_log0("ref_type failed to apply for memory");
            return -1;
        }
        types[NVM_PREDEF_TYPE_CNT + i] = (nvm_type_t*) ref_type;
        ctx->vm->code_mgr->type_len++;
        // fill in field length，name and method length as early as possible
        ref_type->name = NULL;
        ref_type->field_len = 0;
        ref_type->meth_len = 0;
        Item* cl_str = get_class_string(&nvm_class[i], nvm_class[i].this_class);
        ref_type->name = (char*) NanoVM_alloc(ctx, sizeof(char) * (cl_str->value.string.length + 1));
        if (!ref_type->name) {
            for (int k = 0; k < i + 1; k++) {
                NVM_CODE_release_type(ctx, ref_type[NVM_PREDEF_TYPE_CNT + i]);
            }
            for (int j = 0; j < bytecode_len; j++) {
                NanoVM_free(ctx, &nvm_class[j]);
            }
            return -1;
        }
        strcpy(ref_type->name, cl_str->value.string.value);
        ref_type->name[cl_str->value.string.length] = '\0';
        ref_type->super.cat = NVM_TYPE_REF;
    }

    // fill in fields and methods
    for (int i = 0; i < bytecode_len; i++) {
        nvm_ref_type_t* ref_type = (nvm_ref_type_t*) types[NVM_PREDEF_TYPE_CNT + i];
        Item* sup_cl_str = get_class_string(&nvm_class[i], nvm_class[i].super_class);
        ref_type->parent_type = (nvm_ref_type_t*) NanoVM_get_ref_type(ctx, sup_cl_str->value.string.value);
        if (!ref_type->parent_type) {
            for (int j = 0; j < bytecode_len; j++) {
                NVM_CODE_release_type(ctx, ref_type);
                NanoVM_free(ctx, &nvm_class[j]);
            }
            NanoVM_debug_log0("can't find parent_type");
            return -1;
        }
        // begin fill in fields
        int field_cnt = (int) nvm_class[i].fields_count;
        if (field_cnt != 0) {
            if (_build_fields(ctx, &nvm_class[i], ref_type) == -1) {
                for (int j = 0; j < bytecode_len; j++) {
                    NVM_CODE_release_type(ctx, ref_type);
                    NanoVM_free(ctx, &nvm_class[j]);
                }
                return -1;
            }
        }
        // begin fill in methods
        int meth_cnt = (int) nvm_class[i].methods_count;
        if (meth_cnt != 0) {
            if (_build_meths(ctx, &nvm_class[i], ref_type) == -1) {
                for (int j = 0; j < bytecode_len; j++) {
                    NVM_CODE_release_type(ctx, ref_type);
                    NanoVM_free(ctx, nvm_class[j]);
                }
                return -1;
            }
        }

    }
    // finally free memory and type_len increase
    for (int l = 0; l < bytecode_len; l++) {
        NanoVM_free(ctx, nvm_class[l]);
    }
    return 0;
}

nvm_meth_t* NanoVM_get_meth(nvm_ctx_t* ctx, nvm_ref_type_t* ref_type, char* name, nvm_type_t* ret_type, int param_len,
                            nvm_type_t** param_types) {
    nvm_meth_t** meths = ref_type->meths;
    nvm_meth_t* meth;
    int len = ref_type->meth_len;
    for (int i = 0; i < len; i++) {
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
        if (fields[i]->name == name) {
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
    nvm_ref_type_t* ref_type;
    for (int i = 0; i < len; i++) {
        if (types[i]->cat == NVM_TYPE_REF) {
            ref_type = (nvm_ref_type_t*) types[i];
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
            arr_type = (nvm_arr_type_t*) types[i];
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
    types[0] = (nvm_type_t*) prm_type;
    // float
    prm_type = _create_prm_type(ctx, NVM_PRM_TYPE_FLOAT);
    if (!prm_type) {
        return -1;
    }
    types[1] = (nvm_type_t*) prm_type;
    // long
    prm_type = _create_prm_type(ctx, NVM_PRM_TYPE_LONG);
    if (!prm_type) {
        return -1;
    }
    types[2] = (nvm_type_t*) prm_type;
    // double
    prm_type = _create_prm_type(ctx, NVM_PRM_TYPE_DOUBLE);
    if (!prm_type) {
        return -1;
    }
    types[3] = (nvm_type_t*) prm_type;
    // short
    prm_type = _create_prm_type(ctx, NVM_PRM_TYPE_SHORT);
    if (!prm_type) {
        return -1;
    }
    types[4] = (nvm_type_t*) prm_type;
    // byte
    prm_type = _create_prm_type(ctx, NVM_PRM_TYPE_BYTE);
    if (!prm_type) {
        return -1;
    }
    types[5] = (nvm_type_t*) prm_type;
    // char
    prm_type = _create_prm_type(ctx, NVM_PRM_TYPE_CHAR);
    if (!prm_type) {
        return -1;
    }
    types[6] = (nvm_type_t*) prm_type;
    // bool
    prm_type = _create_prm_type(ctx, NVM_PRM_TYPE_BOOLEAN);
    if (!prm_type) {
        return -1;
    }
    types[7] = (nvm_type_t*) prm_type;
    // void
    prm_type = _create_prm_type(ctx, NVM_PRM_TYPE_VOID);
    if (!prm_type) {
        return -1;
    }
    types[8] = (nvm_type_t*) prm_type;
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
    ctx->vm->code_mgr->type_len++;
}

int _build_fields(nvm_ctx_t* ctx, Class* class, nvm_ref_type_t* ref_type) {
    int field_cnt = (int) class->fields_count;
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
        ref_type->field_len++;
        ref_type->fields[i]->name = NULL;
        ref_type->fields[i]->static_v = NULL;
        Item* field_name = get_item(class, class->fields[i].name_idx);
        Item* field_desc = get_item(class, class->fields[i].desc_idx);
        ref_type->fields[i]->acc = (int) class->flags;
        ref_type->fields[i]->dec_type = (nvm_type_t*) ref_type;
        ref_type->fields[i]->name = (char*) NanoVM_alloc(ctx, sizeof(char) * (field_name->value.string.length + 1));
        if (!ref_type->fields[i]->name) {
            return -1;
        }
        strcpy(ref_type->fields[i]->name, field_name->value.string.value);
        ref_type->fields[i]->name[field_name->value.string.length] = '\0';
        //field_name->value.string.value;
        ref_type->fields[i]->off =
                ref_type->parent_type->size + (i == 0 ? 0 : ref_type->fields[i - 1]->off + sizeof(void*));
        ref_type->fields[i]->type = _get_type(ctx, field_desc->value.string.value);

        // if is a unknown type
        if (!ref_type->fields[i]->type) {
            return -1;
        }
        // TODO  parse and fill in static_v
    }
}

int _build_meths(nvm_ctx_t* ctx, Class* class, nvm_ref_type_t* ref_type) {
    int meth_cnt = (int) class->methods_count;
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
        ref_type->meth_len++;
        ref_type->meths[i]->name = NULL;
        Item* meth_name = get_item(class, class->methods[i].name_idx);
        // Item* meth_desc = get_item(&nvm_class[i], nvm_class[i].methods[j].desc_idx);
        ref_type->meths[i]->name = (char*) NanoVM_alloc(ctx, sizeof(char) * (meth_name->value.string.length + 1));
        if (!ref_type->meths[i]->name) {
            return -1;
        }
        strcpy(ref_type->meths[i]->name, meth_name->value.string.value);
        ref_type->meths[i]->name[meth_name->value.string.length] = '\0';
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
    nvm_ref_type_t* ref_type;
    nvm_arr_type_t* arr_type;
    if (type->cat == NVM_TYPE_REF) {
        ref_type = (nvm_ref_type_t*) type;
        // free meths
        for (int j = 0; j < ref_type->meth_len; j++) {
            NVM_CODE_release_meth(ctx, ref_type->meths[j]);
        }
        NanoVM_free(ctx, ref_type->meths);
        // free fields
        for (int i = 0; i < ref_type->field_len; i++) {
            NVM_CODE_release_field(ctx, ref_type->fields[i]);
        }
        NanoVM_free(ctx, ref_type->fields);
        if (ref_type->name) {
            NanoVM_free(ctx, ref_type->name);
        }
        NanoVM_free(ctx, ref_type);
    } else {
        NanoVM_free(ctx, type);
    }
    ctx->vm->code_mgr->type_len--;

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
        NVM_CODE_release_ex(ctx, meth->exs[nvm_ex_t]);
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
    NVM_CODE_release_insn(ctx, ex->start);
    NVM_CODE_release_insn(ctx, ex->end);
    NVM_CODE_release_insn(ctx, ex->handler);
    NanoVM_free(ctx, ex);
}
