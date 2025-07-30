#pragma once

#include <boost/lockfree/queue.hpp>
#include <cstdint>

#include "ring_buffer.h"
#include "utils.h"
#include "worker_pool.h"
namespace logger {

// Основная функция логирования
template <typename MsgType>
void log(LogLevel level, const char* file, uint16_t line, MsgType&& msg);

}  // namespace logger

#include "logger.tpp"