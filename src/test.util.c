#include "util.h"
#include <pthread.h>

int TEST_NanoVM_init_lock() {
  nvm_lock_t lock;
  lock.tid = 1;
  NanoVM_init_lock(&lock);
  TEST(lock.tid == 0, "NanoVM_init_lock should set tid to 0");
  return 0;
}

int TEST_NanoVM_try_lock() {
  nvm_lock_t lock;
  NanoVM_init_lock(&lock);
  lock.tid = 1;
  TEST(NanoVM_try_lock(&lock, 2) == -1, "lock to lock lock(1) on thread 2 should fail");
  TEST(NanoVM_try_lock(&lock, 1) == 0, "lock lock(1) on thread 1 should success");
  return 0;
}

void* _in_thread(void* data) {
  nvm_lock_t* lock = data;
  NanoVM_lock(lock, 2);
  return &lock->tid;
}

int TEST_NanoVM_lock() {
  nvm_lock_t lock;
  NanoVM_init_lock(&lock);
  NanoVM_lock(&lock, 1);
  pthread_t thread;
  int err = pthread_create(&thread, NULL, _in_thread, &lock);
  TEST(err == 0, "pthread_create should not fail");
  err = NanoVM_unlock(&lock, 1);
  TEST(err == 0, "unlock should succeed");
  int* res;
  pthread_join(thread, (void**)&res);
  TEST(*res == 2, "NanoVM_lock should set tid to given tid");
  return 0;
}

int TEST_NanoVM_unlock() {
  nvm_lock_t lock;
  NanoVM_init_lock(&lock);
  NanoVM_lock(&lock, 1);
  TEST(lock.tid == 1, "NanoVM_lock should set tid to given tid");
  int err = NanoVM_unlock(&lock, 1);
  TEST(err == 0, "unlock should succeed");
  TEST(lock.tid == 0, "tid should be set to 0 after unlock");
  return 0;
}

int TEST_NanoVM_rand_int() {
  int res, i;
  for (i = 0; i < 65536; ++i) { // run many times
    res = NanoVM_rand_int(1024);
    TEST(res > 0, "NanoVM_rand_int should return a positive integer");
    TEST(res < 1024, "NanoVM_rand_int(1024) should return an integer not greater than 1024");
  }
  return 0;
}

int TEST_util() {
  int err =
      TEST_NanoVM_init_lock()
    & TEST_NanoVM_try_lock()
    & TEST_NanoVM_lock()
    & TEST_NanoVM_unlock()
    & TEST_NanoVM_rand_int()
  ;
  return err;
}
