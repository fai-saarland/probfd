#ifndef PROBFD_HEURISTICS_PDBS_PROBABILISTIC_PROJECTION_H
#define PROBFD_HEURISTICS_PDBS_PROBABILISTIC_PROJECTION_H

#include "probfd/heuristics/pdbs/abstract_operator.h"
#include "probfd/heuristics/pdbs/engine_interfaces.h"
#include "probfd/heuristics/pdbs/match_tree.h"
#include "probfd/heuristics/pdbs/state_ranking_function.h"
#include "probfd/heuristics/pdbs/types.h"

#include "probfd/utils/graph_visualization.h"

#include "probfd/tasks/root_task.h"

#include "probfd/task_proxy.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <vector>

class State;

namespace probfd {
namespace heuristics {
namespace pdbs {

class MatchTree;

class ProbabilisticProjection {
    struct StateRankSpace {
        StateRank initial_state_;
        std::vector<AbstractOperator> abstract_operators_;
        MatchTree match_tree_;
        std::vector<bool> goal_state_flags_;

        StateRankSpace(
            const ProbabilisticTaskProxy& task_proxy,
            const StateRankingFunction& mapper,
            bool operator_pruning = true);

        void setup_abstract_goal(
            const ProbabilisticTaskProxy& task_proxy,
            const StateRankingFunction& mapper);
        bool is_goal(const StateRank& s) const;
    };

protected:
    std::shared_ptr<StateRankingFunction> state_mapper_;
    StateRankSpace abstract_state_space_;
    std::vector<StateID> dead_ends_;
    std::vector<value_type::value_t> value_table;

protected:
    ProbabilisticProjection(
        const ProbabilisticTaskProxy& task_proxy,
        const Pattern& pattern,
        bool operator_pruning,
        value_type::value_t fill);

    ProbabilisticProjection(
        const ProbabilisticTaskProxy& task_proxy,
        StateRankingFunction* mapper,
        bool operator_pruning,
        value_type::value_t fill);

public:
    std::shared_ptr<StateRankingFunction> get_abstract_state_mapper() const;

    unsigned int num_states() const;

    bool is_dead_end(const State& s) const;
    bool is_dead_end(const StateRank& s) const;

    bool is_goal(const StateRank& s) const;

    StateRank get_abstract_state(const State& s) const;
    StateRank get_abstract_state(const std::vector<int>& s) const;

    [[nodiscard]] value_type::value_t lookup(const State& s) const;
    [[nodiscard]] value_type::value_t lookup(const StateRank& s) const;

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

        ProbabilisticTaskProxy task_proxy(*tasks::g_root_task);
        AbstractOperatorToString op_names(task_proxy);

        auto ats = [=](const AbstractOperator* op) {
            return transition_labels ? op_names(op) : "";
        };

        StateIDMap<StateRank> state_id_map;

        TransitionGenerator<const AbstractOperator*> transition_gen(
            state_id_map,
            abstract_state_space_.match_tree_);

        std::ofstream out(path);

        graphviz::dump<StateRank>(
            out,
            abstract_state_space_.initial_state_,
            &state_id_map,
            &rewards,
            &transition_gen,
            sts,
            ats,
            nullptr,
            true);
        }
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __PROBABILISTIC_PROJECTION_H__