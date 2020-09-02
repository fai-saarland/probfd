#pragma once

#include "../operator_cost.h"
#include "../utils/timer.h"
#include "algorithms/types_common.h"
#include "interfaces/i_printable.h"

#include <iostream>
#include <vector>

class GlobalState;

namespace successor_generator {
template<typename O>
class SuccessorGenerator;
}

namespace probabilistic {

class ProbabilisticOperator;

namespace algorithms {

template<>
class ApplicableActionsGenerator<GlobalState, const ProbabilisticOperator*> {
public:
    struct Statistics : public IPrintable {
        Statistics()
            : calls(0)
            , cache_hits(0)
        {
#if defined(EXPENSIVE_STATISTICS)
            time.stop();
#endif
        }

        virtual void print(std::ostream& out) const override;

        unsigned long long calls;
        unsigned long long cache_hits;
#if defined(EXPENSIVE_STATISTICS)
        utils::Timer time;
#endif
    };

    ApplicableActionsGenerator(
        TransitionGenerator<GlobalState, const ProbabilisticOperator*>* t,
        successor_generator::SuccessorGenerator<const ProbabilisticOperator*>*
            gen,
        const std::vector<ProbabilisticOperator>& ops,
        bool cache_aops);

    ApplicableActionsGenerator(
        TransitionGenerator<GlobalState, const ProbabilisticOperator*>* t,
        successor_generator::SuccessorGenerator<const ProbabilisticOperator*>*
            gen,
        const std::vector<ProbabilisticOperator>& ops,
        int hvar,
        OperatorCost cost_type,
        bool cache_aops);

    ~ApplicableActionsGenerator() = default;

    void operator()(
        const GlobalState& state,
        std::vector<const ProbabilisticOperator*>& aops);

    const IPrintable& get_statistics() const { return statistics_; }

private:
    const bool cache_aops_;
    const bool is_fh_;
    const int fh_var_;

    successor_generator::SuccessorGenerator<const ProbabilisticOperator*>*
        successor_generator_;

    int min_cost_;
    std::vector<int> max_costs_;

    const ProbabilisticOperator* ops_ptr_;
    using CacheEntry = std::vector<uint32_t>;
    std::vector<CacheEntry> cache_;

    TransitionGenerator<GlobalState, const ProbabilisticOperator*>* tgen_;

    Statistics statistics_;
};

} // namespace algorithms

using ApplicableActionsGenerator = algorithms::
    ApplicableActionsGenerator<GlobalState, const ProbabilisticOperator*>;

} // namespace probabilistic
