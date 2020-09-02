#pragma once

#include "../fact_set.h"
#include "../state_component_listener.h"
#include "delta_trap_heuristic.h"

#if TRAP_FORMULA_AS_UB_TREE
#include "../algorithms/ub_tree.h"
#endif

#include <memory>
#include <vector>

class Heuristic;
class PartialState;

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace strips_utils {
class STRIPS;
}

namespace traps {

class DeltaTrapHeuristicRefinement
    : public state_component::StateComponentListener {
public:
    DeltaTrapHeuristicRefinement(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);

    virtual bool requires_recognized_neighbors_implicitly() const override;

protected:
    virtual Status on_new_component(
        state_component::StateComponent&,
        state_component::StateComponent&) override;
    virtual void statistics() const override;

    bool
    is_covered(const strips_utils::FactSet& fact_set, PartialState& partial_state);
    bool in_component(const strips_utils::FactSet& fact_set);
    bool is_dead_end(const PartialState& partial_state);

    std::shared_ptr<strips_utils::STRIPS> task_;
    std::shared_ptr<DeltaTrapHeuristic> trap_;
    std::vector<std::shared_ptr<Heuristic>> deltas_;

    std::vector<bool> in_goal_;
    std::vector<strips_utils::Fact> goal_facts_;
    strips_utils::FactIndexedVector<std::vector<unsigned>> actions_with_fact_context_;

#if TRAP_FORMULA_AS_UB_TREE
    ub_tree::UBTree<strips_utils::Fact, bool> component_;
#else
    strips_utils::FactSetSet component_;
#endif
    // std::vector<int> variable_order_;
};

} // namespace traps
