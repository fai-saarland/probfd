#ifndef PROBFD_HEURISTICS_MAX_HEURISTIC_H
#define PROBFD_HEURISTICS_MAX_HEURISTIC_H

#include "probfd/heuristic.h"
#include "probfd/task_heuristic_factory.h"
#include "probfd/value_type.h"

#include <memory>
#include <vector>

namespace probfd::heuristics {

/**
 * @brief A heuristic that computes the maximum over several heuristics.
 */
template <typename State>
class MaxHeuristic : public Heuristic<State> {
    std::vector<std::shared_ptr<Heuristic<State>>> heuristics;

public:
    explicit MaxHeuristic(
        std::vector<std::shared_ptr<Heuristic<State>>> heuristics)
        : heuristics(std::move(heuristics))
    {
    }

    explicit MaxHeuristic(
        std::initializer_list<std::shared_ptr<Heuristic<State>>> heuristics)
        : heuristics(std::move(heuristics))
    {
    }

    template <typename... Args>
        requires(
            std::convertible_to<Args, std::shared_ptr<Heuristic<State>>> && ...)
    explicit MaxHeuristic(Args&&... args)
        : heuristics{std::forward<Args>(args)...}
    {
    }

    [[nodiscard]]
    value_t evaluate(ParamType<State> state) const override
    {
        value_t result = -INFINITE_VALUE;

        for (const auto& heuristic : heuristics) {
            result = std::max(result, heuristic->evaluate(state));
        }

        return result;
    }
};

} // namespace probfd::heuristics

#endif // PROBFD_HEURISTICS_MAX_HEURISTIC_H