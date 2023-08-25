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

class RepresentativeOpenList
    : public algorithms::OpenList<QuotientAction<OperatorID>> {
    using QuotientAction = QuotientAction<OperatorID>;

    std::shared_ptr<FDROpenList> original_;

public:
    RepresentativeOpenList(std::shared_ptr<FDROpenList> original)
        : original_(original)
    {
    }

    StateID pop() override { return original_->pop(); }

    void push(StateID state_id) override { original_->push(state_id); }

    /*void
    /push(StateID parent, QuotientAction qaction, value_t prob, StateID
    state_id) override
    {
        original_->push(parent, qaction.action, prob, state_id);
    }*/

    unsigned size() const override { return original_->size(); }

    bool empty() const override { return original_->empty(); }

    void clear() override { original_->clear(); }
};

} // namespace quotients
} // namespace probfd

#endif // __state_properties.h_H__