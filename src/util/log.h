
#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdlib.h>

#define LOG(...)                                                               \
  do {                                                                         \
    fprintf(stdout, "[\x1b[0m----\x1b[0m] " __VA_ARGS__);                      \
  } while (0)

#define LOG_GOOD(...)                                                          \
  do {                                                                         \
    fprintf(stdout, "[\x1b[32mOK\x1b[0m] " __VA_ARGS__);                       \
  } while (0)

#define LOG_INFO(...)                                                          \
  do {                                                                         \
    fprintf(stdout, "[\x1b[34mINFO\x1b[0m] " __VA_ARGS__);                     \
  } while (0)

#define LOG_WARN(...)                                                          \
  do {                                                                         \
    fprintf(stdout, "\x1b[33m[WARN]" __VA_ARGS__);                             \
    fprintf(stdout, "\x1b[0m");                                                \
  } while (0)

#define LOG_ERROR(...)                                                         \
  do {                                                                         \
    fprintf(stdout, "\x1b[31m[ERROR] " __VA_ARGS__);                           \
    fprintf(stdout, "\x1b[0m");                                                \
  } while (0)

#define LOG_NEWLINE() fprintf(stdout, "\n");

#endif
