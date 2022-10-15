#include "probfd/utils/logging.h"

#include <iomanip>
#include <limits>

namespace probfd {
namespace logging {

std::ostream out(std::cout.rdbuf());
std::ostream err(std::cerr.rdbuf());

static void print_value(std::ostream& o, const value_type::value_t value)
{
    o << std::setprecision(std::numeric_limits<long double>::digits10 + 1)
      << value;
}

void print_analysis_result(const value_type::value_t res)
{
    print_analysis_result(res, false, 0);
}

void print_analysis_result(
    const value_type::value_t result,
    const bool error_supported,
    const value_type::value_t error)
{
    out << "Analysis result:" << std::endl;
    print_initial_state_value(result, 2);
    out << "  Estimated value error: ";
    if (error_supported) {
        print_value(out, error);
    } else {
        out << "NaN";
    }
    out << std::endl;
}

void print_initial_state_value(const value_type::value_t value, int spaces)
{
    out << whitespace(spaces);
    out << "Value computed for s0: ";
    print_value(out, value);
    out << std::endl;
}

} // namespace logging
} // namespace probfd
