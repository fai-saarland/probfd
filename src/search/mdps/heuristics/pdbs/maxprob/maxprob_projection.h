#pragma once

#include "../probabilistic_projection.h"

namespace successor_generator {
template<typename T>
class SuccessorGenerator;
}

namespace probabilistic {

class AnalysisObjective;

namespace pdbs {

class MaxProbProjection : public ProbabilisticProjection {
    using RegressionSuccessorGenerator =
        successor_generator::SuccessorGenerator<AbstractState>;
    
    bool all_one = false;
    bool deterministic = false;

    QuantitativeResultStore value_table;
    QualitativeResultStore one_states;
    QualitativeResultStore dead_ends;

    unsigned int n_reachable_states = 0;
    unsigned int n_dead_ends = 0;
    unsigned int n_one_states = 0;

    std::shared_ptr<RegressionSuccessorGenerator> regression_aops_generator_;

public:
    MaxProbProjection(
        const Pattern& pattern,
        const std::vector<int>& domains,
        bool precompute_dead_ends = false);

    ~MaxProbProjection() = default;

    AbstractState get_abstract_state(const GlobalState& s) const;

    unsigned int num_reachable_states() const;
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

    void compute_value_table(bool precomputed_dead_ends);

    template<typename StateToString, typename ActionToString>
    void dump_graphviz(
        const std::string& path,
        const StateToString* sts,
        const ActionToString* ats);
};

} // namespace pdbs
} // namespace probabilistic
