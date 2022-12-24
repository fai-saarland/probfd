#ifndef MDPS_TRANSITION_GENERATOR_H
#define MDPS_TRANSITION_GENERATOR_H

#include "probfd/engine_interfaces/transition_generator.h"

#include "probfd/probabilistic_operator.h"

#include "probfd/storage/dynamic_segmented_vector.h"
#include "probfd/storage/segmented_memory_pool.h"

#include "algorithms/segmented_vector.h"

#include "legacy/operator_cost.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

namespace legacy {
class GlobalState;
class Heuristic;
class StateRegistry;
namespace successor_generator {
template <typename Op>
class SuccessorGenerator;
}
} // namespace legacy

namespace probfd {

namespace quotient_system {
template <typename T>
class QuotientSystem;
}

class TransitionGeneratorBase {
    friend class quotient_system::QuotientSystem<const ProbabilisticOperator*>;

protected:
    struct Statistics {
        unsigned long long single_transition_generator_calls = 0;
        unsigned long long all_transitions_generator_calls = 0;
        unsigned long long aops_generator_calls = 0;

        unsigned long long generated_operators = 0;
        unsigned long long generated_states = 0;

        unsigned long long aops_computations = 0;
        unsigned long long computed_operators = 0;

        unsigned long long transition_computations = 0;
        unsigned long long computed_successors = 0;

        void print(std::ostream& out) const;
    };

    struct CacheEntry {
        bool is_initialized() const
        {
            return naops != std::numeric_limits<unsigned>::max();
        }

        unsigned naops = std::numeric_limits<unsigned>::max();
        ActionID* aops = nullptr;
        StateID* succs = nullptr;
    };

    using Cache = storage::DynamicSegmentedVector<CacheEntry>;

protected:
    const ProbabilisticOperator* first_op_;
    const bool caching_;
    const std::vector<std::shared_ptr<legacy::Heuristic>> notify_;

    legacy::StateRegistry* state_registry_;

    Cache cache_;
    storage::SegmentedMemoryPool<> cache_data_;
    std::vector<const ProbabilisticOperator*> aops_;
    std::vector<StateID> successors_;

    Statistics statistics_;

public:
    TransitionGeneratorBase(
        legacy::StateRegistry* state_registry,
        const std::vector<ProbabilisticOperator>& ops,
        const std::vector<std::shared_ptr<legacy::Heuristic>>&
            path_dependent_heuristics,
        bool enable_caching);

    virtual ~TransitionGeneratorBase() = default;

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

protected:
    virtual void compute_successor_states(
        const legacy::GlobalState& s,
        const ProbabilisticOperator* op,
        std::vector<WeightedElement<StateID>>& successors) = 0;

    virtual void compute_applicable_operators(
        const legacy::GlobalState& s,
        std::vector<const ProbabilisticOperator*>& ops) = 0;

    bool setup_cache(const StateID& state_id, CacheEntry& entry);

    CacheEntry& lookup(const StateID& state_id);
    CacheEntry& lookup(const StateID& state_id, bool& initialized);
};

class BudgetTransitionGenerator : public TransitionGeneratorBase {
    const int budget_var_;
    const legacy::OperatorCost budget_cost_type_;
    std::vector<int> reward_;

    std::vector<std::shared_ptr<legacy::successor_generator::SuccessorGenerator<
        const ProbabilisticOperator*>>>
        gens_;

public:
    explicit BudgetTransitionGenerator(
        legacy::StateRegistry* state_registry,
        const std::vector<ProbabilisticOperator>& ops,
        const std::vector<std::shared_ptr<legacy::Heuristic>>&
            path_dependent_heuristics,
        bool enable_caching,
        int budget_var,
        legacy::OperatorCost budget_cost_type);

    ~BudgetTransitionGenerator() override final = default;

private:
    void compute_successor_states(
        const legacy::GlobalState& s,
        const ProbabilisticOperator* op,
        std::vector<WeightedElement<StateID>>& successors) override final;

    void compute_applicable_operators(
        const legacy::GlobalState& s,
        std::vector<const ProbabilisticOperator*>& ops) override final;
};

class DefaultTransitionGenerator : public TransitionGeneratorBase {
    legacy::successor_generator::SuccessorGenerator<
        const ProbabilisticOperator*>* gen_;

public:
    explicit DefaultTransitionGenerator(
        legacy::StateRegistry* state_registry,
        const std::vector<ProbabilisticOperator>& ops,
        const std::vector<std::shared_ptr<legacy::Heuristic>>&
            path_dependent_heuristics,
        bool enable_caching,
        legacy::successor_generator::SuccessorGenerator<
            const ProbabilisticOperator*>* gen);

    ~DefaultTransitionGenerator() override final = default;

private:
    void compute_successor_states(
        const legacy::GlobalState& s,
        const ProbabilisticOperator* op,
        std::vector<WeightedElement<StateID>>& successors) override final;

    void compute_applicable_operators(
        const legacy::GlobalState& s,
        std::vector<const ProbabilisticOperator*>& ops) override final;
};

namespace engine_interfaces {

template <>
class TransitionGenerator<const ProbabilisticOperator*> {
    std::unique_ptr<TransitionGeneratorBase> base;

public:
    TransitionGenerator(
        legacy::StateRegistry* state_registry,
        const std::vector<ProbabilisticOperator>& ops,
        const std::vector<std::shared_ptr<legacy::Heuristic>>&
            path_dependent_heuristics,
        bool enable_caching);

    ~TransitionGenerator() = default;

    TransitionGeneratorBase& getBase() { return *base; }

    void operator()(
        const StateID& state_id,
        std::vector<const ProbabilisticOperator*>& result)
    {
        return base->operator()(state_id, result);
    }

    void operator()(
        const StateID& state,
        const ProbabilisticOperator* const& action,
        Distribution<StateID>& result)
    {
        return base->operator()(state, action, result);
    }

    void operator()(
        const StateID& state,
        std::vector<const ProbabilisticOperator*>& aops,
        std::vector<Distribution<StateID>>& successors)
    {
        return base->operator()(state, aops, successors);
    }

    void print_statistics(std::ostream& out) const
    {
        base->print_statistics(out);
    }
};

} // namespace engine_interfaces

} // namespace probfd

#endif // __TRANSITION_GENERATOR_H__