
#ifndef PROBFD_UTILS_NOT_IMPLEMENTED_H
#define PROBFD_UTILS_NOT_IMPLEMENTED_H

#include <source_location>
#include <string>

namespace probfd {

[[noreturn]]
extern void not_implemented(
    const std::source_location& source_location =
        std::source_location::current());

} // namespace probfd

#endif
