#include "probfd/transition_generator.h"

#include "probfd/utils/logging.h"

#include "probfd/globals.h"

#include "algorithms/int_packer.h"

#include "legacy/global_operator.h"
#include "legacy/global_state.h"
#include "legacy/globals.h"
#include "legacy/heuristic.h"
#include "legacy/state_registry.h"
#include "legacy/successor_generator.h"

#ifndef NDEBUG
#define DEBUG_CACHE_CONSISTENCY_CHECK
#endif

#include <algorithm>
#include <cstring>
#include <map>

namespace probfd {

static std::shared_ptr<legacy::successor_generator::SuccessorGenerator<
    const ProbabilisticOperator*>>
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

    return std::make_shared<legacy::successor_generator::SuccessorGenerator<
        const ProbabilisticOperator*>>(
        legacy::g_variable_domain,
        preconditions,
        ops);
}

TransitionGeneratorBase::TransitionGeneratorBase(
    legacy::StateRegistry* state_registry,
    const std::vector<ProbabilisticOperator>& ops,
    const std::vector<std::shared_ptr<legacy::Heuristic>>&
        path_dependent_heuristics,
    bool enable_caching)
    : first_op_(!ops.empty() ? &ops[0] : nullptr)
    , caching_(enable_caching)
    , notify_(path_dependent_heuristics)
    , state_registry_(state_registry)
{
}

void TransitionGeneratorBase::operator()(
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
        legacy::GlobalState state =
            this->state_registry_->lookup_state(legacy::StateID(state_id));
        std::vector<const ProbabilisticOperator*> test;
        this->compute_applicable_operators(state, test);
        assert(
            std::equal(test.begin(), test.end(), result.begin(), result.end()));
#endif
    } else {
        legacy::GlobalState state =
            state_registry_->lookup_state(legacy::StateID(state_id));
        compute_applicable_operators(state, result);
    }

    ++statistics_.aops_generator_calls;
    statistics_.generated_operators += result.size();
}

void TransitionGeneratorBase::operator()(
    const StateID& state_id,
    const ProbabilisticOperator* const& action,
    Distribution<StateID>& result)
{
#ifdef DEBUG_CACHE_CONSISTENCY_CHECK
    {
        legacy::GlobalState state =
            this->state_registry_->lookup_state(legacy::StateID(state_id));
        for (const auto [var, val] : action->get_preconditions()) {
            assert(state[var] == val);
        }
    }
#endif

    if (caching_) {
        const CacheEntry& entry = cache_[state_id];
        assert(entry.is_initialized());
        const ActionID id = action - first_op_;
        const StateID* succs = entry.succs;

        for (size_t i = 0; i < entry.naops; ++i) {
            ActionID op_id = entry.aops[i];

            if (op_id != id) {
                succs += first_op_[op_id].num_outcomes();
                continue;
            }

#ifdef DEBUG_CACHE_CONSISTENCY_CHECK
            legacy::GlobalState state =
                this->state_registry_->lookup_state(legacy::StateID(state_id));
            std::vector<WeightedElement<StateID>> test;
            this->compute_successor_states(state, action, test);
            assert(test.size() == action->num_outcomes());
#endif
            for (unsigned j = 0; j < action->num_outcomes(); ++j, ++succs) {
                assert(test[j] == WeightedElement(*succs, action->get(j).prob));
                result.add(*succs, action->get(j).prob);
            }

            break;
        }
    } else {
        legacy::GlobalState state =
            state_registry_->lookup_state(legacy::StateID(state_id));
        std::vector<WeightedElement<StateID>> temp;
        this->compute_successor_states(state, action, temp);
        result = Distribution<StateID>(std::move(temp));
    }

    ++statistics_.single_transition_generator_calls;
    statistics_.generated_states += result.size();
}

void TransitionGeneratorBase::operator()(
    const StateID& state_id,
    std::vector<const ProbabilisticOperator*>& aops,
    std::vector<Distribution<StateID>>& successors)
{
    if (caching_) {
        CacheEntry& entry = lookup(state_id);
        const StateID* succs = entry.succs;
        aops.resize(entry.naops, nullptr);
        successors.resize(entry.naops);

#ifdef DEBUG_CACHE_CONSISTENCY_CHECK
        legacy::GlobalState state =
            this->state_registry_->lookup_state(legacy::StateID(state_id));
        std::vector<const ProbabilisticOperator*> test_aops;
        this->compute_applicable_operators(state, test_aops);
        assert(test_aops.size() == entry.naops);
#endif

        for (unsigned i = 0; i < entry.naops; ++i) {
            const ProbabilisticOperator* op = first_op_ + entry.aops[i];
            aops[i] = op;

#ifdef DEBUG_CACHE_CONSISTENCY_CHECK
            assert(aops[i] == test_aops[i]);
            std::vector<WeightedElement<StateID>> test;
            this->compute_successor_states(state, aops[i], test);
#endif

            Distribution<StateID>& result = successors[i];
            for (unsigned j = 0; j < op->num_outcomes(); ++j, ++succs) {
                assert(test[j] == WeightedElement(*succs, op->get(j).prob));
                result.add(*succs, op->get(j).prob);
            }

            statistics_.generated_states += result.size();
        }
    } else {
        legacy::GlobalState state =
            state_registry_->lookup_state(legacy::StateID(state_id));
        compute_applicable_operators(state, aops);
        successors.reserve(aops.size());

        std::vector<WeightedElement<StateID>> temp;
        for (const auto& op : aops) {
            this->compute_successor_states(state, op, temp);
            const auto& new_dist = successors.emplace_back(std::move(temp));
            statistics_.generated_states += new_dist.size();
            temp.clear();
        }
    }

    ++statistics_.all_transitions_generator_calls;
    statistics_.generated_operators += aops.size();
}

bool TransitionGeneratorBase::setup_cache(
    const StateID& state_id,
    CacheEntry& entry)
{
    if (!entry.is_initialized()) {
        legacy::GlobalState state =
            state_registry_->lookup_state(legacy::StateID(state_id));
        assert(aops_.empty() && successors_.empty());
        compute_applicable_operators(state, aops_);
        entry.naops = aops_.size();

        if (entry.naops > 0) {
            entry.aops = cache_data_.allocate<ActionID>(aops_.size());

            std::vector<WeightedElement<StateID>> succs;
            for (size_t i = 0; i < aops_.size(); ++i) {
                const ProbabilisticOperator* op = aops_[i];

                entry.aops[i] = op - first_op_;
                this->compute_successor_states(state, op, succs);

                for (const auto& s : succs) {
                    successors_.push_back(s.element);
                }

                succs.clear();
            }

            entry.succs = cache_data_.allocate<StateID>(successors_.size());

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

TransitionGeneratorBase::CacheEntry&
TransitionGeneratorBase::lookup(const StateID& sid)
{
    CacheEntry& entry = cache_[sid];
    setup_cache(sid, entry);
    return entry;
}

TransitionGeneratorBase::CacheEntry&
TransitionGeneratorBase::lookup(const StateID& sid, bool& setup)
{
    CacheEntry& entry = cache_[sid];
    setup = setup_cache(sid, entry);
    return entry;
}

void TransitionGeneratorBase::print_statistics(std::ostream& out) const
{
    statistics_.print(out);
    out << "  Stored arrays in bytes: " << cache_data_.size_in_bytes()
        << std::endl;
}

void TransitionGeneratorBase::Statistics::print(std::ostream& out) const
{
    out << logging::whitespace(2)
        << "Applicable operators: " << generated_operators << " generated, "
        << computed_operators << " computed, " << aops_generator_calls
        << " generator calls." << std::endl;
    out << logging::whitespace(2) << "Generated " << generated_states
        << " successor state(s): " << computed_successors << " computed, "
        << single_transition_generator_calls << " single-transition calls, "
        << all_transitions_generator_calls << " all-transitions calls."
        << std::endl;
}

BudgetTransitionGenerator::BudgetTransitionGenerator(
    legacy::StateRegistry* state_registry,
    const std::vector<ProbabilisticOperator>& ops,
    const std::vector<std::shared_ptr<legacy::Heuristic>>&
        path_dependent_heuristics,
    bool enable_caching,
    int budget_var,
    legacy::OperatorCost budget_cost_type)
    : TransitionGeneratorBase(
          state_registry,
          ops,
          path_dependent_heuristics,
          enable_caching)
    , budget_var_(budget_var)
    , budget_cost_type_(budget_cost_type)
{
    assert(budget_var_ >= 0);

    std::map<int, std::vector<const ProbabilisticOperator*>, std::greater<>>
        grouped;
    for (const auto& op : ops) {
        const int reward =
            get_adjusted_action_reward(op.get_reward(), budget_cost_type_);
        grouped[reward].push_back(&op);
    }

    reward_.reserve(grouped.size());
    gens_.reserve(grouped.size());

    for (const auto& [i, gops] : grouped) {
        reward_.push_back(i);
        gens_.push_back(construct_generator(gops));
    }
}

void BudgetTransitionGenerator::compute_successor_states(
    const legacy::GlobalState& state,
    const ProbabilisticOperator* op,
    std::vector<WeightedElement<StateID>>& succs)
{
    succs.reserve(op->num_outcomes());

    assert(budget_var_ >= 0);

    const int newb = state[budget_var_] + reward_[op->get_id()];
    assert(newb >= 0);

    for (const auto [det_op, prob] : *op) {
        legacy::GlobalState succ = state_registry_->get_successor_state(
            state,
            *det_op,
            budget_var_,
            newb);

        for (const auto& h : notify_) {
            h->reach_state(state, *det_op, succ);
        }

        succs.emplace_back(succ.get_id().hash(), prob);
    }

    ++statistics_.transition_computations;
    statistics_.computed_successors += succs.size();
}

void BudgetTransitionGenerator::compute_applicable_operators(
    const legacy::GlobalState& s,
    std::vector<const ProbabilisticOperator*>& ops)
{
    assert(budget_var_ >= 0);

    const int budget = s[budget_var_];
    for (std::size_t i = 0; i < reward_.size(); ++i) {
        if (0 <= budget + reward_[i]) {
            gens_[i]->generate_applicable_ops(s, ops);
        }
    }

    ++statistics_.aops_computations;
    statistics_.computed_operators += ops.size();
}

DefaultTransitionGenerator::DefaultTransitionGenerator(
    legacy::StateRegistry* state_registry,
    const std::vector<ProbabilisticOperator>& ops,
    const std::vector<std::shared_ptr<legacy::Heuristic>>&
        path_dependent_heuristics,
    bool enable_caching,
    legacy::successor_generator::SuccessorGenerator<
        const ProbabilisticOperator*>* gen)
    : TransitionGeneratorBase(
          state_registry,
          ops,
          path_dependent_heuristics,
          enable_caching)
    , gen_(gen)
{
}

void DefaultTransitionGenerator::compute_successor_states(
    const legacy::GlobalState& state,
    const ProbabilisticOperator* op,
    std::vector<WeightedElement<StateID>>& succs)
{
    succs.reserve(op->num_outcomes());

    for (const auto [det_op, prob] : *op) {
        legacy::GlobalState succ =
            state_registry_->get_successor_state(state, *det_op);

        for (const auto& h : notify_) {
            h->reach_state(state, *det_op, succ);
        }

        succs.emplace_back(succ.get_id().hash(), prob);
    }

    ++statistics_.transition_computations;
    statistics_.computed_successors += succs.size();
}

void DefaultTransitionGenerator::compute_applicable_operators(
    const legacy::GlobalState& s,
    std::vector<const ProbabilisticOperator*>& ops)
{
    gen_->generate_applicable_ops(s, ops);

    ++statistics_.aops_computations;
    statistics_.computed_operators += ops.size();
}

namespace engine_interfaces {
TransitionGenerator<const ProbabilisticOperator*>::TransitionGenerator(
    legacy::StateRegistry* state_registry,
    const std::vector<ProbabilisticOperator>& ops,
    const std::vector<std::shared_ptr<legacy::Heuristic>>&
        path_dependent_heuristics,
    bool enable_caching)
    : base(
          g_step_var != -1 ? static_cast<TransitionGeneratorBase*>(
                                 new BudgetTransitionGenerator(
                                     state_registry,
                                     ops,
                                     path_dependent_heuristics,
                                     enable_caching,
                                     g_step_var,
                                     g_step_cost_type))
                           : new DefaultTransitionGenerator(
                                 state_registry,
                                 ops,
                                 path_dependent_heuristics,
                                 enable_caching,
                                 g_successor_generator.get()))
{
}

} // namespace engine_interfaces

} // namespace probfd

#undef DEBUG_CACHE_CONSISTENCY_CHECK
