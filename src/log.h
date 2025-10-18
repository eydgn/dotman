#ifndef LOG_H
#define LOG_H

typedef enum {
  LOG_ERROR,
  LOG_WARN,
  LOG_INFO
} log_level_t;

void log_message(log_level_t level, const char* file, const char* func, int line,
                 const char* fmt, ...);
#define LOG_ERROR(...) log_message(LOG_ERROR, __FILE__, __func__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...)  log_message(LOG_WARN, __FILE__, __func__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)  log_message(LOG_INFO, __FILE__, __func__, __LINE__, __VA_ARGS__)
#endif  // !LOG_H
