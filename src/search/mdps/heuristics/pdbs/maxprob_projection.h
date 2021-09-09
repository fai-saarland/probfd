#pragma once

#include "../../globals.h"
#include "../../value_utils.h"

#include "../constant_evaluator.h"
#include "probabilistic_projection.h"

namespace successor_generator {
template <typename T>
class SuccessorGenerator;
}

namespace pdbs {
class PatternDatabase;
}

namespace probabilistic {
namespace pdbs {

class MaxProbProjection : public pdbs::ProbabilisticProjection {
    std::vector<value_utils::IntervalValue> value_table;

    bool all_one = false;
    bool deterministic = false;

    unsigned int n_dead_ends = 0;
    unsigned int n_one_states = 0;

public:
    MaxProbProjection(
        const Pattern& pattern,
        const std::vector<int>& domains = ::g_variable_domain,
        const AbstractStateEvaluator& heuristic =
            ConstantEvaluator<AbstractState>(value_type::one),
        bool precompute_dead_ends = false);

    MaxProbProjection(
        const ::pdbs::PatternDatabase& pdb,
        bool precompute_dead_ends = false);

    MaxProbProjection(
        const MaxProbProjection& pdb,
        int add_var,
        bool precompute_dead_ends = false);

    unsigned int num_dead_ends() const;
    unsigned int num_one_states() const;

    bool is_all_one() const;
    bool is_deterministic() const;

    bool is_dead_end(const GlobalState& s) const;
    bool is_dead_end(const AbstractState& s) const;

    [[nodiscard]] value_type::value_t lookup(const GlobalState& s) const;
    [[nodiscard]] value_type::value_t lookup(const AbstractState& s) const;

    [[nodiscard]] EvaluationResult evaluate(const GlobalState& s) const;
    [[nodiscard]] EvaluationResult evaluate(const AbstractState& s) const;

    void dump_graphviz(
        const std::string& path,
        bool transition_labels = true,
        bool values = true);

private:
    void initialize(
        const AbstractStateEvaluator& heuristic,
        bool precompute_dead_ends);

    void compute_value_table(
        const AbstractStateEvaluator& heuristic,
        bool store_dead_ends);
};

} // namespace pdbs
} // namespace probabilistic
