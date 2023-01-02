#ifndef MDPS_HEURISTICS_PDBS_EXPCOST_PROJECTION_H
#define MDPS_HEURISTICS_PDBS_EXPCOST_PROJECTION_H

#include "probfd/heuristics/pdbs/abstract_policy.h"
#include "probfd/heuristics/pdbs/probabilistic_projection.h"

#include "probfd/heuristics/constant_evaluator.h"

namespace pdbs {
class PatternDatabase;
}

namespace probfd {
namespace heuristics {
namespace pdbs {

class ExpCostProjection : public ProbabilisticProjection {
public:
    explicit ExpCostProjection(
        const ProbabilisticTaskProxy& task_proxy,
        const Pattern& variables,
        bool operator_pruning = true,
        const StateRankEvaluator& heuristic =
            ConstantEvaluator<StateRank>(value_type::zero));

    explicit ExpCostProjection(
        const ProbabilisticTaskProxy& task_proxy,
        const ::pdbs::PatternDatabase& pdb,
        bool operator_pruning = true);

    explicit ExpCostProjection(
        const ProbabilisticTaskProxy& task_proxy,
        const ExpCostProjection& pdb,
        int add_var,
        bool operator_pruning = true);

    explicit ExpCostProjection(
        const ProbabilisticTaskProxy& task_proxy,
        StateRankingFunction* mapper,
        bool operator_pruning = true,
        const StateRankEvaluator& heuristic =
            ConstantEvaluator<StateRank>(value_type::zero));

    [[nodiscard]] EvaluationResult evaluate(const State& s) const;
    [[nodiscard]] EvaluationResult evaluate(const StateRank& s) const;

    AbstractPolicy get_optimal_abstract_policy(
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        bool wildcard = false) const;

    void
    dump_graphviz(const std::string& path, bool transition_labels = true) const;

private:
    void compute_value_table(const StateRankEvaluator& heuristic);

#if !defined(NDEBUG) && defined(USE_LP)
    void verify(
        const engine_interfaces::StateIDMap<StateRank>& state_id_map,
        const std::vector<StateID>& proper_states);
#endif
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __EXPCOST_PROJECTION_H__