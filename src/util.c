#include "util.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int NanoVM_init_lock(lock_t* lock) {
  lock->tid = 0;
  return 0;
}

int NanoVM_try_lock(lock_t* lock, int tid) {
  if (lock->tid == tid) {
    return 0;
  }
  if (!lock->tid) {
    lock->tid = tid;
    return lock->tid == tid;
  }
  // lock->tid != tid
  return -1;
}

void NanoVM_lock(lock_t* lock, int tid) {
  while (NanoVM_try_lock(lock, tid));
}

int NanoVM_unlock(lock_t* lock, int tid) {
  if (lock->tid != tid) {
    return -1;
  }
  lock->tid = 0;
  return 0;
}

int rand_init_flag = 0;
int NanoVM_rand_int(int max) {
  if (!rand_init_flag) {
    srand((unsigned)time(NULL));
    rand_init_flag = 1;
  }
  return (rand() % (max - 2)) + 1;
}

void write_log(int e, const char *fmt, ...) {
  char * name = "[nanovm] ";
  char * newfmt;
  va_list l;

  // +1 for the terminating 0
  // +1 for the \n
  newfmt = (char *)zmalloc(strlen(fmt)+1+strlen(name)+1);
  // it's an util method, and the mem won't go out into other methods
  // so we simply use zmalloc directly
  if (newfmt == NULL) {
    return;
  }

  strcpy(newfmt, name);
  strcat(newfmt, fmt);

  va_start(l, fmt);

  strcat(newfmt, "\n");
  vfprintf(e?stderr:stdout, newfmt, l);

  va_end(l);
  zfree(newfmt);
}
