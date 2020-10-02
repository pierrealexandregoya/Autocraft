#pragma once

#define AUTOCRAFT_NON_COPYABLE(T)\
T(T const&) = delete;\
T &operator=(T const&) = delete;
