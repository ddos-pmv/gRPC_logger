#pragma once

#include "logger.h"

#define LOG(level, string) \
  ::logger::log(::logger::LogLevel::DEBUG, __FILE__, __LINE__, string)