#include "probfd/solver_interface.h"

#include "probfd/interval.h"

#include <iomanip>
#include <iostream>
#include <print>

namespace probfd {

void print_analysis_result(Interval result)
{
    std::println(std::cout, "Analysis result:");
    print_initial_state_value(result);
}

void print_initial_state_value(Interval value, int spaces)
{
    if (spaces == 0) {
        std::println(
            std::cout,
            "Objective value for the initial state lies within the interval {::.5f}",
            value);
    } else {
        std::println(
            std::cout,
            "{:{}}Objective value computed for s0 lies within bound {::.5f}",
            ' ',
            spaces,
            value);
    }
}

} // namespace probfd