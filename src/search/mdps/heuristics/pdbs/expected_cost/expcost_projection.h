#pragma once

#include "../probabilistic_projection.h"

namespace successor_generator {
template<typename T>
class SuccessorGenerator;
}

namespace probabilistic {

class AnalysisObjective;

namespace pdbs {

class ExpCostProjection : public ProbabilisticProjection {
    QuantitativeResultStore value_table;
    unsigned int reachable_states = 0;

public:
    ExpCostProjection(
        const std::vector<int>& variables,
        const std::vector<int>& domains);
    ~ExpCostProjection() = default;

    unsigned int num_reachable_states() const;

    QuantitativeResultStore& get_value_table();
    const QuantitativeResultStore& get_value_table() const;

    [[nodiscard]] value_type::value_t lookup(const GlobalState& s) const;
    [[nodiscard]] value_type::value_t lookup(const AbstractState& s) const;

    void dump_graphviz(
        const std::string& path,
        bool transition_labels = true,
        bool values = true) const;

private:
    void compute_value_table();

    void dump_graphviz_no_values(
        const std::string& path,
        bool transition_labels) const;

    void dump_graphviz_with_values(
        const std::string& path,
        bool transition_labels) const;
};

} // namespace pdbs
} // namespace probabilistic
