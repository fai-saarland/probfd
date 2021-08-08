#pragma once

#include "../../../value_utils.h"
#include "../probabilistic_projection.h"

namespace successor_generator {
template <typename T>
class SuccessorGenerator;
}

namespace pdbs {
class PatternDatabase;
}

namespace probabilistic {
namespace pdbs {
namespace expected_cost {

class ExpCostProjection : public pdbs::ProbabilisticProjection {
    std::vector<value_type::value_t> value_table;

public:
    ExpCostProjection(const Pattern& variables);

    ExpCostProjection(
        const Pattern& variables,
        const std::vector<int>& domains);

    ExpCostProjection(
        const Pattern& variables,
        AbstractStateEvaluator* heuristic);

    ExpCostProjection(
        const Pattern& variables,
        const std::vector<int>& domains,
        AbstractStateEvaluator* heuristic);

    ExpCostProjection(const ::pdbs::PatternDatabase& pdb);

    ExpCostProjection(const ExpCostProjection& pdb, int add_var);

    [[nodiscard]] value_type::value_t lookup(const GlobalState& s) const;
    [[nodiscard]] value_type::value_t lookup(const AbstractState& s) const;

    void dump_graphviz(
        const std::string& path,
        bool transition_labels = true,
        bool values = true) const;

private:
    void compute_value_table(AbstractStateEvaluator* heuristic = nullptr);

    template <typename StateToString, typename ActionToString>
    void dump_graphviz(
        const std::string& path,
        const StateToString* sts,
        const ActionToString* ats) const;
};

} // namespace expected_cost
} // namespace pdbs
} // namespace probabilistic
