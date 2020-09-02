#pragma once

#include "../global_state.h"
#include "algorithms/types_common.h"
#include "probabilistic_operator.h"

#include <utility>

namespace probabilistic {
namespace algorithms {

template<>
class TransitionRewardFunction<GlobalState, const ProbabilisticOperator*> {
public:
    virtual ~TransitionRewardFunction() = default;
    value_type::value_t operator()(
        const GlobalState& state,
        const ProbabilisticOperator* const& op);

protected:
    /*virtual*/ value_type::value_t
    evaluate(const GlobalState& state, const ProbabilisticOperator* op);
};

// class TransitionEvaluator
//     : public ITransitionEvaluator<GlobalState, const ProbabilisticOperator*>,
//       public ITransitionEvaluator<
//           GlobalState,
//           std::pair<GlobalState, const ProbabilisticOperator*>> {
// public:
//     virtual ~TransitionEvaluator() = default;
//     virtual value_type::value_t operator()(
//         const GlobalState& s,
//         const ProbabilisticOperator* const& op) = 0;
//     virtual value_type::value_t operator()(
//         const GlobalState& s,
//         const std::pair<GlobalState, const ProbabilisticOperator*>& op);
// };

} // namespace algorithms

using TransitionEvaluator = algorithms::
    TransitionRewardFunction<GlobalState, const ProbabilisticOperator*>;

} // namespace probabilistic
