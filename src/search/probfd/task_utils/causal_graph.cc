#include "probfd/task_utils/causal_graph.h"

#include "probfd/probabilistic_task.h"
#include "probfd/task_proxy.h"

#include "downward/utils/logging.h"
#include "downward/utils/memory.h"
#include "downward/utils/timer.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <unordered_set>

using namespace std;

namespace probfd::causal_graph {

static std::unordered_map<
    const ProbabilisticTask*,
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

class IntRelationBuilder {
    typedef std::unordered_set<int> IntSet;
    vector<IntSet> int_sets;

    int get_range() const;

public:
    explicit IntRelationBuilder(int range);

    void add_pair(int u, int v);
    void compute_relation(IntRelation& result) const;
};

IntRelationBuilder::IntRelationBuilder(int range)
    : int_sets(range)
{
}

int IntRelationBuilder::get_range() const
{
    return int_sets.size();
}

void IntRelationBuilder::add_pair(int u, int v)
{
    assert(u >= 0 && u < get_range());
    assert(v >= 0 && v < get_range());
    int_sets[u].insert(v);
}

void IntRelationBuilder::compute_relation(IntRelation& result) const
{
    int range = get_range();
    result.clear();
    result.resize(range);
    for (int i = 0; i < range; ++i) {
        result[i].assign(int_sets[i].begin(), int_sets[i].end());
        sort(result[i].begin(), result[i].end());
    }
}

struct ProbabilisticCausalGraphBuilder {
    IntRelationBuilder pre_eff_builder;
    IntRelationBuilder eff_pre_builder;
    IntRelationBuilder eff_eff_builder;

    IntRelationBuilder succ_builder;
    IntRelationBuilder pred_builder;

    explicit ProbabilisticCausalGraphBuilder(int var_count)
        : pre_eff_builder(var_count)
        , eff_pre_builder(var_count)
        , eff_eff_builder(var_count)
        , succ_builder(var_count)
        , pred_builder(var_count)
    {
    }

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

    void handle_operator(const ProbabilisticOperatorProxy& op)
    {
        auto outcomes = op.get_outcomes();

        // Handle pre->eff links from preconditions.
        for (FactProxy pre : op.get_preconditions()) {
            int pre_var_id = pre.get_variable().get_id();
            for (auto outcome : outcomes) {
                for (ProbabilisticEffectProxy eff : outcome.get_effects()) {
                    int eff_var_id = eff.get_fact().get_variable().get_id();
                    if (pre_var_id != eff_var_id)
                        handle_pre_eff_arc(pre_var_id, eff_var_id);
                }
            }
        }

        std::set<int> eff_vars;

        // Handle pre->eff links from effect conditions.
        for (auto outcome : outcomes) {
            for (ProbabilisticEffectProxy eff : outcome.get_effects()) {
                VariableProxy eff_var = eff.get_fact().get_variable();
                int eff_var_id = eff_var.get_id();
                eff_vars.insert(eff_var_id);
                for (FactProxy pre : eff.get_conditions()) {
                    int pre_var_id = pre.get_variable().get_id();
                    if (pre_var_id != eff_var_id)
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
        for (FactProxy pre : op.get_preconditions()) {
            int pre_var_id = pre.get_variable().get_id();
            for (EffectProxy eff : effects) {
                int eff_var_id = eff.get_fact().get_variable().get_id();
                if (pre_var_id != eff_var_id)
                    handle_pre_eff_arc(pre_var_id, eff_var_id);
            }
        }

        // Handle pre->eff links from effect conditions.
        for (EffectProxy eff : effects) {
            VariableProxy eff_var = eff.get_fact().get_variable();
            int eff_var_id = eff_var.get_id();
            for (FactProxy pre : eff.get_conditions()) {
                int pre_var_id = pre.get_variable().get_id();
                if (pre_var_id != eff_var_id)
                    handle_pre_eff_arc(pre_var_id, eff_var_id);
            }
        }

        // Handle eff->eff links.
        for (size_t i = 0; i < effects.size(); ++i) {
            int eff1_var_id = effects[i].get_fact().get_variable().get_id();
            for (size_t j = i + 1; j < effects.size(); ++j) {
                int eff2_var_id = effects[j].get_fact().get_variable().get_id();
                if (eff1_var_id != eff2_var_id)
                    handle_eff_eff_edge(eff1_var_id, eff2_var_id);
            }
        }
    }
};

ProbabilisticCausalGraph::ProbabilisticCausalGraph(
    const ProbabilisticTaskProxy& task_proxy)
{
    int num_variables = task_proxy.get_variables().size();
    ProbabilisticCausalGraphBuilder cg_builder(num_variables);

    for (ProbabilisticOperatorProxy op : task_proxy.get_operators())
        cg_builder.handle_operator(op);

    for (AxiomProxy op : task_proxy.get_axioms())
        cg_builder.handle_operator(op);

    cg_builder.pre_eff_builder.compute_relation(pre_to_eff);
    cg_builder.eff_pre_builder.compute_relation(eff_to_pre);
    cg_builder.eff_eff_builder.compute_relation(eff_to_eff);

    cg_builder.pred_builder.compute_relation(predecessors);
    cg_builder.succ_builder.compute_relation(successors);
}

void ProbabilisticCausalGraph::dump(
    const ProbabilisticTaskProxy& task_proxy,
    utils::LogProxy& log) const
{
    log << "Causal graph: " << endl;
    for (VariableProxy var : task_proxy.get_variables()) {
        int var_id = var.get_id();
        log << "#" << var_id << " [" << var.get_name() << "]:" << endl
            << "    pre->eff arcs: " << pre_to_eff[var_id] << endl
            << "    eff->pre arcs: " << eff_to_pre[var_id] << endl
            << "    eff->eff arcs: " << eff_to_eff[var_id] << endl
            << "    successors: " << successors[var_id] << endl
            << "    predecessors: " << predecessors[var_id] << endl;
    }
}

const ProbabilisticCausalGraph& get_causal_graph(const ProbabilisticTask* task)
{
    if (causal_graph_cache.count(task) == 0) {
        ProbabilisticTaskProxy task_proxy(*task);
        causal_graph_cache.insert(make_pair(
            task,
            std::make_unique<ProbabilisticCausalGraph>(task_proxy)));
    }
    return *causal_graph_cache[task];
}

} // namespace probfd::causal_graph
