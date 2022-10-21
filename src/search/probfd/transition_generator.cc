#include "probfd/transition_generator.h"

#include "probfd/utils/logging.h"

#include "probfd/globals.h"

#include "algorithms/int_packer.h"

#include "global_operator.h"
#include "global_state.h"
#include "globals.h"
#include "heuristic.h"
#include "state_registry.h"
#include "successor_generator.h"

#ifndef NDEBUG
#define DEBUG_CACHE_CONSISTENCY_CHECK
#endif

#include <algorithm>
#include <cstring>
#include <map>

namespace probfd {

static std::shared_ptr<
    successor_generator::SuccessorGenerator<const ProbabilisticOperator*>>
construct_generator(const std::vector<const ProbabilisticOperator*>& ops)
{
    std::vector<std::vector<std::pair<int, int>>> preconditions(ops.size());
    for (size_t i = 0; i != ops.size(); ++i) {
        std::vector<std::pair<int, int>>& pre = preconditions[i];
        for (const auto [var, val] : ops[i]->get_preconditions()) {
            pre.emplace_back(var, val);
        }
        std::sort(pre.begin(), pre.end());
    }

    return std::make_shared<
        successor_generator::SuccessorGenerator<const ProbabilisticOperator*>>(
        g_variable_domain,
        preconditions,
        ops);
}

CostBasedSuccessorGenerator::CostBasedSuccessorGenerator(
    const std::vector<ProbabilisticOperator>& ops,
    OperatorCost cost_type,
    int bvar,
    successor_generator::SuccessorGenerator<const ProbabilisticOperator*>* gen)
    : bvar_(bvar)
    , gen_(gen)
{
    if (bvar >= 0) {
        std::map<int, std::vector<const ProbabilisticOperator*>, std::greater<>>
            grouped;
        for (const auto& op : ops) {
            const int reward =
                get_adjusted_action_reward(op.get_reward(), cost_type);
            grouped[reward].push_back(&op);
        }

        reward_.reserve(grouped.size());
        gens_.reserve(grouped.size());

        for (const auto& [i, gops] : grouped) {
            reward_.push_back(i);
            gens_.push_back(construct_generator(gops));
        }
    }
}

void CostBasedSuccessorGenerator::operator()(
    const GlobalState& s,
    std::vector<const ProbabilisticOperator*>& res) const
{
    if (bvar_ >= 0) {
        const int budget = s[bvar_];
        for (std::size_t i = 0; i < reward_.size(); ++i) {
            if (0 <= budget + reward_[i]) {
                gens_[i]->generate_applicable_ops(s, res);
            }
        }
    } else {
        gen_->generate_applicable_ops(s, res);
    }
}

namespace engine_interfaces {

TransitionGenerator<const ProbabilisticOperator*>::TransitionGenerator(
    StateRegistry* state_registry,
    bool enable_caching,
    const std::vector<std::shared_ptr<Heuristic>>& path_dependent_heuristics)
    : TransitionGenerator(
          g_operators,
          g_successor_generator.get(),
          g_step_var,
          g_step_cost_type,
          state_registry,
          enable_caching,
          path_dependent_heuristics)
{
}

TransitionGenerator<const ProbabilisticOperator*>::TransitionGenerator(
    const std::vector<ProbabilisticOperator>& ops,
    successor_generator::SuccessorGenerator<const ProbabilisticOperator*>* gen,
    int budget_var,
    OperatorCost budget_cost_type,
    StateRegistry* state_registry,
    bool enable_caching,
    const std::vector<std::shared_ptr<Heuristic>>& path_dependent_heuristics)
    : first_op_(!ops.empty() ? &ops[0] : nullptr)
    , caching_(enable_caching)
    , budget_var_(budget_var)
    , budget_cost_type_(budget_cost_type)
    , notify_(path_dependent_heuristics)
    , aops_gen_(ops, budget_cost_type, budget_var, gen)
    , state_registry_(state_registry)
{
    if (budget_var >= 0) {
        reward_.reserve(ops.size());
        for (const ProbabilisticOperator& op : ops) {
            reward_.push_back(
                get_adjusted_action_reward(op.get_reward(), budget_cost_type));
        }
    }
}

void TransitionGenerator<const ProbabilisticOperator*>::operator()(
    const StateID& state_id,
    std::vector<const ProbabilisticOperator*>& result)
{
    if (caching_) {
        CacheEntry& entry = lookup(state_id);
        result.resize(entry.naops, nullptr);
        for (size_t i = 0; i != entry.naops; ++i) {
            result[i] = first_op_ + entry.aops[i];
        }

#ifdef DEBUG_CACHE_CONSISTENCY_CHECK
        GlobalState state =
            this->state_registry_->lookup_state(::StateID(state_id));
        std::vector<const ProbabilisticOperator*> test;
        this->compute_applicable_operators(state, test);
        assert(test.size() == result.size());
        for (size_t i = 0; i < test.size(); ++i) {
            assert(test[i] == result[i]);
        }
#endif
    } else {
        GlobalState state = state_registry_->lookup_state(::StateID(state_id));
        compute_applicable_operators(state, result);
    }

    ++statistics_.aops_generator_calls;
    statistics_.generated_operators += result.size();
}

void TransitionGenerator<const ProbabilisticOperator*>::operator()(
    const StateID& state_id,
    const ProbabilisticOperator* const& action,
    Distribution<StateID>& result)
{
#ifdef DEBUG_CACHE_CONSISTENCY_CHECK
    {
        GlobalState state =
            this->state_registry_->lookup_state(::StateID(state_id));
        for (const auto [var, val] : action->get_preconditions()) {
            assert(state[var] == val);
        }
    }
#endif

    if (caching_) {
        const CacheEntry& entry = cache_[state_id];
        assert(entry.is_initialized());
        const uint32_t idx = action - first_op_;
        const uint32_t* succs = entry.succs;

        for (size_t i = 0; i < entry.naops; ++i) {
            uint32_t op_idx = entry.aops[i];

            if (op_idx == idx) {
#ifdef DEBUG_CACHE_CONSISTENCY_CHECK
                GlobalState state =
                    this->state_registry_->lookup_state(::StateID(state_id));
                std::vector<std::pair<StateID, value_type::value_t>> test;
                this->compute_successor_states(state, action, test);
                assert(test.size() == action->num_outcomes());
#endif
                for (unsigned j = 0; j < action->num_outcomes(); ++j, ++succs) {
                    assert(test[j].first == *succs);
                    assert(test[j].second == action->get(j).prob);
                    result.add(*succs, action->get(j).prob);
                }

                break;
            }

            const ProbabilisticOperator* op = first_op_ + op_idx;
            succs += op->num_outcomes();
        }
    } else {
        GlobalState state = state_registry_->lookup_state(::StateID(state_id));
        std::vector<std::pair<StateID, value_type::value_t>> temp;
        compute_successor_states(state, action, temp);
        result = Distribution<StateID>(std::move(temp));
    }

    ++statistics_.single_transition_generator_calls;
    statistics_.generated_states += result.size();
}

void TransitionGenerator<const ProbabilisticOperator*>::operator()(
    const StateID& state_id,
    std::vector<const ProbabilisticOperator*>& aops,
    std::vector<Distribution<StateID>>& successors)
{
    if (caching_) {
        CacheEntry& entry = lookup(state_id);
        const uint32_t* succs = entry.succs;
        aops.resize(entry.naops, nullptr);
        successors.resize(entry.naops);

#ifdef DEBUG_CACHE_CONSISTENCY_CHECK
        GlobalState state =
            this->state_registry_->lookup_state(::StateID(state_id));
        std::vector<const ProbabilisticOperator*> test_aops;
        this->compute_applicable_operators(state, test_aops);
        assert(test_aops.size() == entry.naops);
#endif

        for (unsigned i = 0; i < entry.naops; ++i) {
            const ProbabilisticOperator* op = first_op_ + entry.aops[i];
            aops[i] = op;

#ifdef DEBUG_CACHE_CONSISTENCY_CHECK
            assert(aops[i] == test_aops[i]);
            std::vector<std::pair<StateID, value_type::value_t>> test;
            this->compute_successor_states(state, aops[i], test);
#endif

            Distribution<StateID>& result = successors[i];
            for (unsigned j = 0; j < op->num_outcomes(); ++j, ++succs) {
                assert(test[j].first == *succs);
                assert(test[j].second == op->get(j).prob);
                result.add(*succs, op->get(j).prob);
            }

            statistics_.generated_states += result.size();
        }
    } else {
        GlobalState state = state_registry_->lookup_state(::StateID(state_id));
        compute_applicable_operators(state, aops);
        successors.reserve(aops.size());

        std::vector<std::pair<StateID, value_type::value_t>> temp;
        for (const auto& op : aops) {
            compute_successor_states(state, op, temp);
            const auto& new_dist = successors.emplace_back(std::move(temp));
            statistics_.generated_states += new_dist.size();
            temp.clear();
        }
    }

    ++statistics_.all_transitions_generator_calls;
    statistics_.generated_operators += aops.size();
}

void TransitionGenerator<const ProbabilisticOperator*>::
    compute_successor_states(
        const GlobalState& state,
        const ProbabilisticOperator* op,
        std::vector<std::pair<StateID, value_type::value_t>>& succs)
{
    succs.reserve(op->num_outcomes());

    if (budget_var_ >= 0) {
        const int newb = state[budget_var_] + reward_[op->get_id()];
        assert(newb >= 0);

        for (const auto [det_op, prob] : *op) {
            GlobalState succ = state_registry_->get_successor_state(
                state,
                *det_op,
                budget_var_,
                newb);

            for (const auto& h : notify_) {
                h->reach_state(state, *det_op, succ);
            }

            succs.emplace_back(succ.get_id().hash(), prob);
        }
    } else {
        for (const auto [det_op, prob] : *op) {
            GlobalState succ =
                state_registry_->get_successor_state(state, *det_op);

            for (const auto& h : notify_) {
                h->reach_state(state, *det_op, succ);
            }

            succs.emplace_back(succ.get_id().hash(), prob);
        }
    }

    ++statistics_.transition_computations;
    statistics_.computed_states += succs.size();
}

void TransitionGenerator<const ProbabilisticOperator*>::
    compute_applicable_operators(
        const GlobalState& s,
        std::vector<const ProbabilisticOperator*>& ops)
{
#if 0
    aops_gen_->generate_applicable_ops(s, ops);
    if (budget_var_ >= 0) {
        const int remaining = s[budget_var_];
        unsigned j = 0;
        for (unsigned i = 0; i < ops.size(); ++i) {
            if (reward_[ops[i]->get_id()] + remaining >= 0) {
                ops[j++] = ops[i];
            }
        }
        ops.resize(j);
    }
#else
    aops_gen_(s, ops);
#endif

    ++statistics_.aops_computations;
    statistics_.computed_operators += ops.size();
}

bool TransitionGenerator<const ProbabilisticOperator*>::setup_cache(
    const StateID& state_id,
    CacheEntry& entry)
{
    if (!entry.is_initialized()) {
        GlobalState state = state_registry_->lookup_state(::StateID(state_id));
        assert(aops_.empty() && successors_.empty());
        compute_applicable_operators(state, aops_);
        entry.naops = aops_.size();

        if (entry.naops > 0) {
            entry.aops = cache_data_.allocate(aops_.size());

            std::vector<std::pair<StateID, value_type::value_t>> succs;
            for (size_t i = 0; i < aops_.size(); ++i) {
                const ProbabilisticOperator* op = aops_[i];

                entry.aops[i] = op - first_op_;
                compute_successor_states(state, op, succs);

                for (const auto& s : succs) {
                    successors_.push_back(s.first);
                }

                succs.clear();
            }

            entry.succs = cache_data_.allocate(successors_.size());

            for (size_t i = 0; i != successors_.size(); ++i) {
                entry.succs[i] = successors_[i];
            }

            aops_.clear();
            successors_.clear();
        }

        return true;
    }

    return false;
}

TransitionGenerator<const ProbabilisticOperator*>::CacheEntry&
TransitionGenerator<const ProbabilisticOperator*>::lookup(const StateID& sid)
{
    CacheEntry& entry = cache_[sid];
    setup_cache(sid, entry);
    return entry;
}

TransitionGenerator<const ProbabilisticOperator*>::CacheEntry&
TransitionGenerator<const ProbabilisticOperator*>::lookup(
    const StateID& sid,
    bool& setup)
{
    CacheEntry& entry = cache_[sid];
    setup = setup_cache(sid, entry);
    return entry;
}

void TransitionGenerator<const ProbabilisticOperator*>::print_statistics(
    std::ostream& out) const
{
    statistics_.print(out);
    out << "  Stored arrays in bytes: " << cache_data_.size_in_bytes()
        << std::endl;
}

void TransitionGenerator<const ProbabilisticOperator*>::Statistics::print(
    std::ostream& out) const
{
    out << logging::whitespace(2)
        << "Applicable operators: " << generated_operators << " generated, "
        << computed_operators << " computed, " << aops_generator_calls
        << " generator calls." << std::endl;
    out << logging::whitespace(2) << "Generated " << generated_states
        << " state(s): " << computed_states << " computed, "
        << single_transition_generator_calls << " single-transition calls, "
        << all_transitions_generator_calls << " all-transitions calls."
        << std::endl;
}

} // namespace engine_interfaces
} // namespace probfd

#undef DEBUG_CACHE_CONSISTENCY_CHECK