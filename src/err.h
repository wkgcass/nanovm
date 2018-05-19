#ifndef NANOVM_ERR_H
#define NANOVM_ERR_H 1

#include <errno.h>

// Out of memory
#define NVM_ERR_OOM_MEM 0xEE01
// Out of heap memory
#define NVM_ERR_OOM_HEP 0xEE02
// Stack over flow
#define NVM_ERR_STK_OVR 0xEE03
// No such method
#define NVM_ERR_MET_DEF 0xEE05
// No such field
#define NVM_ERR_FLD_DEF 0xEE06
// No class def found
#define NVM_ERR_CLS_DEF 0xEE07

#endif
