#ifndef PROBFD_EVALUATOR_H
#define PROBFD_EVALUATOR_H

#include "probfd/type_traits.h"
#include "probfd/types.h"
#include "probfd/value_type.h"

namespace probfd {

/**
 * @brief The interface representing heuristic functions.
 *
 * @tparam State - The state type of the underlying MDP model.
 */
template <typename State>
class Evaluator {
public:
    virtual ~Evaluator() = default;

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

#endif // PROBFD_EVALUATOR_H