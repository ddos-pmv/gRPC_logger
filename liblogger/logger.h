#pragma once

#include <atomic>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>

namespace logger {

enum class LogLevel {
  DEBUG,
  INFO,
  WARNING,
  ERROR,
};

struct RingBuffer {
  int counter = 0;
};

inline const char* toString(LogLevel level) {
  static const char* levelStr[] = {"DEBUG", "INFO", "WARNING", "ERROR"};
  return levelStr[static_cast<int>(level)];
}

class Logger {
 public:
  // Удаляем копирование и присваивание
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;

  // Получение экземпляра (потокобезопасное)
  static Logger& getInstance();

  // Thread-local буфер
  static thread_local RingBuffer buf;

  // Установка выходного потока
  void setOutStream(std::ostream& oStream);

  // // Инициализация файлового вывода
  // void init(const std::string& filename) {
  //   std::lock_guard lock(mtx_);
  //   file_.open(filename, std::ios::app);
  //   if (file_.is_open()) {
  //     out_ = &file_;
  //   }
  // }

  // Основная функция логирования
  void log(LogLevel level, const std::string& file, int line,
           const std::string& msg);

 private:
  Logger() = default;   // Приватный конструктор
  ~Logger() = default;  // Приватный деструктор

  std::mutex mtx_;
  // std::ofstream file_;
  std::ostream* out_ = &std::cout;  // По умолчанию вывод в консоль
};


}  // namespace logger