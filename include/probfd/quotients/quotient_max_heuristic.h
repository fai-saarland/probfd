#ifndef PROBFD_QUOTIENTS_QUOTIENT_MAX_HEURISTIC_H
#define PROBFD_QUOTIENTS_QUOTIENT_MAX_HEURISTIC_H

#include "probfd/quotients/quotient_system.h"

#include "probfd/heuristic.h"

namespace probfd::quotients {

template <typename State, typename Action>
class QuotientMaxHeuristic : public Heuristic<QuotientState<State, Action>> {
    using QState = QuotientState<State, Action>;

    const Heuristic<State>& original_;

public:
    explicit QuotientMaxHeuristic(const Heuristic<State>& original)
        : original_(original)
    {
    }

    value_t evaluate(ParamType<QState> state) const override
    {
        return state.member_maximum(
            std::bind_front(&Heuristic<State>::evaluate, std::ref(original_)));
    }

    void print_statistics() const final { original_.print_statistics(); }
};

} // namespace probfd::quotients

#endif // PROBFD_QUOTIENTS_QUOTIENT_MAX_HEURISTIC_H