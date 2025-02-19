#ifndef STATUS_OS_H
#define STATUS_OS_H

#include "types.h"

#define O_RDONLY 0
#define STDOUT_FILENO 1

// see: /usr/include/asm/unistd_64.h
#define __NR_read 0
#define __NR_write 1
#define __NR_open 2
#define __NR_close 3
#define __NR_lseek 8
#define __NR_nanosleep 35

// see: /usr/include/fcntl.h
#define SEEK_SET 0 /* Seek from beginning of file. */
#define SEEK_CUR 1 /* Seek from current position. */
#define SEEK_END 2 /* Seek from end of file. */

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
  return (i64)syscall3(__NR_read, (void *)fd, buf, (void *)count);
}

static inline i64 write(i64 fd, const char *buf, u64 count) {
  return (i64)syscall3(__NR_write, (void *)fd, (void *)buf, (void *)count);
}

/* use only with strings that are static, no dynamic */
#define print(str) write(STDOUT_FILENO, str, sizeof(str) - 1)

static inline i64 open(const char *pathname, i32 flags) {
  return (i64)syscall2(__NR_open, (void *)pathname, (void *)(i64)flags);
}

static inline i64 close(i64 fd) {
  return (i64)syscall1(__NR_close, (void *)fd);
}

static inline i64 lseek(i64 fd, i64 offset, i64 whence) {
  return (i64)syscall3(__NR_lseek, (void *)fd, (void *)offset, (void *)whence);
}

struct timespec {
  u64 tv_sec;
  i64 tv_nsec;
};
static inline i64 nanosleep(struct timespec *req, struct timespec *rem) {
  return (i64)syscall2(__NR_nanosleep, req, rem);
}

#endif /* STATUS_OS_H */
