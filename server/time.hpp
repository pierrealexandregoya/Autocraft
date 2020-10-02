#pragma once

#include <chrono>

#include "common.hpp"

namespace autocraft {

using steady_clock = std::chrono::steady_clock;

typedef std::chrono::duration<double, std::ratio<1>> readable_duration;

extern steady_clock::time_point start_time;

} // namespace autocraft
