#ifndef PROBFD_HEURISTIC_H
#define PROBFD_HEURISTIC_H

#include "probfd/type_traits.h"
#include "probfd/types.h"
#include "probfd/value_type.h"

namespace probfd {

/**
 * @brief The interface representing a heuristic function.
 *
 * @tparam State - The type of states for which the heuristic is evaluated.
 */
template <typename State>
class Heuristic {
public:
    virtual ~Heuristic() = default;

    /**
     * @brief Evaluates the heuristic on a given state and returns the
     * heuristic value.
     */
    virtual value_t evaluate(param_type<State> state) const = 0;

    /**
     * @brief Prints statistics, e.g. the number of queries made to the
     * interface.
     */
    virtual void print_statistics() const {}
};

} // namespace probfd

#endif // PROBFD_HEURISTIC_H