#ifndef PROBFD_OCCUPATION_MEASURES_UTILS_H
#define PROBFD_OCCUPATION_MEASURES_UTILS_H

#include <ranges>
#include <vector>

#include "downward/task_proxy.h"

namespace probfd {
namespace occupation_measures {

/**
 * @brief Casts a range of facts representing a partial variable assignment to
 * a vector containing -1 for each variable that is not assigned and otherwise
 * the assigned value.
 */
std::vector<int> pasmt_to_vector(const auto& fact_range, int num_variables)
{
    std::vector<int> vec(num_variables, -1);

    for (const FactProxy fact : fact_range) {
        vec[fact.get_variable().get_id()] = fact.get_value();
    }

    return vec;
}

} // namespace occupation_measures
} // namespace probfd

#endif // PROBFD_OCCUPATION_MEASURES_UTILS_H
