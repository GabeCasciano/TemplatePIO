#ifndef LOGGER_H_
#define LOGGER_H_

#include <Arduino.h>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>

enum LOG_LEVEL { INFO, WARNING, ERROR };

inline const char *logString(LOG_LEVEL level) {
  switch (level) {
  case INFO:
    return "[INFO]";
  case WARNING:
    return "[WARNING]";
  case ERROR:
    return "[ERROR]";
  default:
    return "[UNKNOWN]";
  }
}

inline void LOGGER(LOG_LEVEL level, const char *fmt, ...) {
#ifdef DEBUG

  constexpr size_t BUF_SIZE = 1024;
  char buffer[BUF_SIZE];

  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, BUF_SIZE, fmt, args);
  va_end(args);

  Serial.print(logString(level));
  Serial.println(buffer);
  Serial.flush();
  delay(250);

#endif
  if (level == ERROR) {
#ifdef ESP32
    abort();
#else
    noInterrupts();
    NVIC_SystemReset();
#endif
  }
}

#endif
