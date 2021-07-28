#include "transition_generator.h"

#include "../algorithms/int_packer.h"
#include "../global_operator.h"
#include "../global_state.h"
#include "../globals.h"
#include "../heuristic.h"
#include "../state_registry.h"
#include "../successor_generator.h"
#include "globals.h"
#include "logging.h"

#ifndef NDEBUG
#define DEBUG_CACHE_CONSISTENCY_CHECK
#endif

#include <algorithm>
#include <map>

namespace probabilistic {

static std::shared_ptr<
    successor_generator::SuccessorGenerator<const ProbabilisticOperator*>>
construct_generator(const std::vector<const ProbabilisticOperator*>& ops)
{
    std::vector<std::vector<std::pair<int, int>>> preconditions(ops.size());
    for (int i = ops.size() - 1; i >= 0; --i) {
        std::vector<std::pair<int, int>>& pre = preconditions[i];
        for (const auto& p : ops[i]->get_preconditions()) {
            pre.emplace_back(p.var, p.val);
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
        std::map<int, std::vector<const ProbabilisticOperator*>> grouped;
        for (unsigned i = 0; i < ops.size(); ++i) {
            const int cost =
                get_adjusted_action_cost(*ops[i].get(0).op, cost_type);
            grouped[cost].push_back(&ops[i]);
        }
        cost_.reserve(grouped.size());
        gens_.reserve(grouped.size());
        for (auto it = grouped.begin(); it != grouped.end(); ++it) {
            cost_.push_back(it->first);
            gens_.push_back(construct_generator(it->second));
        }
    } else {
    }
}

void CostBasedSuccessorGenerator::operator()(
    const GlobalState& s,
    std::vector<const ProbabilisticOperator*>& res) const
{
    if (bvar_ >= 0) {
        const int budget = s[bvar_];
        for (unsigned i = 0; i < cost_.size(); ++i) {
            if (cost_[i] > budget) {
                break;
            }
            gens_[i]->generate_applicable_ops(s, res);
        }
    } else {
        gen_->generate_applicable_ops(s, res);
    }
}

ApplicableActionsGenerator<const ProbabilisticOperator*>::
    ApplicableActionsGenerator(
        TransitionGenerator<const ProbabilisticOperator*>* tgen)
    : tgen_(tgen)
{
}

void ApplicableActionsGenerator<const ProbabilisticOperator*>::operator()(
    const StateID& state_id,
    std::vector<const ProbabilisticOperator*>& ops)
{
    tgen_->generate_applicable_ops(state_id, ops);
}

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
    // , aops_gen_(gen)
    , state_registry_(state_registry)
{
    if (budget_var >= 0) {
        cost_.resize(ops.size(), 0);
        for (int i = ops.size() - 1; i >= 0; --i) {
            cost_[i] =
                ::get_adjusted_action_cost(*ops[i].get(0).op, budget_cost_type);
        }
    }
}

void TransitionGenerator<const ProbabilisticOperator*>::generate_applicable_ops(
    const StateID& state_id,
    std::vector<const ProbabilisticOperator*>& result)
{
    if (caching_) {
        CacheEntry& entry = cache_[state_id];
        setup_cache(state_id, entry);
        result.resize(entry.naops, nullptr);
        for (int i = entry.naops - 1; i >= 0; --i) {
            result[i] = (first_op_ + entry.aops[i]);
        }
#ifdef DEBUG_CACHE_CONSISTENCY_CHECK
        GlobalState state =
            this->state_registry_->lookup_state(::StateID(state_id));
        std::vector<const ProbabilisticOperator*> test;
        this->compute_applicable_operators(state, test);
        assert(test.size() == result.size());
        for (unsigned i = 0; i < test.size(); ++i) {
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
        for (const auto& pre : action->get_preconditions()) {
            assert(state[pre.var] == pre.val);
        }
    }
#endif
    if (caching_) {
        const CacheEntry& entry = cache_[state_id];
        assert(entry.is_initialized());
        const uint32_t idx = (action - first_op_);
        const uint32_t* succs = entry.succs;
        for (unsigned i = 0; i < entry.naops; ++i) {
            if (entry.aops[i] == idx) {
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
            succs += (first_op_ + entry.aops[i])->num_outcomes();
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
        CacheEntry& entry = cache_[state_id];
        setup_cache(state_id, entry);
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
            const ProbabilisticOperator* op = (first_op_ + entry.aops[i]);
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
        for (unsigned i = 0; i < aops.size(); ++i) {
            compute_successor_states(state, aops[i], temp);
            successors.emplace_back(std::move(temp));
            temp.clear();
            statistics_.generated_states += successors.back().size();
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
        // const int_packer::IntPacker* packer =
        //     state_registry_->get_state_packer();
        const int newb = state[budget_var_] - cost_[op->get_id()];
        assert(newb >= 0);
        for (unsigned i = 0; i < op->num_outcomes(); ++i) {
            const GlobalOperator& det_op = *(op->get(i).op);
#if 0
            PackedStateBin* succ_data =
                state_registry_->get_temporary_successor_state(state, det_op);
            packer->set(succ_data, budget_var_, newb);
            GlobalState succ = state_registry_->make_permanent();
#else
            GlobalState succ = state_registry_->get_successor_state(
                state,
                det_op,
                budget_var_,
                newb);
#endif
            for (int j = notify_.size() - 1; j >= 0; j--) {
                notify_[j]->reach_state(state, det_op, succ);
            }
            succs.emplace_back(succ.get_id().hash(), op->get(i).prob);
        }
    } else {
        for (unsigned i = 0; i < op->num_outcomes(); ++i) {
            const auto& out = op->get(i);
            const GlobalOperator& det_op = *(out.op);
            GlobalState succ =
                state_registry_->get_successor_state(state, det_op);
            for (int j = notify_.size() - 1; j >= 0; j--) {
                notify_[j]->reach_state(state, det_op, succ);
            }
            succs.emplace_back(succ.get_id().hash(), out.prob);
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
            if (cost_[ops[i]->get_id()] <= remaining) {
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
            entry.aops = cache_data_.allocate(
                aops_.size()); // new uint32_t[aops_.size()];
            // entry.offset = new uint32_t[result.size()];
            std::vector<std::pair<StateID, value_type::value_t>> succs;
            for (unsigned i = 0; i < aops_.size(); ++i) {
                // entry.offset[i] = successors_.size();
                const ProbabilisticOperator* op = aops_[i];
                entry.aops[i] = (op - first_op_);
                compute_successor_states(state, op, succs);
                for (unsigned j = 0; j < succs.size(); ++j) {
                    successors_.push_back(succs[j].first);
                }
                succs.clear();
            }
            entry.succs = cache_data_.allocate(
                successors_.size()); // new uint32_t[successors_.size()];
            for (int i = successors_.size() - 1; i >= 0; --i) {
                entry.succs[i] = successors_[i];
            }
            aops_.clear();
            successors_.clear();
        }
        // std::cout << "generated cache for " << state_id << std::endl;
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

} // namespace probabilistic

#undef DEBUG_CACHE_CONSISTENCY_CHECK
