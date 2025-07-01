#pragma once

#include <boost/lockfree/queue.hpp>
#include <cstdint>

#include "ring_buffer.h"
#include "utils.h"
namespace logger {

// Основная функция логирования
template <typename MsgType>
void log(LogLevel level, const char* file, uint16_t line, MsgType&& msg);

inline static boost::lockfree::queue<RingBuffer<PackedEntry, kRingBufferSize>*>
    buffers_queue_;

}  // namespace logger

#include "logger.tpp"