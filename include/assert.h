#ifndef STATUS_ASSERT_H
#define STATUS_ASSERT_H

#define assert(expression)

#if DEBUG
#undef assert
#define assert(expression)                                                     \
  {                                                                            \
    if (!(expression))                                                         \
      __builtin_trap();                                                        \
  }
#endif

#endif /* STATUS_ASSERT_H */
