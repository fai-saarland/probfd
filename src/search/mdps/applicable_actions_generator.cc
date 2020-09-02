#include "applicable_actions_generator.h"

#include "../global_operator.h"
#include "../global_state.h"
#include "../state_id.h"
#include "../successor_generator.h"
#include "probabilistic_operator.h"
#include "transition_generator.h"

#include <limits>

namespace probabilistic {
namespace algorithms {

void
ApplicableActionsGenerator<GlobalState, const ProbabilisticOperator*>::
    Statistics::print(std::ostream& out) const
{
    out << "Applicable operator generator calls: " << calls << std::endl;
    out << "Applicable operator generator cache hits: " << cache_hits
        << std::endl;
#if defined(EXPENSIVE_STATISTICS)
    out << "Applicable operator generation time: " << time << std::endl;
#endif
}

ApplicableActionsGenerator<GlobalState, const ProbabilisticOperator*>::
    ApplicableActionsGenerator(
        TransitionGenerator<GlobalState, const ProbabilisticOperator*>* t,
        successor_generator::SuccessorGenerator<const ProbabilisticOperator*>*
            gen,
        const std::vector<ProbabilisticOperator>& ops,
        bool cache_aops)
    : cache_aops_(cache_aops)
    , is_fh_(false)
    , fh_var_(-1)
    , successor_generator_(gen)
    , min_cost_()
    , max_costs_()
    , ops_ptr_(ops.empty() ? nullptr : &ops[0])
    , cache_()
    , tgen_(t)
    , statistics_()
{
}

ApplicableActionsGenerator<GlobalState, const ProbabilisticOperator*>::
    ApplicableActionsGenerator(
        TransitionGenerator<GlobalState, const ProbabilisticOperator*>* t,
        successor_generator::SuccessorGenerator<const ProbabilisticOperator*>*
            gen,
        const std::vector<ProbabilisticOperator>& ops,
        int fh_var,
        OperatorCost cost_type,
        bool cache_aops)
    : cache_aops_(cache_aops)
    , is_fh_(fh_var != -1)
    , fh_var_(fh_var)
    , successor_generator_(gen)
    , min_cost_()
    , max_costs_()
    , ops_ptr_(ops.empty() ? nullptr : &ops[0])
    , cache_()
    , tgen_(t)
    , statistics_()
{
    if (is_fh_) {
        min_cost_ = std::numeric_limits<int>::max();
        max_costs_.resize(ops.size());
        for (int i = max_costs_.size() - 1; i >= 0; --i) {
            const auto& prob_operator = ops[i];
            int max_cost = 0;
            for (int j = prob_operator.num_outcomes() - 1; j >= 0; --j) {
                int cost = get_adjusted_action_cost(
                    *prob_operator.get(j).op, cost_type);
                min_cost_ = std::min(min_cost_, cost);
                max_cost = std::max(max_cost, cost);
            }
            max_costs_[i] = max_cost;
        }
    }
}

void
ApplicableActionsGenerator<GlobalState, const ProbabilisticOperator*>::
operator()(
    const GlobalState& state,
    std::vector<const ProbabilisticOperator*>& aops)
{
    statistics_.calls++;
#if defined(EXPENSIVE_STATISTICS)
    statistics_.time.resume();
#endif
    if (cache_aops_ && state.get_id().hash() < cache_.size()) {
        const CacheEntry& cached = cache_[state.get_id().hash()];
        if (!cached.empty()) {
            statistics_.cache_hits++;
            aops.reserve(cached.size());
            for (int i = cached.size() - 1; i >= 0; i--) {
                aops.push_back(ops_ptr_ + cached[i]);
            }
#if defined(EXPENSIVE_STATISTICS)
            statistics_.time.stop();
#endif
            return;
        }
    }
    if (is_fh_) {
        const int budget = state[fh_var_];
        if (budget < min_cost_) {
#if defined(EXPENSIVE_STATISTICS)
            statistics_.time.stop();
#endif
            return;
        }
        successor_generator_->generate_applicable_ops(state, aops);
        unsigned i = 0;
        for (unsigned j = 0; j < aops.size(); ++j) {
            if (max_costs_[aops[j]->get_id()] <= budget) {
                aops[i] = aops[j];
                ++i;
            }
        }
        aops.resize(i);
    } else {
        successor_generator_->generate_applicable_ops(state, aops);
    }
    if (cache_aops_) {
        if (state.get_id().hash() >= cache_.size()) {
            cache_.resize(state.get_id().hash() + 1);
        }
        CacheEntry& cached = cache_[state.get_id().hash()];
        for (int i = aops.size() - 1; i >= 0; --i) {
            cached.push_back(aops[i]->get_id());
        }
    }
    if (tgen_) {
        tgen_->notify_applicable_actions(state, aops);
    }
#if defined(EXPENSIVE_STATISTICS)
    statistics_.time.stop();
#endif
}

} // namespace algorithms
} // namespace probabilistic
