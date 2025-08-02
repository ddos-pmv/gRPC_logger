#pragma once

#include <cstdint>
#include <string>

namespace grpclog {

enum class LogLevel : uint8_t {
  TRACE = 0,
  DEBUG = 1,
  INFO = 2,
  WARN = 3,
  ERROR = 4,
  CRITICAL = 5
};

// ==================== CONFIGURATION ====================

struct Config {
  std::string server_address = "localhost:50051";
  size_t queue_capacity = 65536;          // Number of messages in queue
  size_t batch_size = 100;                // Messages per gRPC batch
  uint32_t flush_interval_ms = 100;       // Max time before force flush
  uint32_t connection_timeout_ms = 5000;  // gRPC connection timeout
  uint32_t retry_attempts = 3;            // Connection retry attempts
  bool fallback_to_console = false;       // Fallback if server unavailable
  LogLevel min_level = LogLevel::TRACE;   // Minimum level to log
};

// ==================== INITIALIZATION ====================

// Initialize with default config (auto-connects on first log)
void init();

// Initialize with server address
void init(const std::string& server_address);

// Initialize with full config
void init(const Config& config);

// Shutdown and flush all pending logs
void shutdown();

// Check if initialized
bool is_initialized();

// ==================== MAIN LOGGING API ====================

// Simple string logging
void trace(const std::string& msg, const char* file = __builtin_FILE(),
           int line = __builtin_LINE());
void debug(const std::string& msg, const char* file = __builtin_FILE(),
           int line = __builtin_LINE());
void info(const std::string& msg, const char* file = __builtin_FILE(),
          int line = __builtin_LINE());
void warn(const std::string& msg, const char* file = __builtin_FILE(),
          int line = __builtin_LINE());
void error(const std::string& msg, const char* file = __builtin_FILE(),
           int line = __builtin_LINE());
void critical(const std::string& msg, const char* file = __builtin_FILE(),
              int line = __builtin_LINE());

// Generic log function
void log(LogLevel level, const std::string& msg,
         const char* file = __builtin_FILE(), int line = __builtin_LINE());

// Format versions (C++20 style format - simplified for now)
template <typename... Args>
void trace_f(const std::string& fmt, Args&&... args);

template <typename... Args>
void debug_f(const std::string& fmt, Args&&... args);

template <typename... Args>
void info_f(const std::string& fmt, Args&&... args);

template <typename... Args>
void warn_f(const std::string& fmt, Args&&... args);

template <typename... Args>
void error_f(const std::string& fmt, Args&&... args);

template <typename... Args>
void critical_f(const std::string& fmt, Args&&... args);

// ==================== STATISTICS ====================

struct Stats {
  uint64_t messages_enqueued = 0;    // Total messages added to queue
  uint64_t messages_sent = 0;        // Messages successfully sent to server
  uint64_t messages_dropped = 0;     // Messages dropped due to queue full
  uint64_t queue_size = 0;           // Current queue size
  uint64_t total_bytes_sent = 0;     // Total bytes sent over network
  bool connected = false;            // Connection status
  uint32_t connection_attempts = 0;  // Number of connection attempts
  uint64_t last_send_timestamp = 0;  // Last successful send (nanoseconds)
};

Stats get_stats();
void reset_stats();

// ==================== UTILITY FUNCTIONS ====================

// Convert log level to string
const char* level_to_string(LogLevel level);

// Convert string to log level
LogLevel string_to_level(const std::string& level);

// Set minimum log level (messages below this level are ignored)
void set_min_level(LogLevel level);

// Get current minimum log level
LogLevel get_min_level();

// Force flush all pending messages
void flush();

// Check if logger is currently connected to server
bool is_connected();

}  // namespace grpclog

// ==================== CONVENIENCE MACROS ====================

// Simple macros (recommended)
#define GLOG_TRACE(msg) ::grpclog::trace(msg, __FILE__, __LINE__)
#define GLOG_DEBUG(msg) ::grpclog::debug(msg, __FILE__, __LINE__)
#define GLOG_INFO(msg) ::grpclog::info(msg, __FILE__, __LINE__)
#define GLOG_WARN(msg) ::grpclog::warn(msg, __FILE__, __LINE__)
#define GLOG_ERROR(msg) ::grpclog::error(msg, __FILE__, __LINE__)
#define GLOG_CRITICAL(msg) ::grpclog::critical(msg, __FILE__, __LINE__)

// Format macros (when C++20 format is not available)
#define GLOG_TRACE_F(fmt, ...) ::grpclog::trace_f(fmt, __VA_ARGS__)
#define GLOG_DEBUG_F(fmt, ...) ::grpclog::debug_f(fmt, __VA_ARGS__)
#define GLOG_INFO_F(fmt, ...) ::grpclog::info_f(fmt, __VA_ARGS__)
#define GLOG_WARN_F(fmt, ...) ::grpclog::warn_f(fmt, __VA_ARGS__)
#define GLOG_ERROR_F(fmt, ...) ::grpclog::error_f(fmt, __VA_ARGS__)
#define GLOG_CRITICAL_F(fmt, ...) ::grpclog::critical_f(fmt, __VA_ARGS__)

// Conditional logging (only log if condition is true)
#define GLOG_INFO_IF(condition, msg)                         \
  do {                                                       \
    if (condition) ::grpclog::info(msg, __FILE__, __LINE__); \
  } while (0)

#define GLOG_ERROR_IF(condition, msg)                         \
  do {                                                        \
    if (condition) ::grpclog::error(msg, __FILE__, __LINE__); \
  } while (0)

// Periodic logging (only log every N calls)
#define GLOG_INFO_EVERY_N(n, msg)                                             \
  do {                                                                        \
    static int _glog_counter = 0;                                             \
    if (++_glog_counter % (n) == 1) ::grpclog::info(msg, __FILE__, __LINE__); \
  } while (0)

// First N times logging
#define GLOG_INFO_FIRST_N(n, msg)                                         \
  do {                                                                    \
    static int _glog_counter = 0;                                         \
    if (++_glog_counter <= (n)) ::grpclog::info(msg, __FILE__, __LINE__); \
  } while (0)