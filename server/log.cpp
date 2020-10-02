#include "log.hpp"

namespace autocraft {

bool io_log = false;
bool data_log = false;

#ifndef AUTOCRAFT_MONOTHREAD
std::recursive_mutex log_mutex;
#endif /* !AUTOCRAFT_MONOTHREAD */
unsigned int title_max_size = 50;
std::ostream *log_stream = &std::cout;

} // namespace autocraft
