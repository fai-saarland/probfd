#pragma once

#include "../global_state.h"
#include "algorithms/quotient_system.h"
#include "algorithms/types_heuristic_search.h"
#include "interfaces/i_engine.h"
#include "probabilistic_operator.h"

#include <memory>
#include <utility>
#include <vector>

namespace probabilistic {

using QuotientState = algorithms::quotient_system::
    QuotientState<GlobalState, const ProbabilisticOperator*>;

using QuotientAction = algorithms::quotient_system::
    QuotientAction<GlobalState, const ProbabilisticOperator*>;

namespace algorithms {
template<>
class PolicyChooser<GlobalState, const ProbabilisticOperator*> {
public:
    virtual ~PolicyChooser() = default;
    unsigned operator()(
        const bool unseen,
        const ProbabilisticOperator* const* prev,
        const GlobalState& state,
        const std::vector<
            std::pair<const ProbabilisticOperator*, Distribution<GlobalState>>>&
            greedy_optimal);

    virtual void connect(HeuristicSearchStatusInterface<
                         GlobalState,
                         const ProbabilisticOperator*>* engine) = 0;

protected:
    virtual unsigned choose(
        const GlobalState& state,
        const ProbabilisticOperator* prev,
        const std::vector<
            std::pair<const ProbabilisticOperator*, Distribution<GlobalState>>>&
            greedy_optimal);

    virtual unsigned choose(
        const GlobalState& state,
        const std::vector<
            std::pair<const ProbabilisticOperator*, Distribution<GlobalState>>>&
            greedy_optimal);
};

template<>
class PolicyChooser<QuotientState, QuotientAction> {
public:
    virtual ~PolicyChooser() = default;
    unsigned operator()(
        const bool unseen,
        const QuotientAction* prev,
        const QuotientState& state,
        const std::vector<
            std::pair<QuotientAction, Distribution<QuotientState>>>&
            greedy_optimal);

    virtual void
    connect(HeuristicSearchStatusInterface<QuotientState, QuotientAction>*
                engine) = 0;

protected:
    virtual unsigned choose(
        const QuotientState& state,
        const QuotientAction& act,
        const std::vector<
            std::pair<QuotientAction, Distribution<QuotientState>>>&
            greedy_optimal);

    virtual unsigned choose(
        const QuotientState& state,
        const std::vector<
            std::pair<QuotientAction, Distribution<QuotientState>>>&
            greedy_optimal);
};
} // namespace algorithms

class PolicyChooser
    : public algorithms::
          PolicyChooser<GlobalState, const ProbabilisticOperator*>,
      public algorithms::PolicyChooser<QuotientState, QuotientAction> {
public:
    virtual ~PolicyChooser() = default;

    virtual void connect(algorithms::HeuristicSearchStatusInterface<
                         GlobalState,
                         const ProbabilisticOperator*>*) override;

    virtual void connect(algorithms::HeuristicSearchStatusInterface<
                         QuotientState,
                         QuotientAction>* e) override;

protected:
    algorithms::HeuristicSearchStatusInterface<
        GlobalState,
        const ProbabilisticOperator*>* hs_interface_ = nullptr;
    algorithms::HeuristicSearchStatusInterface<QuotientState, QuotientAction>*
        qhs_interface_ = nullptr;
};

} // namespace probabilistic
