#ifndef MDPS_HEURISTICS_PDBS_PROBABILISTIC_PROJECTION_H
#define MDPS_HEURISTICS_PDBS_PROBABILISTIC_PROJECTION_H

#include "abstract_operator.h"
#include "abstract_state_mapper.h"
#include "engine_interfaces.h"
#include "qualitative_result_store.h"
#include "types.h"

#include "../../utils/graph_visualization.h"

#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <fstream>

namespace successor_generator {
template <typename T>
class SuccessorGenerator;
}

namespace probabilistic {

class AnalysisObjective;

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

    using RegressionSuccessorGenerator =
        successor_generator::SuccessorGenerator<AbstractState>;

public:
    ProbabilisticProjection(
        const Pattern& pattern,
        const std::vector<int>& domains);

    std::shared_ptr<AbstractStateMapper> get_abstract_state_mapper() const;
    std::unordered_set<AbstractState>& get_abstract_goal_states();

    unsigned int num_states() const;

    AbstractState get_abstract_state(const GlobalState& s) const;

    // Returns the pattern (i.e. all variables used) of the PDB
    const Pattern& get_pattern() const;

    unsigned int num_reachable_states() const;

protected:
    void prepare_regression();
    void precompute_dead_ends();

    template <typename StateToString, typename ActionToString>
    void dump_graphviz(
        const std::string& path,
        const StateToString* sts,
        const ActionToString* ats,
        value_type::value_t goal_value) const
    {
        AbstractStateInSetRewardFunction state_reward(
            &goal_states_,
            goal_value,
            value_type::zero);

        StateIDMap<AbstractState> state_id_map;

        ApplicableActionsGenerator<const AbstractOperator*> aops_gen(
            state_id_map,
            state_mapper_,
            progression_aops_generator_);
        TransitionGenerator<const AbstractOperator*> transition_gen(
            state_id_map,
            state_mapper_,
            progression_aops_generator_);

        std::ofstream out(path);

        graphviz::dump(
            out,
            initial_state_,
            &state_id_map,
            &state_reward,
            &aops_gen,
            &transition_gen,
            sts,
            ats,
            true);
    }

private:
    void setup_abstract_goal();
    void prepare_progression();
    void add_abstract_operators(
        const Pattern& pattern,
        const ProbabilisticOperator& op,
        std::set<ProgressionOperatorFootprint>& duplicate_set,
        std::vector<std::vector<std::pair<int, int>>>& preconditions);

protected:
    std::vector<int> var_index_;
    std::shared_ptr<AbstractStateMapper> state_mapper_;
    std::vector<std::pair<int, int>> sparse_goal_;
    AbstractState initial_state_;
    std::unordered_set<AbstractState> goal_states_;

    std::vector<AbstractOperator> abstract_operators_;
    std::shared_ptr<ProgressionSuccessorGenerator> progression_aops_generator_;
    std::shared_ptr<RegressionSuccessorGenerator> regression_aops_generator_;

    unsigned int reachable_states = 0;

    pdbs::QualitativeResultStore one_states;
    pdbs::QualitativeResultStore dead_ends;
};

} // namespace pdbs
} // namespace probabilistic

#endif // __PROBABILISTIC_PROJECTION_H__