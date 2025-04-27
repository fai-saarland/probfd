#ifndef PROBFD_TASK_UTILS_CAUSAL_GRAPH_H
#define PROBFD_TASK_UTILS_CAUSAL_GRAPH_H

#include "probfd/probabilistic_task.h"

#include <vector>

namespace downward {
class VariableSpace;
class AxiomSpace;
} // namespace downward

namespace downward::utils {
class LogProxy;
}

namespace probfd::causal_graph {

using IntRelation = std::vector<std::vector<int>>;

class ProbabilisticCausalGraph {
    IntRelation pre_to_eff;
    IntRelation eff_to_pre;
    IntRelation eff_to_eff;

    IntRelation successors;
    IntRelation predecessors;

    void dump(
        const downward::VariableSpace& variables,
        downward::utils::LogProxy& log) const;

public:
    /* Use the factory function get_causal_graph to create causal graphs
       to avoid creating more than one causal graph per AbstractTask. */
    ProbabilisticCausalGraph(
        const downward::VariableSpace& variables,
        const downward::AxiomSpace& axioms,
        const ProbabilisticOperatorSpace& operators);

    /*
      All below methods querying neighbors (of some sort or other) of
      var guarantee that:
      - the return vertex list is sorted
      - var itself is not in the returned list

      "Successors" and "predecessors" are w.r.t. the common definition
      of causal graphs, which have pre->eff and eff->eff arcs.

      Note that axioms are treated as operators in the causal graph,
      i.e., their condition variables are treated as precondition
      variables and the derived variable is treated as an effect
      variable.

      For effect conditions, we only add pre->eff arcs for the respective
      conditional effect.
    */

    const std::vector<int>& get_pre_to_eff(int var) const
    {
        return pre_to_eff[var];
    }

    const std::vector<int>& get_eff_to_pre(int var) const
    {
        return eff_to_pre[var];
    }

    const std::vector<int>& get_eff_to_same_eff(int var) const
    {
        return eff_to_eff[var];
    }

    const std::vector<int>& get_eff_to_co_eff(int var) const
    {
        return eff_to_eff[var];
    }

    const std::vector<int>& get_successors(int var) const
    {
        return successors[var];
    }

    const std::vector<int>& get_predecessors(int var) const
    {
        return predecessors[var];
    }

    const std::vector<std::vector<int>>& get_arcs() const { return successors; }

    const std::vector<std::vector<int>>& get_inverse_arcs() const
    {
        return predecessors;
    }
};

/* Create or retrieve a causal graph from cache. If causal graphs are created
   with this function, we build at most one causal graph per AbstractTask. */
extern const ProbabilisticCausalGraph& get_causal_graph(
    const downward::VariableSpace& variables,
    const downward::AxiomSpace& axioms,
    const ProbabilisticOperatorSpace& operators);

} // namespace probfd::causal_graph

#endif // PROBFD_TASK_UTILS_CAUSAL_GRAPH_H
