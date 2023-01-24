#ifndef MDPS_LOGGING_H
#define MDPS_LOGGING_H

#include "probfd/value_type.h"

#include <iostream>

namespace probfd {
namespace logging {

struct Discard
    : std::streambuf
    , std::ostream {
    Discard()
        : std::ostream(this)
    {
    }

    template <typename T>
    std::ostream& operator<<(const T&)
    {
        return *this;
    }
};

class whitespace {
public:
    explicit whitespace(int ws)
        : ws_(ws)
    {
    }
    std::ostream& print(std::ostream& out) const
    {
        int i = ws_;
        while (i-- > 0) {
            out << " ";
        }
        return out;
    }

private:
    int ws_;
};

extern std::ostream out;
extern std::ostream err;

void print_analysis_result(const value_t);

void print_analysis_result(
    const value_t,
    const bool error_supported,
    const value_t error);

void print_initial_state_value(const value_t, int spaces = 0);

} // namespace logging
} // namespace probfd

namespace std {
inline ostream& operator<<(ostream& out, const probfd::logging::whitespace& ws)
{
    return ws.print(out);
}
} // namespace std

#endif // __LOGGING_H__