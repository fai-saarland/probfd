#pragma once

#include <iostream>

namespace probabilistic {

struct Discard : std::streambuf, std::ostream {
    Discard()
        : std::ostream(this)
    {
    }

    template<typename T>
    std::ostream& operator<<(const T&)
    {
        return *this;
    }
};

extern std::ostream g_log;

#if !defined(NDEBUG)
extern std::ostream g_debug;
#else
extern Discard g_debug;
#endif

extern std::ostream g_err;

} // namespace probabilistic
