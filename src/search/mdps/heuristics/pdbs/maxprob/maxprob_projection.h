#pragma once

#include "../../../globals.h"
#include "../../../value_utils.h"

#include "../../constant_evaluator.h"
#include "../probabilistic_projection.h"

namespace successor_generator {
template <typename T>
class SuccessorGenerator;
}

namespace probabilistic {
namespace pdbs {
namespace maxprob {

class MaxProbProjection : public pdbs::ProbabilisticProjection {
    using RegressionSuccessorGenerator =
        successor_generator::SuccessorGenerator<AbstractState>;

    std::vector<value_utils::IntervalValue> value_table;

    bool all_one = false;
    bool deterministic = false;

    pdbs::QualitativeResultStore one_states;
    pdbs::QualitativeResultStore dead_ends;

    unsigned int n_dead_ends = 0;
    unsigned int n_one_states = 0;

    std::shared_ptr<RegressionSuccessorGenerator> regression_aops_generator_;

public:
    MaxProbProjection(
        const Pattern& pattern,
        const std::vector<int>& domains = ::g_variable_domain,
        const AbstractStateEvaluator& heuristic =
            ConstantEvaluator<AbstractState>(value_type::zero),
        bool precompute_dead_ends = false);

    MaxProbProjection(
        const MaxProbProjection& pdb,
        int add_var,
        bool precompute_dead_ends = false);

    unsigned int num_dead_ends() const;
    unsigned int num_one_states() const;

    bool is_all_one() const;
    bool is_deterministic() const;

    bool is_dead_end(AbstractState s) const;
    bool is_dead_end(const GlobalState& s) const;

    [[nodiscard]] value_type::value_t lookup(AbstractState s) const;
    [[nodiscard]] value_type::value_t lookup(const GlobalState& s) const;

    void dump_graphviz(
        const std::string& path,
        bool transition_labels = true,
        bool values = true);

private:
    void prepare_regression();
    void precompute_dead_ends();

    void compute_value_table(
        const AbstractStateEvaluator& heuristic,
        bool store_dead_ends);
};

} // namespace maxprob
} // namespace pdbs
} // namespace probabilistic
