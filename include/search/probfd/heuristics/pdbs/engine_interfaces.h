#ifndef PROBFD_HEURISTICS_PDBS_ENGINE_INTERFACES_H
#define PROBFD_HEURISTICS_PDBS_ENGINE_INTERFACES_H

#include "probfd/heuristics/pdbs/abstract_operator.h"
#include "probfd/heuristics/pdbs/match_tree.h"
#include "probfd/heuristics/pdbs/state_ranking_function.h"

#include "probfd/engine_interfaces/cost_function.h"
#include "probfd/engine_interfaces/evaluator.h"
#include "probfd/engine_interfaces/state_space.h"

#include "utils/collections.h"

#include <memory>
#include <ranges>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace pdbs {
class PatternDatabase;
}

namespace probfd {
namespace engine_interfaces {
template <>
class StateSpace<
    heuristics::pdbs::StateRank,
    const heuristics::pdbs::AbstractOperator*> {
    heuristics::pdbs::MatchTree match_tree_;

public:
    StateSpace(
        const ProbabilisticTaskProxy& task_proxy,
        const heuristics::pdbs::StateRankingFunction& ranking_function,
        bool operator_pruning = true);

    StateID get_state_id(heuristics::pdbs::StateRank state) const;

    heuristics::pdbs::StateRank get_state(StateID id) const;

    ActionID
    get_action_id(StateID, const heuristics::pdbs::AbstractOperator* op) const;

    const heuristics::pdbs::AbstractOperator*
    get_action(StateID, ActionID action_id) const;

    void generate_applicable_actions(
        StateID state,
        std::vector<const heuristics::pdbs::AbstractOperator*>& aops);

    void generate_action_transitions(
        StateID state,
        const heuristics::pdbs::AbstractOperator* op,
        Distribution<StateID>& result);

    void generate_all_transitions(
        StateID state,
        std::vector<const heuristics::pdbs::AbstractOperator*>& aops,
        std::vector<Distribution<StateID>>& result);
};

} // namespace engine_interfaces

namespace heuristics {
namespace pdbs {

using StateRankEvaluator = engine_interfaces::Evaluator<StateRank>;
using AbstractCostFunction =
    engine_interfaces::CostFunction<StateRank, const AbstractOperator*>;

class ProjectionStateSpace;

class PDBEvaluator : public StateRankEvaluator {
public:
    explicit PDBEvaluator(const ::pdbs::PatternDatabase& pdb);

    EvaluationResult evaluate(const StateRank& state) const override;

private:
    const ::pdbs::PatternDatabase& pdb;
};

class DeadendPDBEvaluator : public StateRankEvaluator {
public:
    explicit DeadendPDBEvaluator(const ::pdbs::PatternDatabase& pdb);

    EvaluationResult evaluate(const StateRank& state) const override;

private:
    const ::pdbs::PatternDatabase& pdb;
};

class IncrementalPPDBEvaluatorBase : public StateRankEvaluator {
    int left_multiplier;
    int right_multiplier;
    int domain_size;

public:
    explicit IncrementalPPDBEvaluatorBase(
        const StateRankingFunction* mapper,
        int add_var);

protected:
    StateRank to_parent_state(StateRank state) const;
};

template <typename PDBType>
class IncrementalPPDBEvaluator : public IncrementalPPDBEvaluatorBase {
    const PDBType& pdb;

public:
    explicit IncrementalPPDBEvaluator(
        const PDBType& pdb,
        const StateRankingFunction* mapper,
        int add_var);

    EvaluationResult evaluate(const StateRank& state) const override;
};

template <typename PDBType>
class MergeEvaluator : public engine_interfaces::Evaluator<StateRank> {
    const StateRankingFunction& mapper;
    const PDBType& left;
    const PDBType& right;

public:
    MergeEvaluator(
        const StateRankingFunction& mapper,
        const PDBType& left,
        const PDBType& right);

protected:
    EvaluationResult evaluate(const StateRank& state) const override;
};

class BaseAbstractCostFunction : public AbstractCostFunction {
protected:
    const ProjectionStateSpace& state_space_;
    const value_t value_in_;
    const value_t value_not_in_;

public:
    BaseAbstractCostFunction(
        const ProjectionStateSpace& state_space,
        value_t value_in,
        value_t value_not_in);

    TerminationInfo get_termination_info(const StateRank& state) override;
};

class ZeroCostAbstractCostFunction : public BaseAbstractCostFunction {
public:
    using BaseAbstractCostFunction::BaseAbstractCostFunction;
    value_t get_action_cost(StateID, const AbstractOperator*) override;
};

class NormalCostAbstractCostFunction : public BaseAbstractCostFunction {
public:
    using BaseAbstractCostFunction::BaseAbstractCostFunction;
    value_t get_action_cost(StateID, const AbstractOperator* op) override;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __ENGINE_INTERFACES_H__