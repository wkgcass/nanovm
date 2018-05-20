#include "class.h"

nvm_node_t* head = NULL;

int setMagic(ClassFile* cf, Bytecode* bytecode) {

  if (jread(bytecode, cf->magic, sizeof(cf->magic), "Unable to read magic number") == -1) {
    return -1;
  }
  if (cf->magic[0] == 0xCA && cf->magic[1] == 0xFE &&
      cf->magic[2] == 0xBA && cf->magic[3] == 0xBE) {
    //   printf("Valid Magic Number\n");
  } else {
    NanoVM_debug_log0("Not a valid Java Class File\n");
    return -1;
  }
  return 0;
}

/* read Java class version */
int setVersion(ClassFile* cf, Bytecode* bytecode) {
  u1 buff[cf->minor_version];

  if (jread(bytecode, buff, sizeof(cf->minor_version), "Unable to read minor version") == -1) {
    return -1;
  }
  cf->minor_version = convertShort(buff);
  if (jread(bytecode, buff, sizeof(cf->major_version), "Unable to read major version") == -1) {
    return -1;
  }
  cf->major_version = convertShort(buff);
  return 0;
}


/* Read access flag of class */
int setAccessFlag(ClassFile* cf, Bytecode* bytecode) {
  u1 buff[cf->access_flag];

  if (jread(bytecode, buff, sizeof(cf->access_flag), "Unable to read access flag") == -1) {
    return -1;
  }
  cf->access_flag = convertShort(buff);
  return 0;
}

/* set this.class of the class file */
int setThisClass(ClassFile* cf, Bytecode* bytecode) {
  u1 buff[cf->this_class];

  if (jread(bytecode, buff, sizeof(cf->this_class), "Unable to read this flag") == -1) {
    return -1;
  }
  cf->this_class = convertShort(buff);
  return 0;
}

/* set super.class of the class file */
int setSuperClass(ClassFile* cf, Bytecode* bytecode) {
  u1 buff[cf->super_class];

  if (jread(bytecode, buff, sizeof(cf->super_class), "Unable to read super flag") == -1) {
    return -1;
  }
  cf->super_class = convertShort(buff);
  return 0;
}

/* set the interface count of the class file, followed by the interfaces that map to the constant pool */
int setInterfaces(ClassFile* cf, Bytecode* bytecode) {
  u1 buff[cf->interface_count];
  int i = 0;

  if (jread(bytecode, buff, sizeof(cf->interface_count), "Unable to read interface count") == -1) {
    return -1;
  }
  cf->interface_count = convertShort(buff);

  if (cf->interface_count > 0) {
    cf->interfaces = (u2*) malloc_ref(head, sizeof(u2) * cf->interface_count);
    if (!cf->interfaces) {
      return -1;
    }
    for (i = 0; i < cf->interface_count; i++) {
      if (jread(bytecode, buff, sizeof(*cf->interfaces), "Unable to read interfaces") == -1) {
        return -1;
      }
      cf->interfaces[i] = convertShort(buff);
    }
  }
  return 0;
}

/* set field detail */
int setFieldDetail(Field_Detail* field, Bytecode* bytecode) {
  u1 buff[field->access_flags];
  int i = 0;
  if (jread(bytecode, buff, sizeof(field->access_flags), "Unable to read field access flag") == -1) {
    return -1;
  }
  field->access_flags = convertShort(buff);
  if (jread(bytecode, buff, sizeof(field->name_index), "Unable to read name index") == -1) {
    return -1;
  }
  field->name_index = convertShort(buff);
  if (jread(bytecode, buff, sizeof(field->d_index), "Unable to read descriptor index") == -1) {
    return -1;
  }
  field->d_index = convertShort(buff);
  if (jread(bytecode, buff, sizeof(field->a_count), "Unable to read attributes count") == -1) {
    return -1;
  }
  field->a_count = convertShort(buff);

  if (field->a_count > 0) {
    field->attributes = (Attribute_Info*) malloc_ref(head, sizeof(Attribute_Info) * field->a_count);
    if (!field->attributes) {
      return -1;
    }

    for (i = 0; i < field->a_count; i++) {
      if (setAttributeDetail(&field->attributes[i], bytecode) == -1) {
        return -1;
      }
    }
  }
  return 0;
}

/* set the fields of the class file */
int setFields(ClassFile* cf, Bytecode* bytecode) {
  u1 buff[cf->field_count];
  int i;

  if (jread(bytecode, buff, sizeof(cf->field_count), "Unable to read fields count") == -1) {
    return -1;
  }
  cf->field_count = convertShort(buff);

  if (cf->field_count > 0) {
    cf->fields = (Field_Detail*) malloc_ref(head, sizeof(Field_Detail) * cf->field_count);
    if (!cf->fields) {
      return -1;
    }

    for (i = 0; i < cf->field_count; i++) {
      if (setFieldDetail(&cf->fields[i], bytecode) == -1) {
        return -1;
      }
    }
  }
  return 0;
}

int setAttributes(ClassFile* cf, Bytecode* bytecode) {
  u1 buff[cf->attribute_count];
  int i = 0;

  if (jread(bytecode, buff, sizeof(cf->attribute_count), "Unable to read attribute count") == -1) {
    return -1;
  }
  cf->attribute_count = convertShort(buff);

  if (cf->attribute_count > 0) {
    cf->attributes = (Attribute_Info*) malloc_ref(head, sizeof(Attribute_Info) * cf->attribute_count);
    if (!cf->attributes) {
      return -1;
    }
    if(setAttributeDetail(&cf->attributes[i], bytecode) == -1) {
      return -1;
    }
  }
  return 0;
}

/* set attribute detail */
int setAttributeDetail(Attribute_Info* a, Bytecode* bytecode) {
  u1 buff[a->length];
  int i;

  if (jread(bytecode, buff, sizeof(a->name_index), "Unable to read attribute index") == -1) {
    return -1;
  }
  a->name_index = convertShort(buff);
  if (jread(bytecode, buff, sizeof(a->length), "Unable to read attribute length") == -1) {
    return -1;
  }
  a->length = convertInt(buff);
  a->info = (u1*) malloc_ref(head, sizeof(u1) * a->length);
  if (!a->info) {
    return -1;
  }
  for (i = 0; i < (int) a->length; i++) {
    if (jread(bytecode, &a->info[i], sizeof(*a->info), "Unable to read attribute info") == -1) {
      return -1;
    }
  }
  return 0;
}

/* set method detail */
int setMethodDetail(Method_Detail* method, Bytecode* bytecode) {
  u1 buff[method->access_flags];
  int i;
  if (jread(bytecode, buff, sizeof(method->access_flags), "Unable to read field access flag") == -1) {
    return -1;
  }
  method->access_flags = convertShort(buff);
  if (jread(bytecode, buff, sizeof(method->name_index), "Unable to read name index") == -1) {
    return -1;
  }
  method->name_index = convertShort(buff);
  if (jread(bytecode, buff, sizeof(method->d_index), "Unable to read descriptor index") == -1) {
    return -1;
  }
  method->d_index = convertShort(buff);
  if (jread(bytecode, buff, sizeof(method->a_count), "Unable to read attributes count") == -1) {
    return -1;
  }
  method->a_count = convertShort(buff);

  if (method->a_count > 0) {
    method->attributes = (Attribute_Info*) malloc_ref(head, sizeof(Attribute_Info) * method->a_count);
    if (!method->attributes) {
      return -1;
    }
    for (i = 0; i < method->a_count; i++) {
      if (setAttributeDetail(&method->attributes[i], bytecode) == -1) {
        return -1;
      }
    }
  }
  return 0;
}

/* set methods of the class file */
int setMethods(ClassFile* cf, Bytecode* bytecode) {
  u1 buff[cf->method_count];
  int i;

  if (jread(bytecode, buff, sizeof(cf->method_count), "Unable to read method count") == -1) {
    return -1;
  }
  cf->method_count = convertShort(buff);

  if (cf->method_count > 0) {
    cf->methods = (Method_Detail*) malloc_ref(head, sizeof(Method_Detail) * cf->method_count);
    if (!cf->methods) {
      return -1;
    }
    for (i = 0; i < cf->method_count; i++) {
      if (setMethodDetail(&cf->methods[i], bytecode) == -1) {
        return -1;
      }
    }
  }
  return 0;
}

/* read a Java Byte Class into ClassFile structure and return */
nvm_node_t* readClass(Bytecode* bytecode, ClassFile* cf) {
  head = NanoVM_init_linkedlist();
  if (!head) {
    return NULL;
  }
  if (setMagic(cf, bytecode) == -1 || setVersion(cf, bytecode) == -1 ||
      setCPool(cf, bytecode) == -1 || setAccessFlag(cf, bytecode) == -1 ||
      setThisClass(cf, bytecode) == -1 || setSuperClass(cf, bytecode) == -1 ||
      setInterfaces(cf, bytecode) == -1 || setFields(cf, bytecode) == -1 ||
      setMethods(cf, bytecode) == -1 || setAttributes(cf, bytecode) == -1) {
    free_ref(head);
    return NULL;
  }

  return head;
}

int setClassInfo(Class_Info* c_info, Bytecode* bytecode) {
  u1 buff[c_info->index];

  if (jread(bytecode, buff, sizeof(c_info->index), "Unable to read Class Info") == -1) {
    return -1;
  }
  c_info->index = convertShort(buff);
  return 0;
}


/* set the method_info type constant pool */
int setMethodInfo(Method_Info* m_info, Bytecode* bytecode) {
  u1 buff[m_info->index];

  if (jread(bytecode, buff, sizeof(m_info->index), "Unable to read Method Info Index") == -1) {
    return -1;
  }
  m_info->index = convertShort(buff);
  if (jread(bytecode, buff, sizeof(m_info->nt_index), "Unable to read Method Info NIndex") == -1) {
    return -1;
  }
  m_info->nt_index = convertShort(buff);
  return 0;
}

/* read UTF8 String */
int setUTF8Info(UTF8_Info* u_info, Bytecode* bytecode) {
  u1 buff[u_info->len];

  if (jread(bytecode, buff, sizeof(u_info->len), "Unable to read UTF8 Length") == -1) {
    return -1;
  }
  u_info->len = convertShort(buff);
  u_info->bytes = (u1*) malloc_ref(head, sizeof(u1) * u_info->len + 1);
  if (!u_info->bytes) {
    return -1;
  }
  u_info->bytes[u_info->len + 1] = '\0';
  if (jread(bytecode, u_info->bytes, u_info->len, "Unable to read UTF8") == -1) {
    return -1;
  }
  return 0;
}


/* set name and type */
int setNameTypeInfo(NameAndType_Info* nt_info, Bytecode* bytecode) {
  u1 buff[nt_info->d_index];

  if (jread(bytecode, buff, sizeof(nt_info->d_index), "Unable to read NameAndType Info") == -1) {
    return -1;
  }
  nt_info->n_index = convertShort(buff);
  if (jread(bytecode, buff, sizeof(nt_info->n_index), "Unable to read NameAndType Info") == -1) {
    return -1;
  }
  nt_info->d_index = convertShort(buff);
  return 0;
}

/* read integer */
int setIntInfo(Integer_Info* i_info, Bytecode* bytecode) {
  u1 buff[i_info->bytes];
  if (jread(bytecode, buff, sizeof(i_info->bytes), "Unable to read Integer") == -1) {
    return -1;
  }
  i_info->bytes = convertInt(buff);
  return 0;
}

/* read float */
int setFloatInfo(Float_Info* f_info, Bytecode* bytecode) {
  u1 buff[f_info->bytes];
  if (jread(bytecode, buff, sizeof(f_info->bytes), "Unable to read float") == -1) {
    return -1;
  }
  f_info->bytes = convertInt(buff);
  return 0;
}


/* set field ref info */
int setFieldRefInfo(Field_Info* fld_info, Bytecode* bytecode) {
  u1 buff[fld_info->index];

  if (jread(bytecode, buff, sizeof(fld_info->index), "Unable to read field info index\n") == -1) {
    return -1;
  }
  fld_info->index = convertShort(buff);
  if (jread(bytecode, buff, sizeof(fld_info->nt_index), "Unable to read field into nt_index\n") == -1) {
    return -1;
  }
  fld_info->nt_index = convertShort(buff);
  return 0;
}

/* set the string information*/
int setStringInfo(String_Info* s_info, Bytecode* bytecode) {
  u1 buff[s_info->s_index];

  if (jread(bytecode, buff, sizeof(s_info->s_index), "Unable to read string index") == -1) {
    return -1;
  }
  s_info->s_index = convertShort(buff);
  return 0;
}

/* read constant pool detail */
int setCPoolDetail(CP_Info* cp, Bytecode* bytecode, int cindex) {

  if (jread(bytecode, &cp->tag, sizeof(cp->tag), "Unable to read CP flag") == -1) {
    return -1;
  }
  switch (cp->tag) {
    case UTF8:
      return setUTF8Info(&cp->c_detail.u8_info, bytecode);;
    case INT:
      return setIntInfo(&cp->c_detail.i_info, bytecode);
    case FLOAT:
      return setFloatInfo(&cp->c_detail.f_info, bytecode);
    case CLASS:
      return setClassInfo(&cp->c_detail.c_info, bytecode);
    case STRING:
      return setStringInfo(&cp->c_detail.s_info, bytecode);
    case FIELDREF:
      return setFieldRefInfo(&cp->c_detail.fld_info, bytecode);
    case METHODREF:
      return setMethodInfo(&cp->c_detail.m_info, bytecode);
    case NAMEANDTYPE:
      return setNameTypeInfo(&cp->c_detail.nt_info, bytecode);
    default:
      NanoVM_debug_log1("Unable to handle tag %d\n", cp->tag);
      return -1;
  }
}

/* read the constant pool information */
int setCPool(ClassFile* cf, Bytecode* bytecode) {
  u1 buff[cf->c_pool_count];
  int i = 0;

  if (jread(bytecode, buff, sizeof(cf->c_pool_count), "Unable to read cpool count") == -1) {
    return -1;
  }
  cf->c_pool_count = convertShort(buff);
  cf->c_pool = (CP_Info*) malloc_ref(head, sizeof(CP_Info) * (1 + cf->c_pool_count));
  if (!cf->c_pool) {
    return -1;
  }

  for (i = 1; i < cf->c_pool_count; i++) {
    if (setCPoolDetail(&cf->c_pool[i], bytecode, i) == -1) {
      return -1;
    }
  }
  return 0;
}

inline u2 convertShort(u1 buff[]) {
  return ((buff[0] << 8) & 0xFF00) | (buff[1] & 0xFF);
}

inline u4 convertInt(u1 buff[]) {
  return ((buff[0] << 24) & 0xFF000000) | ((buff[1] << 16) & 0xFF0000) | ((buff[2] << 8) & 0xFF00) | (buff[3] & 0xFF);
}

int jread(Bytecode* bytecode, void* buff, int s, char* error) {
  if (bytecode->idx + s > bytecode->len) {
    return -1;
  }
  memcpy(buff, bytecode->data + bytecode->idx, s);
  bytecode->idx += s;
  return 0;
}
