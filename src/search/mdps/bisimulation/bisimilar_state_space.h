#pragma once

#include "../../algorithms/segmented_vector.h"
#include "../../operator_cost.h"
#include "../../state_id.h"
#include "../types.h"
#include "../distribution.h"

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
