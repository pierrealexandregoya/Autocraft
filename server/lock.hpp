#pragma

#ifndef AUTOCRAFT_MONOTHREAD
# include <mutex>
# define AUTOCRAFT_LOCK(m) std::lock_guard<decltype(m)> _my_lock(m)
#else /* !AUTOCRAFT_MONOTHREAD */
# define AUTOCRAFT_LOCK(m) do {} while(0)
#endif /* !AUTOCRAFT_MONOTHREAD */
