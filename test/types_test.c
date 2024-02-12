#include "test.h"
#include "utils.h"

int main(void) {
  test(sizeof(u64) == 8, "u64 must be 8 bytes");
  test(sizeof(u32) == 4, "u32 must be 4 bytes");
  test(sizeof(u16) == 2, "u16 must be 2 bytes");
  test(sizeof(u8) == 1, "u8 must be 1 bytes");

  test(sizeof(i64) == 8, "i64 must be 8 bytes");
  test(sizeof(i32) == 4, "i32 must be 4 bytes");
  test(sizeof(i16) == 2, "i16 must be 2 bytes");
  test(sizeof(i8) == 1, "i8 must be 1 bytes");

  test(sizeof(f64) == 8, "f64 must be 8 bytes");
  test(sizeof(f32) == 4, "f32 must be 4 bytes");

  test_summary();
  return (int)testinfo.fail;
}
