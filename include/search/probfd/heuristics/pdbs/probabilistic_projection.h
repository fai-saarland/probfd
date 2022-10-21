#ifndef MDPS_HEURISTICS_PDBS_PROBABILISTIC_PROJECTION_H
#define MDPS_HEURISTICS_PDBS_PROBABILISTIC_PROJECTION_H

#include "probfd/heuristics/pdbs/abstract_operator.h"
#include "probfd/heuristics/pdbs/abstract_state_mapper.h"
#include "probfd/heuristics/pdbs/engine_interfaces.h"
#include "probfd/heuristics/pdbs/qualitative_result_store.h"
#include "probfd/heuristics/pdbs/types.h"

#include "probfd/utils/graph_visualization.h"

#include "probfd/globals.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace successor_generator {
template <typename T>
class SuccessorGenerator;
}

namespace probfd {

class AnalysisObjective;

namespace heuristics {

namespace pdbs {

class ProbabilisticProjection {
    // Footprint used for detecting duplicate operators.
    struct ProgressionOperatorFootprint {
        int precondition_hash;
        std::vector<std::pair<AbstractState, value_type::value_t>> successors;

        ProgressionOperatorFootprint(
            int precondition_hash,
            const AbstractOperator& op)
            : precondition_hash(precondition_hash)
            , successors(op.outcomes.begin(), op.outcomes.end())
        {
            std::sort(successors.begin(), successors.end());
        }

        friend bool operator<(
            const ProgressionOperatorFootprint& a,
            const ProgressionOperatorFootprint& b)
        {
            return std::tie(a.precondition_hash, a.successors) <
                   std::tie(b.precondition_hash, b.successors);
        }

        friend bool operator==(
            const ProgressionOperatorFootprint& a,
            const ProgressionOperatorFootprint& b)
        {
            return std::tie(a.precondition_hash, a.successors) ==
                   std::tie(b.precondition_hash, b.successors);
        }
    };

protected:
    using ProgressionSuccessorGenerator =
        successor_generator::SuccessorGenerator<const AbstractOperator*>;

public:
    ProbabilisticProjection(
        const Pattern& pattern,
        const std::vector<int>& domains,
        bool operator_pruning = true);

    ProbabilisticProjection(
        AbstractStateMapper* mapper,
        bool operator_pruning = true);

    std::shared_ptr<AbstractStateMapper> get_abstract_state_mapper() const;
    std::unordered_set<AbstractState>& get_abstract_goal_states();

    unsigned int num_states() const;

    bool is_dead_end(const GlobalState& s) const;
    bool is_dead_end(const AbstractState& s) const;

    bool is_goal(const AbstractState& s) const;

    AbstractState get_abstract_state(const GlobalState& s) const;
    AbstractState get_abstract_state(const std::vector<int>& s) const;

    // Returns the pattern (i.e. all variables used) of the PDB
    const Pattern& get_pattern() const;

protected:
    template <typename StateToString>
    void dump_graphviz(
        const std::string& path,
        const StateToString& sts,
        bool transition_labels,
        value_type::value_t goal_value) const
    {
        using namespace engine_interfaces;

        AbstractOperatorToString op_names(&g_operators);

        auto ats = [=](const AbstractOperator* op) {
            return transition_labels ? op_names(op) : "";
        };

        ZeroCostAbstractRewardFunction reward(
            &goal_states_,
            goal_value,
            value_type::zero);

        StateIDMap<AbstractState> state_id_map;

        TransitionGenerator<const AbstractOperator*> transition_gen(
            state_id_map,
            state_mapper_,
            progression_aops_generator_);

        std::ofstream out(path);

        graphviz::dump<AbstractState>(
            out,
            initial_state_,
            &state_id_map,
            &reward,
            &transition_gen,
            sts,
            ats,
            nullptr,
            true);
    }

private:
    void setup_abstract_goal();
    void build_operators(bool operator_pruning);

    void add_abstract_operators(
        const ProbabilisticOperator& op,
        std::set<ProgressionOperatorFootprint>& duplicate_set,
        std::vector<std::vector<std::pair<int, int>>>&
            progression_preconditions,
        bool operator_pruining);

protected:
    std::shared_ptr<AbstractStateMapper> state_mapper_;

    AbstractState initial_state_;
    std::unordered_set<AbstractState> goal_states_;
    std::vector<AbstractOperator> abstract_operators_;
    std::shared_ptr<ProgressionSuccessorGenerator> progression_aops_generator_;

    std::vector<StateID> dead_ends_;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __PROBABILISTIC_PROJECTION_H__