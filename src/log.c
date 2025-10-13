#include "log.h"

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

void log_message(log_level_t level, const char* file, int line, const char* func,
                 const char* msg) {
  int use_color = isatty(fileno(stderr));

  if (use_color) {
    (void) fprintf(stderr, "%s[%s]%s ", level_colors[level], level_names[level],
                   COLOR_RESET);
  } else {
    (void) fprintf(stderr, "[%s] ", level_names[level]);
  }

  if (level == LOG_ERROR) {
    (void) fprintf(stderr, "%s:%s():%d -> ", file, func, line);
  }

  (void) fprintf(stderr, "%s\n", msg);

  (void) fflush(stderr);
}
