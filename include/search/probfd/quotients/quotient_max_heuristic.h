#ifndef PROBFD_QUOTIENT_SYSTEM_HEURISTIC_SEARCH_INTERFACE_H
#define PROBFD_QUOTIENT_SYSTEM_HEURISTIC_SEARCH_INTERFACE_H

#include "probfd/algorithms/fdr_types.h"
#include "probfd/algorithms/open_list.h"
#include "probfd/algorithms/policy_picker.h"
#include "probfd/algorithms/successor_sampler.h"

#include "probfd/quotients/quotient_system.h"

#include "probfd/evaluator.h"

namespace probfd {

namespace quotients {

template <typename State, typename Action>
class QuotientMaxHeuristic : public Evaluator<QuotientState<State, Action>> {
    using QuotientState = QuotientState<State, Action>;

    const Evaluator<State>& original_;

public:
    QuotientMaxHeuristic(const Evaluator<State>& original)
        : original_(original)
    {
    }

    value_t evaluate(param_type<QuotientState> state) const override
    {
        return state.member_maximum(
            std::bind_front(&Evaluator<State>::evaluate, std::ref(original_)));
    }

    void print_statistics() const final override
    {
        original_.print_statistics();
    }
};

} // namespace quotients
} // namespace probfd

#endif // __state_properties.h_H__