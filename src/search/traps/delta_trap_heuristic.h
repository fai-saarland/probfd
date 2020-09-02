#pragma once

#define TRAP_FORMULA_AS_UB_TREE 0

#include "../fact_set.h"
#include "../heuristic.h"

#if TRAP_FORMULA_AS_UB_TREE
#include "../algorithms/ub_tree.h"
#endif

#include <memory>
#include <vector>

namespace strips_utils {
class STRIPS;
}

namespace traps {

class DeltaTrapHeuristic : public Heuristic {
public:
    DeltaTrapHeuristic(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);

    void print_statistics() const;

    virtual bool supports_partial_state_evaluation() const override;

    bool add_conjunction_to_formula(const strips_utils::FactSet& fact_set);

    bool is_satisfied(const strips_utils::FactSet& fact_set);

protected:
    virtual int compute_heuristic(const GlobalState& state) override;
    virtual int compute_heuristic(const PartialState& partial_state) override;

private:
    void
    build_k_trap(int k, const std::vector<std::shared_ptr<Heuristic>>& deltas);

    const bool prune_transitions_;
    const bool delta_prune_states_;
    const bool delta_prune_transitions_;
    const bool cache_delta_estimates_;

    std::shared_ptr<strips_utils::STRIPS> fact_map_;
#if TRAP_FORMULA_AS_UB_TREE
    ub_tree::UBTree<strips_utils::Fact, bool> conjunctions_;
#else
    strips_utils::FactSetSet conjunctions_;
#endif
};

} // namespace traps
