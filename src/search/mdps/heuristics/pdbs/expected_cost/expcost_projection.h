#pragma once

#include "../probabilistic_projection.h"
#include "../../../value_utils.h"

namespace successor_generator {
template<typename T>
class SuccessorGenerator;
}

namespace pdbs {
class PatternDatabase;
}

namespace probabilistic {

class AnalysisObjective;

namespace pdbs {

class ExpCostProjection : public ProbabilisticProjection {
    std::vector<value_utils::SingleValue> value_table;
    unsigned int reachable_states = 0;

public:
    ExpCostProjection(
        const Pattern& variables,
        AbstractStateEvaluator* heuristic = nullptr);
    
    ExpCostProjection(
        const Pattern& variables,
        const std::vector<int>& domains,
        AbstractStateEvaluator* heuristic = nullptr);

    ExpCostProjection(const ::pdbs::PatternDatabase& pdb);

    ~ExpCostProjection() = default;

    value_type::value_t get_value(const GlobalState &state) const;

    unsigned int num_reachable_states() const;

    [[nodiscard]] value_type::value_t lookup(const GlobalState& s) const;
    [[nodiscard]] value_type::value_t lookup(const AbstractState& s) const;

    void dump_graphviz(
        const std::string& path,
        bool transition_labels = true,
        bool values = true) const;

private:
    void compute_value_table(AbstractStateEvaluator* heuristic = nullptr);

    template<typename StateToString, typename ActionToString>
    void dump_graphviz(
        const std::string& path,
        const StateToString* sts,
        const ActionToString* ats) const;
};

} // namespace pdbs
} // namespace probabilistic
