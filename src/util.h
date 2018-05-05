#ifndef NANOVM_UTIL_H
#define NANOVM_UTIL_H 1

typedef struct {
  int tid; // it's the nanovm thread id, not os tid
} lock_t;

 int NanoVM_init_lock(lock_t* lock);
 int NanoVM_try_lock (lock_t* lock);
void NanoVM_lock     (lock_t* lock);
 int NanoVM_unlock   (lock_t* lock);

int NanoVM_rand_int(int max);

void write_log(int e, const char *fmt, ...);

#define debug_log0(fmt) write_log(0, fmt)
#define debug_log1(fmt, a) write_log(0, fmt, a)
#define error_log0(fmt) write_log(1, fmt)
#define error_log1(fmt, a) write_log(1, fmt, a)

#endif
