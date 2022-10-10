#ifndef MDPS_TRANSITION_GENERATOR_H
#define MDPS_TRANSITION_GENERATOR_H

#include "../algorithms/segmented_array_store.h"
#include "../algorithms/segmented_vector.h"
#include "../operator_cost.h"
#include "engine_interfaces/transition_generator.h"
#include "probabilistic_operator.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

class StateRegistry;
class Heuristic;
class GlobalState;

namespace successor_generator {
template <typename Op>
class SuccessorGenerator;
}

namespace probabilistic {

namespace quotient_system {
template <typename T>
class QuotientSystem;
}

struct CostBasedSuccessorGenerator {
public:
    explicit CostBasedSuccessorGenerator(
        const std::vector<ProbabilisticOperator>& ops,
        OperatorCost cost_type,
        int bvar,
        successor_generator::SuccessorGenerator<const ProbabilisticOperator*>*
            gen);

    void operator()(
        const GlobalState& s,
        std::vector<const ProbabilisticOperator*>& result) const;

private:
    int bvar_;
    std::vector<int> reward_;
    std::vector<std::shared_ptr<
        successor_generator::SuccessorGenerator<const ProbabilisticOperator*>>>
        gens_;
    successor_generator::SuccessorGenerator<const ProbabilisticOperator*>* gen_;
};

template <>
class TransitionGenerator<const ProbabilisticOperator*> {
    friend class quotient_system::QuotientSystem<const ProbabilisticOperator*>;

public:
    explicit TransitionGenerator(
        const std::vector<ProbabilisticOperator>& ops,
        successor_generator::SuccessorGenerator<const ProbabilisticOperator*>*
            gen,
        int budget_var,
        OperatorCost budget_cost_type,
        StateRegistry* state_registry,
        bool enable_caching,
        const std::vector<std::shared_ptr<Heuristic>>&
            path_dependent_heuristics);

    explicit TransitionGenerator(
        StateRegistry* state_registry,
        bool enable_caching,
        const std::vector<std::shared_ptr<Heuristic>>&
            path_dependent_heuristics);

    void operator()(
        const StateID& state_id,
        std::vector<const ProbabilisticOperator*>& result);

    void operator()(
        const StateID& state,
        const ProbabilisticOperator* const& action,
        Distribution<StateID>& result);

    void operator()(
        const StateID& state,
        std::vector<const ProbabilisticOperator*>& aops,
        std::vector<Distribution<StateID>>& successors);

    void print_statistics(std::ostream& out) const;

private:
    struct CacheEntry {
        bool is_initialized() const
        {
            return naops != std::numeric_limits<unsigned>::max();
        }

        unsigned naops = std::numeric_limits<unsigned>::max();
        uint32_t* aops = nullptr;
        uint32_t* succs = nullptr;
    };

    struct Statistics {
        void print(std::ostream& out) const;

        unsigned long long aops_computations = 0;
        unsigned long long computed_operators = 0;

        unsigned long long transition_computations = 0;
        unsigned long long computed_states = 0;

        unsigned long long single_transition_generator_calls = 0;
        unsigned long long all_transitions_generator_calls = 0;
        unsigned long long aops_generator_calls = 0;

        unsigned long long generated_operators = 0;
        unsigned long long generated_states = 0;
    };

    using Cache = segmented_vector::DynamicSegmentedVector<CacheEntry>;

    void compute_successor_states(
        const GlobalState& s,
        const ProbabilisticOperator* op,
        std::vector<std::pair<StateID, value_type::value_t>>& successors);

    void compute_applicable_operators(
        const GlobalState& s,
        std::vector<const ProbabilisticOperator*>& ops);

    bool setup_cache(const StateID& state_id, CacheEntry& entry);

    CacheEntry& lookup(const StateID& state_id);
    CacheEntry& lookup(const StateID& state_id, bool& initialized);

    const ProbabilisticOperator* first_op_;
    const bool caching_;
    const int budget_var_;
    const OperatorCost budget_cost_type_;
    const std::vector<std::shared_ptr<Heuristic>> notify_;
    CostBasedSuccessorGenerator aops_gen_;
    std::vector<int> reward_;

    StateRegistry* state_registry_;

    Cache cache_;
    segmented_array_store::SegmentedArrayStore<uint32_t> cache_data_;
    std::vector<const ProbabilisticOperator*> aops_;
    std::vector<StateID> successors_;

    Statistics statistics_;
};

} // namespace probabilistic

#endif // __TRANSITION_GENERATOR_H__