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
namespace heuristics {
namespace pdbs {

using StateRankEvaluator = engine_interfaces::Evaluator<StateRank>;
using AbstractCostFunction =
    engine_interfaces::CostFunction<StateRank, const AbstractOperator*>;

class ProjectionStateSpace;

class PDBEvaluator : public StateRankEvaluator {
public:
    explicit PDBEvaluator(const ::pdbs::PatternDatabase& pdb);

    EvaluationResult evaluate(StateRank state) const override;

private:
    const ::pdbs::PatternDatabase& pdb;
};

class DeadendPDBEvaluator : public StateRankEvaluator {
public:
    explicit DeadendPDBEvaluator(const ::pdbs::PatternDatabase& pdb);

    EvaluationResult evaluate(StateRank state) const override;

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

    EvaluationResult evaluate(StateRank state) const override;
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
    EvaluationResult evaluate(StateRank state) const override;
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

    TerminationInfo get_termination_info(StateRank state) override;
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