#ifndef MDPS_HEURISTICS_PDBS_MAXPROB_PROJECTION_H
#define MDPS_HEURISTICS_PDBS_MAXPROB_PROJECTION_H

#include "probfd/heuristics/pdbs/abstract_policy.h"
#include "probfd/heuristics/pdbs/probabilistic_projection.h"

#include "probfd/heuristics/constant_evaluator.h"

#include "probfd/value_utils.h"

namespace pdbs {
class PatternDatabase;
}

namespace probfd {
namespace heuristics {
namespace pdbs {

class MaxProbProjection : public ProbabilisticProjection {
public:
    MaxProbProjection(
        const ProbabilisticTaskProxy& task_proxy,
        const Pattern& pattern,
        bool operator_pruning = true,
        const StateRankEvaluator& heuristic =
            ConstantEvaluator<StateRank>(-1_vt));

    MaxProbProjection(
        const ProbabilisticTaskProxy& task_proxy,
        const ::pdbs::PatternDatabase& pdb,
        bool operator_pruning = true);

    MaxProbProjection(
        const ProbabilisticTaskProxy& task_proxy,
        const MaxProbProjection& pdb,
        int add_var,
        bool operator_pruning = true);

    MaxProbProjection(
        const ProbabilisticTaskProxy& task_proxy,
        const MaxProbProjection& left,
        const MaxProbProjection& right,
        bool operator_pruning = true);

    [[nodiscard]] EvaluationResult evaluate(const State& s) const;
    [[nodiscard]] EvaluationResult evaluate(const StateRank& s) const;

    AbstractPolicy get_optimal_abstract_policy(
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        bool wildcard = false) const;

    void dump_graphviz(const std::string& path, bool transition_labels = true);

private:
    void compute_value_table(const StateRankEvaluator& heuristic);

#if !defined(NDEBUG) && defined(USE_LP)
    void verify(const engine_interfaces::StateIDMap<StateRank>& state_id_map);
#endif
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __MAXPROB_PROJECTION_H__