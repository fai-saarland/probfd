#include "probfd/caching_task_state_space.h"

#include "probfd/transition.h"

#include "probfd/distribution.h"
#include "probfd/probabilistic_task.h"
#include "probfd/task_proxy.h"

#include "downward/utils/logging.h"

#include "downward/evaluator.h"
#include "downward/operator_id.h"
#include "downward/task_proxy.h"

#include <cassert>
#include <functional>
#include <ostream>
#include <ranges>
#include <span>
#include <type_traits>

using namespace std::ranges;
using namespace std::views;

class Evaluator;

namespace probfd {

CachingTaskStateSpace::CachingTaskStateSpace(
    std::shared_ptr<ProbabilisticTask> task,
    utils::LogProxy log,
    std::shared_ptr<FDRSimpleCostFunction> cost_function,
    const std::vector<std::shared_ptr<::Evaluator>>& path_dependent_evaluators)
    : TaskStateSpace(
          std::move(task),
          std::move(log),
          std::move(cost_function),
          path_dependent_evaluators)
{
}

void CachingTaskStateSpace::generate_applicable_actions(
    const State& state,
    std::vector<OperatorID>& result)
{
    CacheEntry& entry = lookup(state);
    result.reserve(entry.naops);
    for (size_t i = 0; i != entry.naops; ++i) {
        result.push_back(entry.aops[i]);
    }

    ++statistics_.aops_generator_calls;
    statistics_.generated_operators += result.size();
}

void CachingTaskStateSpace::generate_action_transitions(
    const State& state,
    OperatorID op_id,
    Distribution<StateID>& result)
{
    const ProbabilisticOperatorsProxy operators = task_proxy_.get_operators();

    const CacheEntry& entry = lookup(state);
    assert(entry.is_initialized());
    const StateID* succs = entry.succs;

    auto it = find_if(counted(entry.aops, entry.naops), [&](OperatorID id) {
        if (op_id == id) return true;
        succs += operators[id].get_outcomes().size();
        return false;
    });

    for (const auto outcome : operators[*it].get_outcomes()) {
        result.add_probability(*succs++, outcome.get_probability());
    }

    ++statistics_.single_transition_generator_calls;
}

void CachingTaskStateSpace::generate_all_transitions(
    const State& state,
    std::vector<OperatorID>& aops,
    std::vector<Distribution<StateID>>& successors)
{
    const ProbabilisticOperatorsProxy operators = task_proxy_.get_operators();

    CacheEntry& entry = lookup(state);
    const StateID* succs = entry.succs;
    aops.reserve(entry.naops);
    successors.reserve(entry.naops);

    for (OperatorID op_id : counted(entry.aops, entry.naops)) {
        aops.push_back(op_id);

        Distribution<StateID>& result = successors.emplace_back();

        const ProbabilisticOperatorProxy op = operators[op_id];

        for (const auto outcome : op.get_outcomes()) {
            result.add_probability(*succs++, outcome.get_probability());
        }
    }

    ++statistics_.all_transitions_generator_calls;
    statistics_.generated_operators += aops.size();
}

void CachingTaskStateSpace::generate_all_transitions(
    const State& state,
    std::vector<Transition>& transitions)
{
    const ProbabilisticOperatorsProxy operators = task_proxy_.get_operators();

    CacheEntry& entry = lookup(state);
    const StateID* succs = entry.succs;
    transitions.reserve(entry.naops);

    for (OperatorID op_id : counted(entry.aops, entry.naops)) {
        Transition& t = transitions.emplace_back(op_id);
        Distribution<StateID>& result = t.successor_dist;

        const ProbabilisticOperatorProxy op = operators[op_id];
        const ProbabilisticOutcomesProxy outcomes = op.get_outcomes();
        const size_t num_outcomes = outcomes.size();

        for (const auto outcome : outcomes) {
            result.add_probability(*succs++, outcome.get_probability());
        }

        statistics_.generated_states += num_outcomes;
    }

    ++statistics_.all_transitions_generator_calls;
    statistics_.generated_operators += transitions.size();
}

void CachingTaskStateSpace::print_statistics() const
{
    TaskStateSpace::print_statistics();
    log_ << "  Stored arrays in bytes: " << cache_data_.size_in_bytes()
         << std::endl;
}

void CachingTaskStateSpace::compute_successor_states(
    const State& state,
    OperatorID op_id,
    std::vector<StateID>& succs)
{
    const ProbabilisticOperatorProxy op = task_proxy_.get_operators()[op_id];
    const auto outcomes = op.get_outcomes();
    const size_t num_outcomes = outcomes.size();
    succs.reserve(num_outcomes);

    for (const ProbabilisticOutcomeProxy outcome : outcomes) {
        State succ =
            state_registry_.get_successor_state(state, outcome.get_effects());

        for (const auto& h : notify_) {
            OperatorID det_op_id(outcome.get_determinization_id());
            h->notify_state_transition(state, det_op_id, succ);
        }

        succs.emplace_back(succ.get_id());
    }

    ++statistics_.transition_computations;
    statistics_.computed_successors += num_outcomes;
}

void CachingTaskStateSpace::setup_cache(const State& state, CacheEntry& entry)
{
    if (entry.is_initialized()) return;

    assert(aops_.empty() && successors_.empty());
    compute_applicable_operators(state, aops_);
    entry.naops = aops_.size();

    if (entry.naops > 0) {
        entry.aops = cache_data_.allocate<OperatorID>(aops_.size());

        std::vector<StateID> succs;
        for (size_t i = 0; i < aops_.size(); ++i) {
            OperatorID op = aops_[i];
            entry.aops[i] = op;

            compute_successor_states(state, op, succs);

            for (const StateID s : succs) {
                successors_.push_back(s);
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
}

CachingTaskStateSpace::CacheEntry&
CachingTaskStateSpace::lookup(const State& state)
{
    CacheEntry& entry = cache_[state];
    setup_cache(state, entry);
    return entry;
}

} // namespace probfd
