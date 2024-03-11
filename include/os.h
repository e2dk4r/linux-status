#ifndef STATUS_OS_H
#define STATUS_OS_H

#include "types.h"

#define O_RDONLY 0
#define SYSOUT 1
#define NR_READ 0
#define NR_WRITE 1
#define NR_OPEN 2
#define NR_CLOSE 3
#define NR_NANOSLEEP 35

/* syscalls */
extern void *syscall1(u64 number, void *arg1);
extern void *syscall2(u64 number, void *arg1, void *arg2);
extern void *syscall3(u64 number, void *arg1, void *arg2, void *arg3);
extern void *syscall4(u64 number, void *arg1, void *arg2, void *arg3,
                      void *arg4);
extern void *syscall5(u64 number, void *arg1, void *arg2, void *arg3,
                      void *arg4, void *arg5);

/* used syscalls in status */
static inline i64 read(i64 fd, void *buf, u64 count) {
  return (i64)syscall3(NR_READ, (void *)fd, buf, (void *)count);
}

static inline i64 write(i64 fd, const char *buf, u64 count) {
  return (i64)syscall3(NR_WRITE, (void *)fd, (void *)buf, (void *)count);
}

/* use only with strings that are static, no dynamic */
#define print(str) write(SYSOUT, str, sizeof(str) - 1)

static inline i64 open(const char *pathname, i32 flags) {
  return (i64)syscall2(NR_OPEN, (void *)pathname, (void *)(i64)flags);
}

static inline i64 close(i64 fd) { return (i64)syscall1(3, (void *)fd); }

struct timespec {
  u64 tv_sec;
  i64 tv_nsec;
};
static inline i64 nanosleep(struct timespec *req, struct timespec *rem) {
  return (i64)syscall2(NR_NANOSLEEP, req, rem);
}

#endif /* STATUS_OS_H */
