#ifndef NANOVM_H
#define NANOVM_H 1

#include <stddef.h>

typedef struct _nanovm_ nanovm_t;

typedef struct {
  long heap_cap;   // heap memory for objects
  long mem_cap;    // other memory
   int frame_cap;  // max frame size (stack over flow limit)
   int type_cap;   // max types
   int thread_cap; // max thread size
} nanovm_config_t;

typedef struct {
  size_t len;
   char* bytecode;
} nanovm_bytecode_t;

 int NanoVM_GLOBAL_init();
void NanoVM_GLOBAL_free();

nanovm_t* NanoVM_create(nanovm_config_t* conf);

int NanoVM_parse_code(nanovm_t* vm, int bytecode_len, nanovm_bytecode_t* bytecodes);

 int NanoVM_start  (nanovm_t* nanovm, char* main_class);
 int NanoVM_stop   (nanovm_t* nanovm);
void NanoVM_release(nanovm_t* nanovm);

/*
 * NanoVM_stop will set `stop` flag on all running threads
 * return 0 when no threads are running, or -1 means still got threads running,
 * maybe blocked by io.
 */

#endif
