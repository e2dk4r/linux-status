#ifndef STATUS_UTILS_H
#define STATUS_UTILS_H

#include "assert.h"
#include "os.h"
#include "types.h"

static inline i8 isdigit(const char value) {
  return value >= '0' && value <= '9';
}

/* parse integer from string
 * this expects to you give initialized integer
 */
static inline char *strtou64(char *str, u64 size, u64 *dest) {
  assert(str != 0);
  assert(dest != 0);
  assert(size != 0);
  char *max = str + size - 1;

  // while is not digit go to next character
  while (!isdigit(*str) && str != max) {
    str++;
  }
  do {
    *dest *= 10; /* first multiply by 10 */
    *dest += (u8)(*str - '0');

    if (str == max)
      break;

    str++;
  } while (isdigit(*str));

  return str;
}

static inline char *strchr(char *str, char key) {
  while (*str != key) {
    str++;
  }
  return str;
}

struct string {
  char *buf;
  u64 len;
};

static inline struct string u64tostr(u64 value, char *buf, u64 size) {
  assert(buf != 0);
  assert(size > 0);

  char *p;
  char *last = buf + size - 1;

  p = last;
  do {
    *p = '0' + (value % 10);
    value /= 10;

    if (value == 0)
      break;

    if (p == buf) {
      if (value != 0)
        continue;
      break;
    }

    p--;
  } while (1);

  u64 len = (u64)(last - p) + 1;
  return (struct string){
      .buf = p,
      .len = len,
  };
}

static inline void print_u64(u64 value, char *buf, u64 size) {
  struct string str = u64tostr(value, buf, size);
  write(SYSOUT, str.buf, str.len);
}

/* gb to kb */
static inline u64 gbtokb(u64 gb) { return gb << 20; }

/* kb to gb */
static inline u64 kbtogb(u64 kb) { return kb >> 20; }

/* kb to mb */
static inline u64 kbtomb(u64 kb) { return kb >> 10; }

/* b to mb */
static inline u64 btomb(u64 b) { return b >> 20; }

/* b to kb */
static inline u64 btokb(u64 b) { return b >> 10; }

/* mb to b */
static inline u64 mbtob(u64 b) { return b << 20; }

static inline void *memcpy(void *dest, const void *src, u64 sz) {
  assert(dest != 0);
  assert(src != 0);
  assert(sz > 0);
  u8 *dest8 = (u8 *)dest;
  u8 *src8 = (u8 *)src;

  u64 block;
  u64 blocks;
  u64 remaining = sz;

  blocks = sz >> 3;
  for (block = 0; block < blocks; block++) {
    *(u64 *)(dest8) = *(u64 *)(src8);
    src8 += 8;
    dest8 += 8;
  }
  remaining -= blocks << 3;
  if (remaining == 0)
    return dest;

  blocks = remaining >> 2;
  for (block = 0; block < blocks; block++) {
    *(u32 *)(dest8) = *(u32 *)(src8);
    src8 += 4;
    dest8 += 4;
  }
  remaining -= blocks << 2;
  if (remaining == 0)
    return dest;

  blocks = remaining >> 1;
  for (block = 0; block < blocks; block++) {
    *(u16 *)(dest8) = *(u16 *)(src8);
    src8 += 2;
    dest8 += 2;
  }
  remaining -= blocks << 1;
  if (remaining == 0)
    return dest;

  for (block = 0; block < remaining; block++) {
    *dest8 = *src8;
    src8 += 1;
    dest8 += 1;
  }

#if 0
  void *max = (void *)(src8 + sz);

  while (src8 != max) {
    *dest8 = *src8;
    dest8++;
    src8++;
  }
#endif

  return dest;
}

static inline i8 memcmp(void *left, void *right, u64 sz) {
  assert(left != 0);
  assert(right != 0);
  assert(sz > 0);
  i8 result;
  void *max = (u8 *)right + sz - 1;

  u8 *left8 = left;
  u8 *right8 = right;

  while (1) {
    result = (i8)(*left8 - *right8);

    if (result != 0)
      return result;

    left8++;
    right8++;
    if (right8 == max)
      break;
  }

  return 0;
}

static inline char *strstrn(char *str, u64 str_sz, char *substring, u64 substring_sz) {
  assert(str != 0);
  assert(str_sz > 0);
  assert(substring != 0);
  assert(substring_sz > 0);

  char *pstr = str;
  char *pstr_max = str + str_sz;
  while (1) {
    if (memcmp(pstr, substring, substring_sz) == 0)
      return pstr;

    pstr++;
    if (pstr == pstr_max)
        break;
  }

  return (char *)-1;
}

#endif /* STATUS_UTILS_H */
