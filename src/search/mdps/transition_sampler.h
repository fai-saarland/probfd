#pragma once

#include "../global_state.h"
#include "algorithms/types_heuristic_search.h"
#include "probabilistic_operator.h"

#include <utility>

namespace probabilistic {
namespace algorithms {
template<>
class TransitionSampler<GlobalState, const ProbabilisticOperator*> {
public:
    virtual ~TransitionSampler() = default;
    GlobalState operator()(
        const GlobalState& state,
        const ProbabilisticOperator* op,
        const Distribution<GlobalState>& transition);

    void connect(algorithms::HeuristicSearchStatusInterface<
                 GlobalState,
                 const ProbabilisticOperator*>* hsinterface);

protected:
    virtual GlobalState sample(
        const GlobalState& state,
        const ProbabilisticOperator* op,
        const Distribution<GlobalState>& transition) = 0;

    algorithms::HeuristicSearchStatusInterface<
        GlobalState,
        const ProbabilisticOperator*>* hs_interface_ = nullptr;
};
} // namespace algorithms
using TransitionSampler =
    algorithms::TransitionSampler<GlobalState, const ProbabilisticOperator*>;
} // namespace probabilistic
