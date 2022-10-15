#ifndef MERGE_AND_SHRINK_SCC_H
#define MERGE_AND_SHRINK_SCC_H

/*
  This implements Tarjan's linear-time algorithm for finding the maximal
  strongly connected components. It takes time proportional to the sum
  of the number of vertices and arcs.

  Instantiate class SCC with a graph represented as a std::vector of std::vectors,
  where graph[i] is the std::vector of successors of vertex i.

  Method get_result() returns a std::vector of strongly connected components,
  each of which is a std::vector of vertices (ints).
  This is a partitioning of all vertices where each SCC is a maximal subset
  such that each node in an SCC is reachable from all other nodes in the SCC.
  Note that the derived graph where each SCC is a single "supernode" is
  necessarily acyclic. The SCCs returned by get_result() are in a topological
  sort order with regard to this derived DAG.
*/

#include <vector>
#include <set>

namespace merge_and_shrink {

class SCC {
    // The following three are indexed by vertex number.
    std::vector<int> dfs_numbers;
    std::vector<int> dfs_minima;
    std::vector<int> stack_indices;

    std::vector<int> stack; // This is indexed by the level of recursion.
    std::vector<std::vector<int> > sccs;
    std::vector <int> vars_scc; // SCC index for each var
    std::vector <int> scc_layer; // Layer of each SCC
    std::vector <int> vars_layer; // LayerSCC of each var
    std::vector <std::set<int> > scc_graph;

    int current_dfs_number;

    void dfs(int vertex, const std::vector<std::vector<int> > & graph);
public:
    SCC(){
    }
    void compute_scc(const std::vector<std::vector<int> > & graph);

    const std::vector<std::vector<int> > & get_sccs() const{
      return sccs;
    }

    const std::vector <int> & get_vars_scc() const{
      return vars_scc;
    }
    const std::vector <int> & get_scc_layer() const{
      return scc_layer;
    }
    const std::vector <int> & get_vars_layer() const{
      return vars_layer;
    }

    const std::vector <std::set<int> > get_scc_graph() const {
      return scc_graph;
    }

};

}

#endif
