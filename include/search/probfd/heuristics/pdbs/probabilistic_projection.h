#ifndef MDPS_HEURISTICS_PDBS_PROBABILISTIC_PROJECTION_H
#define MDPS_HEURISTICS_PDBS_PROBABILISTIC_PROJECTION_H

#include "probfd/heuristics/pdbs/abstract_operator.h"
#include "probfd/heuristics/pdbs/abstract_state_mapper.h"
#include "probfd/heuristics/pdbs/engine_interfaces.h"
#include "probfd/heuristics/pdbs/match_tree.h"
#include "probfd/heuristics/pdbs/types.h"

#include "probfd/utils/graph_visualization.h"

#include "probfd/globals.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace probfd {

class AnalysisObjective;

namespace heuristics {

namespace pdbs {

class MatchTree;

class ProbabilisticProjection {
protected:
    std::shared_ptr<AbstractStateMapper> state_mapper_;

    AbstractState initial_state_;
    std::unordered_set<AbstractState> goal_states_;
    std::vector<AbstractOperator> abstract_operators_;

    MatchTree match_tree_;

    std::vector<StateID> dead_ends_;

    std::vector<value_type::value_t> value_table;

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

    [[nodiscard]] value_type::value_t lookup(const GlobalState& s) const;
    [[nodiscard]] value_type::value_t lookup(const AbstractState& s) const;

    // Returns the pattern (i.e. all variables used) of the PDB
    const Pattern& get_pattern() const;

protected:
    template <typename StateToString>
    void dump_graphviz(
        const std::string& path,
        const StateToString& sts,
        AbstractRewardFunction& rewards,
        bool transition_labels) const
    {
        using namespace engine_interfaces;

        AbstractOperatorToString op_names(&g_operators);

        auto ats = [=](const AbstractOperator* op) {
            return transition_labels ? op_names(op) : "";
        };

        StateIDMap<AbstractState> state_id_map;

        TransitionGenerator<const AbstractOperator*> transition_gen(
            state_id_map,
            match_tree_);

        std::ofstream out(path);

        graphviz::dump<AbstractState>(
            out,
            initial_state_,
            &state_id_map,
            &rewards,
            &transition_gen,
            sts,
            ats,
            nullptr,
            true);
    }

private:
    void setup_abstract_goal();
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __PROBABILISTIC_PROJECTION_H__