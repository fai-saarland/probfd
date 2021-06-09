#pragma once

#include "../../algorithms/segmented_vector.h"
#include "../../operator_cost.h"
#include "../../state_id.h"
#include "../distribution.h"
#include "../engine_interfaces/action_evaluator.h"
#include "../engine_interfaces/action_id_map.h"
#include "../engine_interfaces/applicable_actions_generator.h"
#include "../engine_interfaces/state_evaluator.h"
#include "../engine_interfaces/state_reward_function.h"
#include "../engine_interfaces/state_id_map.h"
#include "../engine_interfaces/transition_generator.h"

#include <cassert>
#include <unordered_set>
#include <vector>

class GlobalState;
namespace merge_and_shrink {
class Abstraction;
}

namespace probabilistic {
namespace bisimulation {

class QuotientState : public ::StateID {
public:
    using ::StateID::StateID;
    explicit QuotientState(const ::StateID& id)
        : ::StateID(id)
    {
    }
    QuotientState& operator=(const ::StateID& id)
    {
        StateID::operator=(id);
        return *this;
    }
    bool operator==(const ::StateID& s) const
    {
        return this->hash() == s.hash();
    }
    bool operator!=(const ::StateID& s) const
    {
        return this->hash() != s.hash();
    }
    bool operator<(const ::StateID& s) const { return this->hash() < s.hash(); }
};

struct QuotientAction {
    explicit QuotientAction(unsigned idx)
        : idx(idx)
    {
    }

    bool operator==(const QuotientAction& o) const { return o.idx == idx; }

    unsigned idx;
};

struct CachedTransition {
    unsigned op;
    int* successors;
};

class BisimilarStateSpace {
public:
    explicit BisimilarStateSpace(
        const GlobalState& initial_state,
        int budget,
        OperatorCost cost_type);
    ~BisimilarStateSpace();

    QuotientState get_initial_state() const;
    bool has_limited_budget() const;
    bool is_goal_state(const QuotientState& s) const;
    bool is_dead_end(const QuotientState& s) const;

    void get_applicable_actions(
        const StateID& s,
        std::vector<QuotientAction>& result) const;

    void get_successors(
        const StateID& s,
        const QuotientAction& action,
        Distribution<StateID>& succs);

    QuotientState get_budget_extended_state(const int& ref, const int& budget);

    unsigned num_bisimilar_states() const;
    unsigned num_extended_states() const;
    unsigned num_transitions() const;

    void dump(std::ostream& out) const;
    void dump_extended(std::ostream& out) const;

private:
    struct Hash {
        using elem_type = std::pair<int, int>;
        explicit Hash(
            const segmented_vector::SegmentedVector<elem_type>* extended)
            : extended_(extended)
        {
        }
        std::size_t operator()(unsigned i) const;
        const segmented_vector::SegmentedVector<elem_type>* extended_;
    };

    struct Equal {
        using elem_type = std::pair<int, int>;
        explicit Equal(
            const segmented_vector::SegmentedVector<elem_type>* extended)
            : extended_(extended)
        {
        }
        bool operator()(int i, int j) const;
        const segmented_vector::SegmentedVector<elem_type>* extended_;
    };

    const bool limited_budget_;
    const OperatorCost cost_type_;

    merge_and_shrink::Abstraction* abstraction_;
    unsigned num_cached_transitions_;

    QuotientState initial_state_;
    QuotientState dead_end_state_;
    segmented_vector::SegmentedVector<std::vector<CachedTransition>>
        transitions_;
    std::vector<int> operator_cost_;
    
    std::vector<int*> store_;

    segmented_vector::SegmentedVector<std::pair<int, int>> extended_;
    std::unordered_set<int, Hash, Equal> ids_;
};

} // namespace bisimulation

template<>
struct StateIDMap<bisimulation::QuotientState> {
    StateID get_state_id(const bisimulation::QuotientState& state) const;
    bisimulation::QuotientState get_state(const StateID& state_id) const;
};

template<>
struct ActionIDMap<bisimulation::QuotientAction> {
    ActionID get_action_id(
        const StateID& state_id,
        const bisimulation::QuotientAction& action) const;
    bisimulation::QuotientAction
    get_action(const StateID& state_id, const ActionID& action) const;
};

template<>
struct ApplicableActionsGenerator<bisimulation::QuotientAction> {
    explicit ApplicableActionsGenerator(
        bisimulation::BisimilarStateSpace* bisim);
    void operator()(
        const StateID& state,
        std::vector<bisimulation::QuotientAction>& result) const;
    bisimulation::BisimilarStateSpace* bisim_;
};

template<>
struct TransitionGenerator<bisimulation::QuotientAction> {
    explicit TransitionGenerator(bisimulation::BisimilarStateSpace* bisim);
    void operator()(
        const StateID& state,
        const bisimulation::QuotientAction& action,
        Distribution<StateID>& result) const;
    void operator()(
        const StateID& state,
        std::vector<bisimulation::QuotientAction>& aops,
        std::vector<Distribution<StateID>>& result) const;
    bisimulation::BisimilarStateSpace* bisim_;
};

namespace bisimulation {

using QuotientStateRewardFunction = StateRewardFunction<bisimulation::QuotientState>;
using QuotientActionRewardFunction = ActionRewardFunction<bisimulation::QuotientAction>;
using QuotientStateEvaluator = StateEvaluator<bisimulation::QuotientState>;

struct DefaultQuotientStateEvaluator
    : public QuotientStateEvaluator
{
    explicit DefaultQuotientStateEvaluator(
        bisimulation::BisimilarStateSpace* bisim,
        const value_type::value_t min,
        const value_type::value_t max,
        value_type::value_t default_value = 0);

    EvaluationResult
    evaluate(const bisimulation::QuotientState& state) override;

    bisimulation::BisimilarStateSpace* bisim_;
    const value_type::value_t min_;
    const value_type::value_t max_;
    const value_type::value_t default_;
};

struct DefaultQuotientStateRewardFunction
    : public QuotientStateRewardFunction
{
    explicit DefaultQuotientStateRewardFunction(
        bisimulation::BisimilarStateSpace* bisim,
        const value_type::value_t min,
        const value_type::value_t max,
        value_type::value_t default_value = 0);

    EvaluationResult
    evaluate(const bisimulation::QuotientState& state) override;

    bisimulation::BisimilarStateSpace* bisim_;
    const value_type::value_t min_;
    const value_type::value_t max_;
    const value_type::value_t default_;
};

struct DefaultQuotientActionRewardFunction
    : public QuotientActionRewardFunction
{
    value_type::value_t
    evaluate(StateID state, bisimulation::QuotientAction action) override;
};

}

} // namespace probabilistic

namespace std {
template<>
struct hash<probabilistic::bisimulation::QuotientState> {
    size_t operator()(const probabilistic::bisimulation::QuotientState& s) const
    {
        return s.hash();
    }
};
} // namespace std
