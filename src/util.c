#include "util.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int NanoVM_init_lock(nvm_lock_t* lock) {
  lock->tid = 0;
  lock->cnt = 0;
  return 0;
}

int NanoVM_try_lock(nvm_lock_t* lock, int tid) {
  if (lock->tid == tid) {
    ++lock->cnt;
    return 0;
  }
  if (!lock->tid) {
    lock->tid = tid;
    if (lock->tid == tid) {
      ++lock->cnt;
      return 0;
    }
  }
  // lock->tid != tid
  return -1;
}

void NanoVM_lock(nvm_lock_t* lock, int tid) {
  while (NanoVM_try_lock(lock, tid));
}

int NanoVM_unlock(nvm_lock_t* lock, int tid) {
  if (lock->tid != tid) {
    return -1;
  }
  if (!(--lock->cnt)) {
    lock->tid = 0;
  }
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

void NanoVM_write_log(int e, const char *fmt, ...) {
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

char* NanoVM_strerror(int err) {
  if (err > 0xEE00) {
    switch (err) {
      case NVM_ERR_OOM_MEM:
        return "Out of memory";
      case NVM_ERR_OOM_HEP:
        return "Out of heap memory";
      case NVM_ERR_STK_OVR:
        return "Stack over flow";
      case NVM_ERR_MET_DEF:
        return "No such method";
      case NVM_ERR_FLD_DEF:
        return "No such field";
      case NVM_ERR_CLS_DEF:
        return "No class def found";
      default:
        return NULL;
    }
  } else {
    return NULL;
  }
}
