#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include "util.h"
#include <stdlib.h>
#ifndef CLASS_H
#define CLASS_H
#define UTF8        1
#define INT         3
#define FLOAT       4
#define LONG        5
#define DOUBLE      6
#define CLASS       7
#define STRING      8
#define FIELDREF    9
#define METHODREF   10
#define IMETHODREF  11
#define NAMEANDTYPE 12
#define MHANDLE     15
#define MTYPE       16
#define IDYNAMIC    18

#define ACC_PUBLIC       0x0001
#define ACC_PRIVATE      0x0002
#define ACC_PROTECTED    0x0004
#define ACC_STATIC       0x0008
#define ACC_FINAL        0x0010
#define ACC_SUPER        0x0020
#define ACC_VOLATILE     0x0040
#define ACC_TRANSIENT    0x0080
#define ACC_SYNTHETIC    0x1000
#define ACC_INTERFACE    0x0200
#define ACC_ABSTRACT     0x0400
#define ACC_ANNOTATION   0x2000
#define ACC_ENUM         0x4000
#define ACC_SYNCHRONIZED 0x0020
#define ACC_BRIDGE       0x0040
#define ACC_VARARGS      0x0080
#define ACC_NATIVE       0x0100
#define ACC_ABSTRACT     0x0400
#define ACC_STRICT       0x0800
#define ACC_SYNTHETIC    0x1000

typedef unsigned char u1;
typedef unsigned short u2;
typedef unsigned int u4;
typedef struct {
    u1* data;
    int idx;
    int len;
} Bytecode;

typedef struct {
    u2 index;
} Class_Info;

typedef struct {
    u2 index;
    u2 nt_index;
} Field_Info;

typedef struct {
    u2 index;
    u2 nt_index;
} Method_Info;

typedef struct {
    u2 index;
    u2 nt_index;
} InterfaceMethod_Info;

typedef struct {
    u2 s_index;
} String_Info;

typedef struct {
    u4 bytes;
} Integer_Info;

typedef struct {
    u4 bytes;
} Float_Info;

typedef struct {
    u4 high_bytes;
    u4 low_bytes;
} Long_Info;

typedef struct {
    u4 high_bytes;
    u4 low_bytes;
} Double_Info;

typedef struct {
    u2 n_index;
    u2 d_index;
} NameAndType_Info;

typedef struct {
    u2 len;
    u1* bytes;
} UTF8_Info;

typedef struct {
    u2 ref_kind;
    u2 ref_index;
} MethodHandle_Info;

typedef struct {
    u2 d_index;
} MethodType_Info;

typedef struct {
    u2 b_m_index;
    u2 nt_index;
} InvokeDynamic_Info;

typedef struct CP_Info_ {
    u1 tag;
    char* info;
    union {
        Class_Info c_info;
        Field_Info fld_info;
        Method_Info m_info;
        InterfaceMethod_Info IM_info;
        String_Info s_info;
        Integer_Info i_info;
        Float_Info f_info;
        Long_Info l_info;
        Double_Info d_info;
        NameAndType_Info nt_info;
        UTF8_Info u8_info;
        MethodHandle_Info mh_info;
        MethodType_Info mt_info;
        InvokeDynamic_Info id_info;
    } c_detail;
} CP_Info;

typedef struct Attribute_Info_ {
    u2 name_index;
    u4 length;
    u1* info;
} Attribute_Info;

typedef struct Field_Detail_ {
    u2 access_flags;
    u2 name_index;
    u2 d_index;
    u2 a_count;
    Attribute_Info* attributes;
} Field_Detail;

typedef struct Method_Detail_ {
    u2 access_flags;
    u2 name_index;
    u2 d_index;
    u2 a_count;
    Attribute_Info* attributes;
} Method_Detail;

typedef struct ClassFile_ {
    u1 magic[4];
    u2 minor_version;
    u2 major_version;
    u2 c_pool_count;
    CP_Info* c_pool;
    u2 access_flag;
    u2 this_class;
    u2 super_class;
    u2 interface_count;
    u2* interfaces;
    u2 field_count;
    Field_Detail* fields;
    u2 method_count;
    Method_Detail* methods;
    u2 attribute_count;
    Attribute_Info* attributes;
} ClassFile;


int setCPool(ClassFile* cf, Bytecode* bytecode);

int setCPoolDetail(CP_Info* cp, Bytecode* bytecode, int cindex);

int setNameTypeInfo(NameAndType_Info* nt_info, Bytecode* bytecode);

int setUTF8Info(UTF8_Info* u_info, Bytecode* bytecode);

int setMethodInfo(Method_Info* m_info, Bytecode* bytecode);

int setClassInfo(Class_Info* c_info, Bytecode* bytecode);

int setStringInfo(String_Info* s_info, Bytecode* bytecode);

int setIntInfo(Integer_Info* i_info, Bytecode* bytecode);

int setFloatInfo(Float_Info* f_info, Bytecode* bytecode);

int setFieldRefInfo(Field_Info* fld_info, Bytecode* bytecode);

int setMagic(ClassFile* cf, Bytecode* bytecode);

int setVersion(ClassFile* cf, Bytecode* bytecode);

int setAccessFlag(ClassFile* cf, Bytecode* bytecode);

int setThisClass(ClassFile* cf, Bytecode* bytecode);

int setSuperClass(ClassFile* cf, Bytecode* bytecode);

int setInterfaces(ClassFile* cf, Bytecode* bytecode);

int setFields(ClassFile* cf, Bytecode* bytecode);

int setFieldDetail(Field_Detail* field, Bytecode* bytecode);

int setMethods(ClassFile* cf, Bytecode* bytecode);

int setMethodDetail(Method_Detail* method, Bytecode* bytecode);

int setAttributeDetail(Attribute_Info* a, Bytecode* bytecode);

int setAttributes(ClassFile* cf, Bytecode* bytecode);

nvm_node_t* readClass(Bytecode* bytecode, ClassFile* cf);

u2 convertShort(u1 buff[]);

u4 convertInt(u1 buff[]);

int jread(Bytecode* bytecode, void* buff, int s, char* error);

#endif //CLASS_H__
