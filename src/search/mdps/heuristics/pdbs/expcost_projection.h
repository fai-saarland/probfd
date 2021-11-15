#ifndef MDPS_HEURISTICS_PDBS_EXPCOST_PROJECTION_H
#define MDPS_HEURISTICS_PDBS_EXPCOST_PROJECTION_H

#include "../../globals.h"

#include "../constant_evaluator.h"

#include "abstract_policy.h"
#include "probabilistic_projection.h"

namespace pdbs {
class PatternDatabase;
}

namespace probabilistic {
namespace pdbs {

class ExpCostProjection : public pdbs::ProbabilisticProjection {
    std::vector<value_type::value_t> value_table;

public:
    explicit ExpCostProjection(
        const Pattern& variables,
        const std::vector<int>& domains = ::g_variable_domain,
        bool operator_pruning = true,
        const AbstractStateEvaluator& heuristic =
            ConstantEvaluator<AbstractState>(value_type::zero));

    explicit ExpCostProjection(
        AbstractStateMapper* mapper,
        bool operator_pruning = true,
        const AbstractStateEvaluator& heuristic =
            ConstantEvaluator<AbstractState>(value_type::zero));

    explicit ExpCostProjection(
        const ::pdbs::PatternDatabase& pdb,
        bool operator_pruning = true);

    explicit ExpCostProjection(
        const ExpCostProjection& pdb,
        int add_var,
        bool operator_pruning = true);

    [[nodiscard]] value_type::value_t lookup(const GlobalState& s) const;
    [[nodiscard]] value_type::value_t lookup(const AbstractState& s) const;

    [[nodiscard]] EvaluationResult evaluate(const GlobalState& s) const;
    [[nodiscard]] EvaluationResult evaluate(const AbstractState& s) const;

    AbstractPolicy get_optimal_abstract_policy(
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        bool wildcard = false) const;

    void dump_graphviz(
        const std::string& path,
        bool transition_labels = true,
        bool values = true) const;

private:
    void compute_value_table(const AbstractStateEvaluator& heuristic);

#ifndef NDEBUG
    void verify(const StateIDMap<AbstractState>& state_id_map);
#endif
};

} // namespace pdbs
} // namespace probabilistic

#endif // __EXPCOST_PROJECTION_H__