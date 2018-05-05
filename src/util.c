#include "util.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

int NanoVM_init_lock(lock_t* lock) {
  // TODO
  debug_log0("NanoVM_init_lock");
  return 0;
}

int NanoVM_try_lock(lock_t* lock) {
  // TODO
  debug_log0("NanoVM_try_lock");
  return 0;
}

void NanoVM_lock(lock_t* lock) {
  // TODO
  debug_log0("NanoVM_lock");
}

int NanoVM_unlock(lock_t* lock) {
  // TODO
  debug_log0("NanoVM_unlock");
  return 0;
}

int NanoVM_rand_int(int max) {
  // TODO
  debug_log0("NanoVM_rand_int");
  return 0;
}

void write_log(int e, const char *fmt, ...) {
  char * name = "[nanovm] ";
  char * newfmt;
  va_list l;

  // +1 for the terminating 0
  // +1 for the \n
  // TODO malloc
  newfmt = (char *)malloc(strlen(fmt)+1+strlen(name)+1);
  if (newfmt == NULL) {
    return;
  }

  strcpy(newfmt, name);
  strcat(newfmt, fmt);

  va_start(l, fmt);

  strcat(newfmt, "\n");
  vfprintf(e?stderr:stdout, newfmt, l);

  va_end(l);
  free(newfmt);
}
