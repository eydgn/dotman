#ifndef LOG_H
#define LOG_H

typedef enum {
  LOG_ERROR,
  LOG_WARN,
  LOG_INFO
} log_level_t;

void log_message(log_level_t level, const char* file, int line, const char* func,
                 const char* msg);
#define LOG_ERROR(msg) log_message(LOG_ERROR, __FILE__, __LINE__, __func__, msg)
#define LOG_WARN(msg)  log_message(LOG_WARN, __FILE__, __LINE__, __func__, msg)
#define LOG_INFO(msg)  log_message(LOG_INFO, __FILE__, __LINE__, __func__, msg)
#endif  // !LOG_H
