#include "test.h"
#include "utils.h"

int main(void) {
  print("strtou64():\n");
  {
    char buf[] = "643";
    u64 value = 0;

    strtou64(buf, sizeof(buf), &value);
    test(value == 643, "\"643\" must be 643");
  }
  {
    char buf[] = "857";
    u64 value = 0;

    strtou64(buf, 1, &value);
    test(value == 8, "\"857\" must be 8 if buffer capacity is 1");
  }

  print("u64tostr():\n");
  {
    u64 value = 534;
    char buf[64];
    struct string result;

    result = u64tostr(value, buf, sizeof(buf));
    test(result.len == 3, "534 must be 3 characters long");
    test(result.buf[0] == '5' && result.buf[1] == '3' && result.buf[2] == '4',
         "534 must give \"534\" in string");

    result = u64tostr(value, buf, 1);
    test(result.len == 1,
         "534 must be 1 characters long if buffer capacity is 1");
    test(result.buf[0] == '5',
         "534 must give \"5\" in string if buffer capacity is 1");

    value = 0;
    result = u64tostr(value, buf, sizeof(buf));
    test(result.len == 1, "0 must be 1 characters long");
    test(result.buf[0] == '0', "0 must give \"0\" in string");
  }

  print("gbtokb():\n");
  { test(gbtokb(1) == (u64)1048576, "1gb must be 1048576kb"); }

  print("kbtogb():\n");
  {
    test(kbtogb(1048576) == 1, "1048576kb must be 1gb");
    test(kbtogb(1048575) == 0, "1048575kb must be 0gb");
  }

  print("kbtomb():\n");
  {
    test(kbtomb(5120) == 5, "5120kb must be 5mb");
    test(kbtomb(5119) == 4, "5119kb must be 4mb");
  }

  print("btomb():\n");
  {
    test(btomb(1048576) == 1, "1048576b must be 1mb");
    test(btomb(1048575) == 0, "1048575b must be 0mb");
  }

  print("btokb():\n");
  {
    test(btokb(1024) == 1, "1024b must be 1kb");
    test(btokb(1023) == 0, "1023b must be 0kb");
  }

  print("memcpy():\n");
  {
    char from[] = "abc";
    char to[4];

    memcpy(to, from, sizeof(from));
    test(to[0] == from[0] && to[1] == from[1] && to[2] == from[2] &&
             to[3] == from[3],
         "abc must be abc");
  }
  {
    char from[] = "abc";
    char to[2];

    memcpy(to, from, 2);
    test(to[0] == from[0] && to[1] == from[1],
         "abc must be ab with buffer size 2");
  }
  {
    char from[] =
        "272f01fc07224b4bee25da849fbe43be5a811ed673341aa55af3d07196f9d27cb0d540"
        "46f553a42a4cc20c008582e83ea0f8d0fe1908962df6303f44bcc7988e";
    char to[129];

    memcpy(to, from, sizeof(from));
    test(to[0] == from[0] && to[1] == from[1] && to[2] == from[2] &&
             to[3] == from[3],
         "really long string must be copied");
  }
  {
    i64 from = 5;
    i64 to;

    memcpy(&to, &from, sizeof(from));
    test(to == from, "5 must be 5");
  }

  print("memcmp():\n");
  {
    char right[] =
        "272f01fc07224b4bee25da849fbe43be5a811ed673341aa55af3d07196f9d27cb0d540"
        "46f553a42a4cc20c008582e83ea0f8d0fe1908962df6303f44bcc7988e";

    test(memcmp(right, right, sizeof(right)) == 0, "strings must be same");
  }
  {
    char left[] = "abcdef";
    char right[] = "abc";

    test(memcmp(left, right, sizeof(right)) == 0, "strings must be same when subset");
  }
  {
    char left[] = "abd";
    char right[] = "abc";

    test(memcmp(left, right, sizeof(right)) != 0, "strings must not be same");
  }

  print("strstrn():\n");
  {
    char left[] = "start_middle_finish";
    char right[] = "start";

    test(strstrn(left, sizeof(left), right, sizeof(right)) == left, "must return address from start");
  }
  {
    char left[] = "start_middle_finish";
    char right[] = "middle";

    test(strstrn(left, sizeof(left), right, sizeof(right)) == left + 6, "must return address from middle");
  }
  {
    char left[] = "start_middle_finish";
    char right[] = "finish";

    test(strstrn(left, sizeof(left), right, sizeof(right)) == left + 13, "must return address from finish");
  }
  {
    char left[] = "start_middle_finish";
    char right[] = "none";

    test(strstrn(left, sizeof(left), right, sizeof(right)) == (char*)-1, "must not return any address");
  }

  test_summary();
  return (int)testinfo.fail;
}
