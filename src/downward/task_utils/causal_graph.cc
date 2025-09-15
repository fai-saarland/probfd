#include "downward/task_utils/causal_graph.h"

#include "downward/abstract_task.h"
#include "downward/axiom_utils.h"
#include "downward/state.h"

#include "downward/utils/logging.h"
#include "downward/utils/memory.h"
#include "downward/utils/timer.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

using namespace std;

/*
  We only want to create one causal graph per task, so they are cached globally.

  TODO: We need to rethink the memory management here. Objects in this cache are
  never reclaimed (before termination of the program). Also, currently every
  heuristic that uses one would receive its own causal graph object even if it
  uses an unmodified task because it will create its own copy of
  CostAdaptedTask.
  We have the same problems for other objects that are associated with tasks
  (causal graphs, successor generators and axiom evlauators, DTGs, ...) and can
  maybe deal with all of them in the same way.
*/

namespace downward::causal_graph {

static utils::HashMap<
    std::tuple<VariableSpace*, AxiomSpace*, ClassicalOperatorSpace*>,
    unique_ptr<CausalGraph>>
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
    typedef unordered_set<int> IntSet;
    vector<IntSet> int_sets;

    int get_range() const;

public:
    explicit IntRelationBuilder(int range);
    ~IntRelationBuilder();

    void add_pair(int u, int v);
    void compute_relation(IntRelation& result) const;
};

IntRelationBuilder::IntRelationBuilder(int range)
    : int_sets(range)
{
}

IntRelationBuilder::~IntRelationBuilder()
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

struct CausalGraphBuilder {
    IntRelationBuilder pre_eff_builder;
    IntRelationBuilder eff_pre_builder;
    IntRelationBuilder eff_eff_builder;

    IntRelationBuilder succ_builder;
    IntRelationBuilder pred_builder;

    explicit CausalGraphBuilder(int var_count)
        : pre_eff_builder(var_count)
        , eff_pre_builder(var_count)
        , eff_eff_builder(var_count)
        , succ_builder(var_count)
        , pred_builder(var_count)
    {
    }

    ~CausalGraphBuilder() {}

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

    void handle_operator(const AxiomOrOperatorProxy& op)
    {
        EffectsProxy effects = op.get_effects();

        // Handle pre->eff links from preconditions.
        for (FactPair pre : op.get_preconditions()) {
            int pre_var_id = pre.var;
            for (EffectProxy eff : effects) {
                int eff_var_id = eff.get_fact().var;
                if (pre_var_id != eff_var_id)
                    handle_pre_eff_arc(pre_var_id, eff_var_id);
            }
        }

        // Handle pre->eff links from effect conditions.
        for (EffectProxy eff : effects) {
            int eff_var_id = eff.get_fact().var;
            for (FactPair pre : eff.get_conditions()) {
                int pre_var_id = pre.var;
                if (pre_var_id != eff_var_id)
                    handle_pre_eff_arc(pre_var_id, eff_var_id);
            }
        }

        // Handle eff->eff links.
        for (size_t i = 0; i < effects.size(); ++i) {
            int eff1_var_id = effects[i].get_fact().var;
            for (size_t j = i + 1; j < effects.size(); ++j) {
                int eff2_var_id = effects[j].get_fact().var;
                if (eff1_var_id != eff2_var_id)
                    handle_eff_eff_edge(eff1_var_id, eff2_var_id);
            }
        }
    }
};

CausalGraph::CausalGraph(
    const VariableSpace& variables,
    const AxiomSpace& axioms,
    const ClassicalOperatorSpace& operators)
{
    int num_variables = variables.size();
    CausalGraphBuilder cg_builder(num_variables);

    for (OperatorProxy op : operators) cg_builder.handle_operator(op);

    for (AxiomProxy op : axioms) cg_builder.handle_operator(op);

    cg_builder.pre_eff_builder.compute_relation(pre_to_eff);
    cg_builder.eff_pre_builder.compute_relation(eff_to_pre);
    cg_builder.eff_eff_builder.compute_relation(eff_to_eff);

    cg_builder.pred_builder.compute_relation(predecessors);
    cg_builder.succ_builder.compute_relation(successors);
}

CausalGraph::~CausalGraph()
{
}

void CausalGraph::dump(const VariableSpace& variables) const
{
    utils::g_log << "Causal graph: " << endl;
    for (VariableProxy var : variables) {
        int var_id = var.get_id();
        utils::g_log << "#" << var_id << " [" << var.get_name() << "]:" << endl
                     << "    pre->eff arcs: " << pre_to_eff[var_id] << endl
                     << "    eff->pre arcs: " << eff_to_pre[var_id] << endl
                     << "    eff->eff arcs: " << eff_to_eff[var_id] << endl
                     << "    successors: " << successors[var_id] << endl
                     << "    predecessors: " << predecessors[var_id] << endl;
    }
}

const CausalGraph& get_causal_graph(const AbstractTaskTuple& task)
{
    auto [variables, axioms, operators] =
        slice<VariableSpace&, AxiomSpace&, ClassicalOperatorSpace&>(task);
    auto ptrs = std::make_tuple(&variables, &axioms, &operators);

    if (!causal_graph_cache.contains(ptrs)) {
        causal_graph_cache.insert(make_pair(
            ptrs,
            std::make_unique<CausalGraph>(variables, axioms, operators)));
    }
    return *causal_graph_cache[ptrs];
}
} // namespace downward::causal_graph