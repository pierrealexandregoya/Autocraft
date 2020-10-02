#pragma once

#include <iomanip>
#include <iostream>
#include <mutex>
#include <thread>

#include "config.hpp"

namespace autocraft {

AUTOCRAFT_API extern bool io_log;
AUTOCRAFT_API extern bool data_log;
#ifndef AUTOCRAFT_MONOTHREAD
AUTOCRAFT_API extern std::recursive_mutex log_mutex;
#endif /* !AUTOCRAFT_MONOTHREAD */
AUTOCRAFT_API extern unsigned int title_max_size;
AUTOCRAFT_API extern std::ostream *log_stream;

} // namespace autocraft


#if (!defined(AUTOCRAFT_TESTS) && !defined(AUTOCRAFT_NO_LOG)) || (defined(AUTOCRAFT_TESTS) && !defined(NDEBUG))
# include "lock.hpp"
# ifdef AUTOCRAFT_BUILDING_THE_LIB
#  define _AUTOCRAFT_MY_LOG(to_log)\
do {\
AUTOCRAFT_LOCK(log_mutex);\
*log_stream << to_log << std::endl;\
} while (0)
# else /* AUTOCRAFT_BUILDING_THE_LIB */
#  define _AUTOCRAFT_MY_LOG(to_log)\
do {\
AUTOCRAFT_LOCK(autocraft::log_mutex);\
*autocraft::log_stream << to_log << std::endl;\
} while (0)
# endif /* AUTOCRAFT_BUILDING_THE_LIB */
#else /* (!defined(AUTOCRAFT_TESTS) && !defined(AUTOCRAFT_NO_LOG)) || (defined(AUTOCRAFT_TESTS) && !defined(NDEBUG)) */
# define _AUTOCRAFT_MY_LOG(to_log) do {} while (0)
#endif /* (!defined(AUTOCRAFT_TESTS) && !defined(AUTOCRAFT_NO_LOG)) || (defined(AUTOCRAFT_TESTS) && !defined(NDEBUG)) */

#ifdef AUTOCRAFT_BUILDING_THE_LIB
# define AUTOCRAFT_LOG(title, to_log) _AUTOCRAFT_MY_LOG("[" << std::setw(5) << std::this_thread::get_id() << "] [" << std::setw(title_max_size) << title << "] " << to_log)
#else /* AUTOCRAFT_BUILDING_THE_LIB */
# define AUTOCRAFT_LOG(title, to_log) _AUTOCRAFT_MY_LOG("[" << std::setw(5) << std::this_thread::get_id() << "] [" << std::setw(autocraft::title_max_size) << title << "] " << to_log)
#endif /* AUTOCRAFT_BUILDING_THE_LIB */
