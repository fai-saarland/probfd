#pragma once

#include "../fact_set.h"
#include "critical_path_heuristic_refinement.h"

#include <deque>
#include <vector>

class GlobalState;

namespace critical_path_heuristic {

class ConjunctionInfo;
class CounterInfo;

class CriticalPathHeuristicNeighborsRefinement
    : public CriticalPathHeuristicRefinement {
public:
    CriticalPathHeuristicNeighborsRefinement(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);

    virtual bool requires_recognized_neighbors_explicitly() const override;

protected:
    virtual bool refine_conjunction_set(
        state_component::StateComponent& states,
        state_component::StateComponent& neighbors) override;

    virtual std::string name() const;

    struct RefinementNode {
        explicit RefinementNode(const ConjunctionInfo* conjunction);
        const ConjunctionInfo* conjunction;
        int achiever_index;
    };

    bool initialize_current_state_reachable(const GlobalState& state);
    void
    store_current_states_fact_relation(state_component::StateComponent& states);
    void store_neighbors_unreachable_conjunctions(
        state_component::StateComponent& neighbors);
    void synchronize_reachability();
    bool push_for_refinement(const strips_utils::FactSet& subgoal);
    std::vector<unsigned> get_neighbor_covering(const strips_utils::FactSet& subgoal);
    std::vector<strips_utils::Fact> get_inverse_state_covering(
        const strips_utils::FactSet& subgoal,
        const strips_utils::FactSet& conflict);
    bool process();

    unsigned state_component_size_;
    unsigned num_neighbors_;
    unsigned last_num_conjunctions_;
    strips_utils::FactIndexedVector<std::vector<unsigned>> inverse_fact_to_state_;
    std::vector<bool> current_states_reachability_;
    std::vector<std::vector<unsigned>> conjunction_to_neighbor_;
    std::deque<RefinementNode> refinement_queue_;
};

} // namespace critical_path_heuristic
