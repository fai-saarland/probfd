#ifndef MDPS_HEURISTICS_PDBS_MAXPROB_PROJECTION_H
#define MDPS_HEURISTICS_PDBS_MAXPROB_PROJECTION_H

#include "probfd/heuristics/pdbs/abstract_policy.h"
#include "probfd/heuristics/pdbs/probabilistic_projection.h"

#include "probfd/heuristics/constant_evaluator.h"

#include "probfd/globals.h"
#include "probfd/value_utils.h"

namespace pdbs {
class PatternDatabase;
}

namespace probfd {
namespace heuristics {
namespace pdbs {

class MaxProbProjection : public ProbabilisticProjection {
public:
    explicit MaxProbProjection(
        const Pattern& pattern,
        const std::vector<int>& domains = ::g_variable_domain,
        bool operator_pruning = true,
        const AbstractStateEvaluator& heuristic =
            ConstantEvaluator<AbstractState>(value_type::one));

    explicit MaxProbProjection(
        AbstractStateMapper* mapper,
        bool operator_pruning = true,
        const AbstractStateEvaluator& heuristic =
            ConstantEvaluator<AbstractState>(value_type::one));

    explicit MaxProbProjection(
        const ::pdbs::PatternDatabase& pdb,
        bool operator_pruning = true);

    explicit MaxProbProjection(
        const MaxProbProjection& pdb,
        int add_var,
        bool operator_pruning = true);

    [[nodiscard]] EvaluationResult evaluate(const GlobalState& s) const;
    [[nodiscard]] EvaluationResult evaluate(const AbstractState& s) const;

    AbstractPolicy get_optimal_abstract_policy(
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        bool wildcard = false) const;

    void dump_graphviz(const std::string& path, bool transition_labels = true);

private:
    void compute_value_table(const AbstractStateEvaluator& heuristic);

#if !defined(NDEBUG) && defined(USE_LP)
    void
    verify(const engine_interfaces::StateIDMap<AbstractState>& state_id_map);
#endif
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __MAXPROB_PROJECTION_H__