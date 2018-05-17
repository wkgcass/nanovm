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

nvm_node_t* NanoVM_init_linkedlist(){
  nvm_node_t* head = (nvm_node_t*) zmalloc(sizeof(nvm_node_t));
  if (!head) {
    return NULL;
  }
  head->next = NULL;
  head->addr = NULL;
  return head;
}

int NanoVM_ins_node(nvm_node_t* head, void* addr) {
  nvm_node_t* new_node = (nvm_node_t*) zmalloc(sizeof(nvm_node_t));
  if (!new_node) {
    return -1;
  }
  new_node->next = NULL;
  new_node->addr = addr;
  nvm_node_t* node = head;
  while (node->next) {
    node = node->next;
  }
  node->next = new_node;
  return 0;
}

void NanoVM_del_all(nvm_node_t* head) {
  if (!head) {
    return;
  }
  nvm_node_t* node = head;
  while (node->next) {
    nvm_node_t* tmp = node->next;
    zfree(node);
    node = tmp;
  }
  zfree(node);
}
