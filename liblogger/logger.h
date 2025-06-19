#pragma once

#include <atomic>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>

#include "ring_buffer.h"

namespace logger {

constexpr size_t kRingBufferSize = 8192;

enum class LogLevel {
  DEBUG,
  INFO,
  WARNING,
  ERROR,
};

inline const char* toString(LogLevel level) {
  static const char* levelStr[] = {"DEBUG", "INFO", "WARNING", "ERROR"};
  return levelStr[static_cast<int>(level)];
}

struct PackedEntry {
  uint8_t log_level;
  uint16_t log_line;
  uint32_t log_timestamp;
  char log_file[64];
  char message[256];
} __attribute__((packed));

class Logger {
 public:
  // Удаляем копирование и присваивание
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;

  // Основная функция логирования
  template <typename MsgType>
  static void log(LogLevel level, const std::string& file, int line,
                  MsgType&& msg);

  // Thread-local буфер
  static thread_local RingBuffer<PackedEntry, kRingBufferSize> ring_buffer;

  // Установка выходного потока
  void setOutStream(std::ostream& oStream);

 private:
  Logger() = default;   // Приватный конструктор
  ~Logger() = default;  // Приватный деструктор

  std::mutex mtx_;
  std::ostream* out_ = &std::cout;  // По умолчанию вывод в консоль
};

}  // namespace logger

#include "logger.tpp"