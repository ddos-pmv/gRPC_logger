#include <time.h>

#include <cstdint>
#include <string_view>
#include <type_traits>

#include "logger.h"
#include "utils.h"

namespace logger {

inline uint32_t get_timestamp() {
  static thread_local timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  return static_cast<uint32_t>(ts.tv_sec);
}

template <typename MsgType>
void log(LogLevel level, const char* file, uint16_t line, MsgType&& msg) {
  /* thread_local variables */
  static thread_local bool is_added = false;
  static thread_local RingBuffer<PackedEntry, kRingBufferSize> ring_buffer;

  static thread_local PackedEntry entry;

  entry.log_level = static_cast<uint8_t>(level);
  entry.log_line = line;
  entry.log_timestamp = get_timestamp();
  std::strncpy(entry.log_file, file, sizeof(entry.log_file));

  if constexpr (std::is_convertible_v<MsgType, std::string_view>) {
    std::string_view sv = msg;
    size_t msg_len = std::min(sv.length(), sizeof(entry.message) - 1);
    std::strncpy(entry.message, sv.data(), msg_len);
    entry.message[msg_len] = '\0';
  } else {
    static_assert([] { return false; }(), "Unsupported msg type");
  }
  ring_buffer.write(entry);

  buffers_queue.push(&ring_buffer);
  // if (!is_added) {
  //   buffers_queue_.push(&ring_buffer);
  //   is_added = true;
  //   std::cout << "new thread" << '\n';
  // }

  std::cout << "message logged: " << '\n';
}

}  // namespace logger