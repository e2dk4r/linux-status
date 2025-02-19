#ifndef TEST_H
#define TEST_H

#include "os.h"
#include "types.h"
#include "utils.h"

static struct {
  u64 fail;
  u64 count;
} testinfo = {
    .fail = 0,
    .count = 0,
};

void test_ok(char *msg, u64 len) {
  print("[x] ");
  write(STDOUT_FILENO, msg, len);
  print("\n");
  testinfo.count++;
}
void test_fail(char *msg, u64 len) {
  print("[ ] ");
  write(STDOUT_FILENO, msg, len);
  print("\n");
  testinfo.count++;
  testinfo.fail++;
}

void test_summary(void) {
  char buf[256];

  print("Summary: ");
  print("\n");
  print("  from ");
  print_u64(testinfo.count, buf, sizeof(buf));
  print(" tests ");
  print_u64(testinfo.fail, buf, sizeof(buf));
  print(" failed and ");
  print_u64(testinfo.count - testinfo.fail, buf, sizeof(buf));
  print(" were successful\n");
}

#define test(predicate, message)                                               \
  if (predicate) {                                                             \
    test_ok(message, sizeof(message) - 1);                                     \
  } else {                                                                     \
    test_fail(message, sizeof(message) - 1);                                   \
  }

#endif /* TEST_H */
