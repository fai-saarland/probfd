#include "logging.h"

namespace probabilistic {

std::ostream g_log(std::cout.rdbuf());

#if !defined(NDEBUG)
std::ostream g_debug(std::cout.rdbuf());
#else
Discard g_debug;
#endif

std::ostream g_err(std::cerr.rdbuf());

} // namespace probabilistic
