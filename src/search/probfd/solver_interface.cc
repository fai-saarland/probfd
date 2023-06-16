#include "probfd/solver_interface.h"

#include "plugins/plugin.h"

#include <iomanip>

namespace probfd {

void SolverInterface::print_value(std::ostream& o, value_t value)
{
    o << std::setprecision(std::numeric_limits<long double>::digits10 + 1)
      << value;
}

void SolverInterface::print_analysis_result(Interval result)
{
    std::cout << "Analysis result:" << std::endl;
    print_initial_state_value(result);
    std::cout << "  Range of error: ";
    print_value(std::cout, result.length());
    std::cout << std::endl;
}

void SolverInterface::print_initial_state_value(Interval value, int spaces)
{
    for (int i = 0; i < spaces; ++i) std::cout << " ";
    std::cout << "Value computed for s0: [";
    print_value(std::cout, value.lower);
    std::cout << ",";
    print_value(std::cout, value.upper);
    std::cout << "]" << std::endl;
}

static class SolverInterfaceCategoryPlugin
    : public plugins::TypedCategoryPlugin<SolverInterface> {
public:
    SolverInterfaceCategoryPlugin()
        : TypedCategoryPlugin("SolverInterface")
    {
        document_synopsis("Represents a generic planning problem solver");
    }
} _category_plugin;

} // namespace probfd