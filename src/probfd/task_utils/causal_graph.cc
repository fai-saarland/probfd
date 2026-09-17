#include "probfd/task_utils/causal_graph.h"

#include "probfd/merge_and_shrink/factored_transition_system.h"

#include "probfd/probabilistic_operator_space.h"
#include "probfd/probabilistic_task.h"

#include "downward/axiom_space.h"
#include "downward/variable_space.h"

#include "downward/utils/logging.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <set>
#include <unordered_map>
#include <unordered_set>

using namespace std;

using namespace downward;

namespace probfd::causal_graph {

static utils::HashMap<
    std::tuple<
        const VariableSpace*,
        const AxiomSpace*,
        const ProbabilisticOperatorSpace*>,
    std::unique_ptr<ProbabilisticCausalGraph>>
    causal_graph_cache;

/*
  An IntRelationBuilder constructs an IntRelation by adding one pair
  to the relation at a time. Duplicates are automatically handled
  (i.e., it is OK to add the same pair twice), and the pairs need not
  be added in any specific sorted order.

  Define the following parameters:
  - K: range of the IntRelation (i.e., allowed values {0, ..., K - 1})
  - M: number of pairs added to the relation (including duplicates)
  - N: number of unique pairs in the final relation
  - D: maximal number of unique elements (x, y) in the relation for given x

  Then we get:
  - O(K + N) memory usage during construction and for final IntRelation
  - O(K + M + N log D) construction time
*/

namespace {
class IntRelationBuilder {
    typedef std::unordered_set<int> IntSet;
    vector<IntSet> int_sets;

    std::size_t get_range() const;

public:
    explicit IntRelationBuilder(std::size_t range);

    void add_pair(int u, int v);
    void compute_relation(IntRelation& result) const;
};

IntRelationBuilder::IntRelationBuilder(std::size_t range)
    : int_sets(range)
{
}

std::size_t IntRelationBuilder::get_range() const
{
    return int_sets.size();
}

void IntRelationBuilder::add_pair(int u, int v)
{
    assert(u >= 0 && std::cmp_less(u, get_range()));
    assert(v >= 0 && std::cmp_less(v, get_range()));
    int_sets[u].insert(v);
}

void IntRelationBuilder::compute_relation(IntRelation& result) const
{
    const std::size_t range = get_range();
    result.clear();
    result.resize(range);
    for (std::size_t i = 0; i < range; ++i) {
        result[i].assign_range(int_sets[i]);
        ranges::sort(result[i]);
    }
}

struct ProbabilisticCausalGraphBuilder {
    IntRelationBuilder pre_eff_builder;
    IntRelationBuilder eff_pre_builder;
    IntRelationBuilder eff_eff_builder;

    IntRelationBuilder succ_builder;
    IntRelationBuilder pred_builder;

    explicit ProbabilisticCausalGraphBuilder(std::size_t var_count)
        : pre_eff_builder(var_count)
        , eff_pre_builder(var_count)
        , eff_eff_builder(var_count)
        , succ_builder(var_count)
        , pred_builder(var_count)
    {
    }

    void handle_operator(const ProbabilisticOperatorProxy& op)
    {
        auto outcomes = op.get_outcomes();

        // Handle pre->eff links from preconditions.
        for (const FactPair pre : op.get_preconditions()) {
            const int pre_var_id = pre.var;
            for (auto outcome : outcomes) {
                for (ProbabilisticEffectProxy eff : outcome.get_effects()) {
                    if (const int eff_var_id = eff.get_fact().var;
                        pre_var_id != eff_var_id)
                        handle_pre_eff_arc(pre_var_id, eff_var_id);
                }
            }
        }

        std::set<int> eff_vars;

        // Handle pre->eff links from effect conditions.
        for (auto outcome : outcomes) {
            for (ProbabilisticEffectProxy eff : outcome.get_effects()) {
                int eff_var_id = eff.get_fact().var;
                eff_vars.insert(eff_var_id);
                for (const FactPair pre : eff.get_conditions()) {
                    if (const int pre_var_id = pre.var;
                        pre_var_id != eff_var_id)
                        handle_pre_eff_arc(pre_var_id, eff_var_id);
                }
            }
        }

        // Handle eff->eff links.
        for (auto it = eff_vars.begin(); it != eff_vars.end(); ++it) {
            for (auto it2 = std::next(it); it2 != eff_vars.end(); ++it2) {
                handle_eff_eff_edge(*it, *it2);
            }
        }
    }

    void handle_operator(const AxiomProxy& op)
    {
        auto effects = op.get_effects();

        // Handle pre->eff links from preconditions.
        for (const FactPair pre : op.get_preconditions()) {
            const int pre_var_id = pre.var;
            for (auto eff : effects) {
                const int eff_var_id = eff.get_fact().var;
                if (pre_var_id != eff_var_id)
                    handle_pre_eff_arc(pre_var_id, eff_var_id);
            }
        }

        // Handle pre->eff links from effect conditions.
        for (auto eff : effects) {
            const int eff_var_id = eff.get_fact().var;
            for (const FactPair pre : eff.get_conditions()) {
                const int pre_var_id = pre.var;
                if (pre_var_id != eff_var_id)
                    handle_pre_eff_arc(pre_var_id, eff_var_id);
            }
        }

        // Handle eff->eff links.
        for (size_t i = 0; i < effects.size(); ++i) {
            const int eff1_var_id = effects[i].get_fact().var;
            for (size_t j = i + 1; j < effects.size(); ++j) {
                const int eff2_var_id = effects[j].get_fact().var;
                if (eff1_var_id != eff2_var_id)
                    handle_eff_eff_edge(eff1_var_id, eff2_var_id);
            }
        }
    }

    void handle_operator(
        const std::set<int>& pre_factors,
        const std::set<int>& state_change_factors)
    {
        // Handle pre->eff links from preconditions.
        for (const int pre_var_id : pre_factors) {
            for (const int eff_var_id : state_change_factors) {
                if (pre_var_id != eff_var_id) {
                    handle_pre_eff_arc(pre_var_id, eff_var_id);
                }
            }
        }

        // Handle eff->eff links.
        for (auto it = state_change_factors.begin();
             it != state_change_factors.end();
             ++it) {
            for (auto it2 = std::next(it); it2 != state_change_factors.end();
                 ++it2) {
                handle_eff_eff_edge(*it, *it2);
            }
        }
    }

private:
    void handle_pre_eff_arc(int u, int v)
    {
        assert(u != v);
        pre_eff_builder.add_pair(u, v);
        succ_builder.add_pair(u, v);
        eff_pre_builder.add_pair(v, u);
        pred_builder.add_pair(v, u);
    }

    void handle_eff_eff_edge(int u, int v)
    {
        assert(u != v);
        eff_eff_builder.add_pair(u, v);
        eff_eff_builder.add_pair(v, u);
        succ_builder.add_pair(u, v);
        succ_builder.add_pair(v, u);
        pred_builder.add_pair(u, v);
        pred_builder.add_pair(v, u);
    }
};
} // namespace

ProbabilisticCausalGraph::ProbabilisticCausalGraph(
    const VariableSpace& variables,
    const AxiomSpace& axioms,
    const ProbabilisticOperatorSpace& operators)
{
    const std::size_t num_variables = variables.size();
    ProbabilisticCausalGraphBuilder cg_builder(num_variables);

    for (ProbabilisticOperatorProxy op : operators)
        cg_builder.handle_operator(op);

    for (AxiomProxy op : axioms) cg_builder.handle_operator(op);

    cg_builder.pre_eff_builder.compute_relation(pre_to_eff);
    cg_builder.eff_pre_builder.compute_relation(eff_to_pre);
    cg_builder.eff_eff_builder.compute_relation(eff_to_eff);

    cg_builder.pred_builder.compute_relation(predecessors);
    cg_builder.succ_builder.compute_relation(successors);
}

ProbabilisticCausalGraph::ProbabilisticCausalGraph(
    const merge_and_shrink::FactoredTransitionSystem& fts)
{
    assert(fts.get_num_active_entries() == fts.get_size());

    const std::size_t num_labels = fts.get_labels().get_num_total_labels();

    std::vector<std::set<int>> lbl_to_pre_factors(num_labels);
    std::vector<std::set<int>> lbl_to_state_change_factors(num_labels);

    for (int factor_idx : fts) {
        const auto& factor = fts.get_transition_system(factor_idx);
        for (const auto& local_label_info : factor.label_infos()) {
            std::unordered_set<int> source_states;
            for (const auto& transition : local_label_info.get_transitions()) {
                source_states.insert(transition.src);
                for (const int target : transition.targets) {
                    if (target != transition.src) {
                        for (const int label :
                             local_label_info.get_label_group()) {
                            lbl_to_state_change_factors[label].insert(
                                factor_idx);
                        }
                    }
                }
            }

            if (source_states.size() != factor.get_size()) {
                for (const int label : local_label_info.get_label_group()) {
                    lbl_to_pre_factors[label].insert(factor_idx);
                }
            }
        }
    }

    ProbabilisticCausalGraphBuilder cg_builder(fts.get_size());

    for (const auto& [pre_factors, state_change_factors] :
         std::views::zip(lbl_to_pre_factors, lbl_to_state_change_factors)) {
        cg_builder.handle_operator(pre_factors, state_change_factors);
    }

    cg_builder.pre_eff_builder.compute_relation(pre_to_eff);
    cg_builder.eff_pre_builder.compute_relation(eff_to_pre);
    cg_builder.eff_eff_builder.compute_relation(eff_to_eff);

    cg_builder.pred_builder.compute_relation(predecessors);
    cg_builder.succ_builder.compute_relation(successors);
}

void ProbabilisticCausalGraph::dump(
    const VariableSpace& variables,
    utils::LogProxy& log) const
{
    log.println("Causal graph:");
    for (VariableProxy var : variables) {
        int var_id = var.get_id();
        log.println(
            "#{} [{}]:\n"
            "    pre->eff arcs: {}\n"
            "    eff->pre arcs: {}\n"
            "    eff->eff arcs: {}\n"
            "    successors: {}\n"
            "    predecessors: {}",
            var_id,
            var.get_name(),
            pre_to_eff[var_id],
            eff_to_pre[var_id],
            eff_to_eff[var_id],
            successors[var_id],
            predecessors[var_id]);
    }
}

const ProbabilisticCausalGraph& get_causal_graph(
    const VariableSpace& variables,
    const AxiomSpace& axioms,
    const ProbabilisticOperatorSpace& operators)
{
    const std::tuple<
        const VariableSpace*,
        const AxiomSpace*,
        const ProbabilisticOperatorSpace*>
        entry = map_tuple(
            std::forward_as_tuple(variables, axioms, operators),
            [](auto&& arg) { return &arg; });

    if (!causal_graph_cache.contains(entry)) {
        causal_graph_cache.emplace(
            entry,
            std::make_unique<ProbabilisticCausalGraph>(
                variables,
                axioms,
                operators));
    }
    return *causal_graph_cache[entry];
}

} // namespace probfd::causal_graph
