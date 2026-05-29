#ifndef PROBFD_HEURISTICS_CONSTANT_HEURISTIC_H
#define PROBFD_HEURISTICS_CONSTANT_HEURISTIC_H

#include "probfd/heuristic.h"
#include "probfd/value_type.h"

namespace probfd::heuristics {

/**
 * @brief A heuristic returning a constant estimate for each state.
 */
template <typename State>
class ConstantHeuristic : public Heuristic<State> {
    const value_t value_;

public:
    /**
     * @brief Construct with constant estimate \p value .
     */
    explicit ConstantHeuristic(value_t value)
        : value_(value)
    {
    }

    [[nodiscard]]
    value_t evaluate(ParamType<State>) const override
    {
        return value_;
    }
};

} // namespace probfd::heuristics

#endif // PROBFD_HEURISTICS_CONSTANT_HEURISTIC_H