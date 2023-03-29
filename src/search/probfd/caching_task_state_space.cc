#include "probfd/caching_task_state_space.h"

#include "algorithms/int_packer.h"

#include "evaluator.h"

#include "tasks/root_task.h"

#ifndef NDEBUG
#define DEBUG_CACHE_CONSISTENCY_CHECK
#endif

#include <algorithm>
#include <cstring>
#include <map>

namespace probfd {

CachingTaskStateSpace::CachingTaskStateSpace(
    std::shared_ptr<ProbabilisticTask> task,
    const std::vector<std::shared_ptr<Evaluator>>& path_dependent_evaluators)
    : TaskStateSpace(task, path_dependent_evaluators)
{
}

StateID CachingTaskStateSpace::get_state_id(const State& state)
{
    return state.get_id();
}

State CachingTaskStateSpace::get_state(StateID state_id)
{
    return state_registry_.lookup_state(::StateID(state_id));
}

ActionID CachingTaskStateSpace::get_action_id(StateID, OperatorID op_id)
{
    return ActionID(op_id.get_index());
}

OperatorID CachingTaskStateSpace::get_action(StateID, ActionID action_id)
{
    return OperatorID(action_id);
}

void CachingTaskStateSpace::generate_applicable_actions(
    StateID state_id,
    std::vector<OperatorID>& result)
{
    CacheEntry& entry = lookup(state_id);
    result.reserve(entry.naops);
    for (size_t i = 0; i != entry.naops; ++i) {
        result.push_back(entry.aops[i]);
    }

#ifdef DEBUG_CACHE_CONSISTENCY_CHECK
    State state = state_registry_.lookup_state(::StateID(state_id));
    std::vector<OperatorID> test;
    compute_applicable_operators(state, test);
    assert(std::ranges::equal(test, result));
#endif

    ++statistics_.aops_generator_calls;
    statistics_.generated_operators += result.size();
}

void CachingTaskStateSpace::generate_action_transitions(
    StateID state_id,
    OperatorID op_id,
    Distribution<StateID>& result)
{
    const ProbabilisticOperatorsProxy operators = task_proxy.get_operators();

#ifdef DEBUG_CACHE_CONSISTENCY_CHECK
    {
        State state = state_registry_.lookup_state(::StateID(state_id));
        for (const FactProxy fact : operators[op_id].get_preconditions()) {
            assert(state[fact.get_variable()].get_value() == fact.get_value());
        }
    }
#endif

    const CacheEntry& entry = cache_[state_id];
    assert(entry.is_initialized());
    const StateID* succs = entry.succs;

    for (size_t i = 0; i < entry.naops; ++i) {
        OperatorID id = entry.aops[i];
        ProbabilisticOperatorProxy op = operators[id];
        ProbabilisticOutcomesProxy outcomes = op.get_outcomes();

        const size_t num_outcomes = outcomes.size();

        if (op_id != id) {
            succs += num_outcomes;
            continue;
        }

#ifdef DEBUG_CACHE_CONSISTENCY_CHECK
        State state = state_registry_.lookup_state(::StateID(state_id));
        std::vector<ItemProbabilityPair<StateID>> test;
        compute_successor_states(state, op_id, test);
        assert(test.size() == num_outcomes);
#endif
        for (unsigned j = 0; j < num_outcomes; ++j, ++succs) {
            value_t probability = outcomes[j].get_probability();
            assert(test[j] == ItemProbabilityPair(*succs, probability));
            result.add(*succs, probability);
        }

        break;
    }

    ++statistics_.single_transition_generator_calls;
    statistics_.generated_states += result.size();
}

void CachingTaskStateSpace::generate_all_transitions(
    StateID state_id,
    std::vector<OperatorID>& aops,
    std::vector<Distribution<StateID>>& successors)
{
    const ProbabilisticOperatorsProxy operators = task_proxy.get_operators();

    CacheEntry& entry = lookup(state_id);
    const StateID* succs = entry.succs;
    aops.reserve(entry.naops);
    successors.resize(entry.naops);

#ifdef DEBUG_CACHE_CONSISTENCY_CHECK
    State state = state_registry_.lookup_state(::StateID(state_id));
    std::vector<OperatorID> test_aops;
    compute_applicable_operators(state, test_aops);
    assert(test_aops.size() == entry.naops);
#endif

    for (unsigned i = 0; i < entry.naops; ++i) {
        OperatorID op_id = entry.aops[i];
        aops.push_back(op_id);

#ifdef DEBUG_CACHE_CONSISTENCY_CHECK
        assert(aops[i] == test_aops[i]);
        std::vector<ItemProbabilityPair<StateID>> test;
        compute_successor_states(state, aops[i], test);
#endif

        Distribution<StateID>& result = successors[i];

        const ProbabilisticOperatorProxy op = operators[op_id];
        const ProbabilisticOutcomesProxy outcomes = op.get_outcomes();
        const size_t num_outcomes = outcomes.size();

        for (unsigned j = 0; j < num_outcomes; ++j, ++succs) {
            value_t probability = outcomes[j].get_probability();
            assert(test[j] == ItemProbabilityPair(*succs, probability));
            result.add(*succs, probability);
        }

        statistics_.generated_states += result.size();
    }

    ++statistics_.all_transitions_generator_calls;
    statistics_.generated_operators += aops.size();
}

bool CachingTaskStateSpace::setup_cache(StateID state_id, CacheEntry& entry)
{
    if (!entry.is_initialized()) {
        State state = state_registry_.lookup_state(::StateID(state_id));
        assert(aops_.empty() && successors_.empty());
        compute_applicable_operators(state, aops_);
        entry.naops = aops_.size();

        if (entry.naops > 0) {
            entry.aops = cache_data_.allocate<OperatorID>(aops_.size());

            std::vector<ItemProbabilityPair<StateID>> succs;
            for (size_t i = 0; i < aops_.size(); ++i) {
                OperatorID op = aops_[i];
                entry.aops[i] = op;

                compute_successor_states(state, op, succs);

                for (const auto& s : succs) {
                    successors_.push_back(s.item);
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

CachingTaskStateSpace::CacheEntry& CachingTaskStateSpace::lookup(StateID sid)
{
    CacheEntry& entry = cache_[sid];
    setup_cache(sid, entry);
    return entry;
}

CachingTaskStateSpace::CacheEntry&
CachingTaskStateSpace::lookup(StateID sid, bool& setup)
{
    CacheEntry& entry = cache_[sid];
    setup = setup_cache(sid, entry);
    return entry;
}

void CachingTaskStateSpace::print_statistics(std::ostream& out) const
{
    TaskStateSpace::print_statistics(out);
    out << "  Stored arrays in bytes: " << cache_data_.size_in_bytes()
        << std::endl;
}

} // namespace probfd

#undef DEBUG_CACHE_CONSISTENCY_CHECK
