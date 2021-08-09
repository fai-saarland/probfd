#pragma once

#include "../../../globals.h"
#include "../../../value_utils.h"
#include "../../constant_evaluator.h"
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
    ExpCostProjection(
        const Pattern& variables,
        const std::vector<int>& domains = ::g_variable_domain,
        const AbstractStateEvaluator& heuristic =
            ConstantEvaluator<AbstractState>(value_type::zero));

    ExpCostProjection(const ::pdbs::PatternDatabase& pdb);

    ExpCostProjection(const ExpCostProjection& pdb, int add_var);

    [[nodiscard]] value_type::value_t lookup(const GlobalState& s) const;
    [[nodiscard]] value_type::value_t lookup(const AbstractState& s) const;

    void dump_graphviz(
        const std::string& path,
        bool transition_labels = true,
        bool values = true) const;

private:
    void compute_value_table(const AbstractStateEvaluator& heuristic);
};

} // namespace expected_cost
} // namespace pdbs
} // namespace probabilistic
