#ifndef MDPS_BISIMULATION_BISIMILAR_STATE_SPACE_H
#define MDPS_BISIMULATION_BISIMILAR_STATE_SPACE_H

#include "../../algorithms/segmented_vector.h"
#include "../../operator_cost.h"
#include "../../state_id.h"
#include "../distribution.h"
#include "../types.h"

#include <cassert>
#include <unordered_set>
#include <vector>

class GlobalState;
namespace merge_and_shrink {
class Abstraction;
}

namespace probabilistic {

/// Namespace dedicated to probabilistic bisimulation.
namespace bisimulation {

/** \class QuotientState
 * Class representing a state in a quotient MDP.
 */
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

/** \struct QuotientAction
 * Struct representing an action in a quotient MDP.
 */
struct QuotientAction {
    explicit QuotientAction(unsigned idx)
        : idx(idx)
    {
    }

    bool operator==(const QuotientAction& o) const { return o.idx == idx; }

    unsigned idx; ///< Numbering of this action
};

struct CachedTransition {
    unsigned op;
    int* successors;
};

/**
 * Class representing a probabilistic bisimulation of an MDP.
 */
class BisimilarStateSpace {
public:
    /**
     * @brief Construct the probabilistic bisimulation with the given initial
     * state, budget and operator cost type.
     *
     * @param initial_state - The initial state of the original MDP.
     * @param budget - The considered budget. Action costs may not exceed this
     * value. An unlimited budget can be specified with \ref g_unlimited_budget.
     * @param cost_type - The operator cost type.
     */
    explicit BisimilarStateSpace(
        const GlobalState& initial_state,
        int budget,
        OperatorCost cost_type);
    ~BisimilarStateSpace();

    /// Get the initial state of the quotient MDP.
    QuotientState get_initial_state() const;

    /// Returns true iff a limited budget is considered.
    bool has_limited_budget() const;

    /// Returns true iff the given quotient state is a goal.
    bool is_goal_state(const QuotientState& s) const;

    /// Returns true iff the given quotient state is a dead end.
    bool is_dead_end(const QuotientState& s) const;

    /// Returns the applicable actions for the state \p s in \p result.
    void get_applicable_actions(
        const StateID& s,
        std::vector<QuotientAction>& result) const;

    /**
     * @brief Returns the successor distribution for the state \p s and
     * quotient action \p action in \p succs .
     */
    void get_successors(
        const StateID& s,
        const QuotientAction& action,
        Distribution<StateID>& succs);

    QuotientState get_budget_extended_state(const int& ref, const int& budget);

    /**
     * @brief Returns the number of states in the probabilistic bisimulation,
     * not considering the remaining budget.
     */
    unsigned num_bisimilar_states() const;

    /**
     * @brief Returns the number of states in the probabilistic bisimulation
     * where states are extended with their finite budget.
     */
    unsigned num_extended_states() const;

    /**
     * @brief Returns the number of abstract transiitons in the probabilistic
     * bisimulation.
     */
    unsigned num_transitions() const;

    /// Dumps the quotient state space.
    void dump(std::ostream& out) const;

    /// Dumps the quotient state space verbosely.
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
} // namespace probabilistic

namespace std {
template <>
struct hash<probabilistic::bisimulation::QuotientState> {
    size_t operator()(const probabilistic::bisimulation::QuotientState& s) const
    {
        return s.hash();
    }
};
} // namespace std

#endif // __BISIMILAR_STATE_SPACE_H__