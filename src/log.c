#include "log.h"

#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

static const char* level_colors[] = {
    [LOG_ERROR] = "\033[1;31m",  // Bold Red
    [LOG_WARN]  = "\033[1;33m",  // Bold Yellow
    [LOG_INFO]  = "\033[1;32m",  // Bold Green
};

static const char* level_names[] = {
    [LOG_ERROR] = "ERROR",
    [LOG_WARN]  = "WARN",
    [LOG_INFO]  = "INFO",
};

static const char* COLOR_RESET = "\033[0m";

void log_message(log_level_t level, const char* file, int line, const char* fmt, ...) {
  int use_color = isatty(fileno(stderr));

  if (use_color) {
    fprintf(stderr, "%s[%s]%s ", level_colors[level], level_names[level], COLOR_RESET);
  } else {
    fprintf(stderr, "[%s] ", level_names[level]);
  }

  if (level == LOG_ERROR) {
    fprintf(stderr, "%s:%d -> ", file, line);
  }

  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);

  fprintf(stderr, "\n");

  fflush(stderr);
}
