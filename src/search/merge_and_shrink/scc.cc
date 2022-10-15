#include "merge_and_shrink/scc.h"

#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

namespace merge_and_shrink {

void SCC::compute_scc(const vector<vector<int> > &graph)
{
    int node_count = graph.size();
    dfs_numbers.resize(node_count, -1);
    dfs_minima.resize(node_count, -1);
    stack_indices.resize(node_count, -1);
    stack.reserve(node_count);
    current_dfs_number = 0;

    for (int i = 0; i < node_count; i++)
        if (dfs_numbers[i] == -1) {
            dfs(i, graph);
        }

    reverse(sccs.begin(), sccs.end());

    // for(int i = 0; i < sccs.size(); i++) {
    //   cout << "SCC: ";
    //   for(int j = 0; j < sccs[i].size(); j++)
    // 	cout << " " << sccs[i][j];
    //   cout << endl;
    // }

    //First, compute the scc associated with each var
    vars_scc.resize(node_count, -1);
    for (size_t i = 0; i < sccs.size(); i++) {
        for (size_t j = 0; j < sccs[i].size(); j++) {
            vars_scc[sccs[i][j]] = i;
        }
    }

    scc_graph.resize(sccs.size());
    //Compute the layer in the CG of each scc and var.
    scc_layer.resize(sccs.size(), -1);
    vars_layer.resize(node_count, 0);
    //Initially all the sccs are considered to have layer 0 (root)

    for (size_t i = 0; i < sccs.size(); i++) {
        //If no one pointed this scc, it is a root.
        if (scc_layer[i] == -1) {
            scc_layer[i] = 0;
        }
        int layer = scc_layer[i];
        for (size_t j = 0; j < sccs[i].size(); j++) {
            int var = sccs[i][j];
            vars_layer[var] = layer; //Assign the layer of the scc to its
            //variables

            //Each element pointed by var and not in scc i, it is a
            //descendant of scc i
            for (size_t k = 0; k < graph[var].size(); k++) {
                int var2 = graph[var][k];
                int scc_var2 = vars_scc[var2];
                //If this is the first time we point it, we have found a
                //shortest path from the root to scc_var2
                if ((size_t) scc_var2 != i) {
                    scc_graph[i].insert(scc_var2);
                    if (scc_layer[scc_var2] == -1) {
                        //cout << var << " => " << var2 << ": " << scc_var2 << " updated to " << layer +1 << endl;
                        scc_layer[scc_var2] = layer + 1;
                    }
                }
            }
        }
    }

    // for(int i = 0; i < sccs.size(); i++) {
    //   cout << "SCC: " << i<< " => ";
    //   const set<int> & descendants = scc_graph[i];
    //   for(set<int>::iterator it = descendants.begin();
    // 	  it != descendants.end(); ++it){
    // 	cout << *it << " ";
    //   }
    //   cout << endl;
    // }
}

void SCC::dfs(int vertex, const vector<vector<int> > &graph)
{
    int vertex_dfs_number = current_dfs_number++;
    dfs_numbers[vertex] = dfs_minima[vertex] = vertex_dfs_number;
    stack_indices[vertex] = stack.size();
    stack.push_back(vertex);

    const vector<int> &successors = graph[vertex];
    for (size_t i = 0; i < successors.size(); i++) {
        int succ = successors[i];
        int succ_dfs_number = dfs_numbers[succ];
        if (succ_dfs_number == -1) {
            dfs(succ, graph);
            dfs_minima[vertex] = min(dfs_minima[vertex], dfs_minima[succ]);
        } else if (succ_dfs_number < vertex_dfs_number && stack_indices[succ] != -1) {
            dfs_minima[vertex] = min(dfs_minima[vertex], succ_dfs_number);
        }
    }

    if (dfs_minima[vertex] == vertex_dfs_number) {
        int stack_index = stack_indices[vertex];
        vector<int> scc;
        for (size_t i = stack_index; i < stack.size(); i++) {
            scc.push_back(stack[i]);
            stack_indices[stack[i]] = -1;
        }
        stack.erase(stack.begin() + stack_index, stack.end());
        sccs.push_back(scc);
    }
}

/*
#include <iostream>
using namespace std;

int main() {

#if 0
  int n0[] = {1, -1};
  int n1[] = {-1};
  int n2[] = {3, 4, -1};
  int n3[] = {2, 4, -1};
  int n4[] = {0, 3, -1};
  int *all_nodes[] = {n0, n1, n2, n3, n4, 0};
#endif

  // int n0[] = {-1};
  // int n1[] = {3, -1};
  // int n2[] = {4, -1};
  // int n3[] = {8, -1};
  // int n4[] = {0, 7, -1};
  // int n5[] = {4, -1};
  // int n6[] = {5, 8, -1};
  // int n7[] = {2, 6, -1};
  // int n8[] = {1, -1};
x  int n0[] = {1, -1};
  int n1[] = {2, 3, -1};
  int n2[] = {1, 5, -1};
  int n3[] = {7, 4, -1};
  int n4[] = {3, -1};
  int n5[] = {6, -1};
  int n6[] = {5, 8, 0, -1};
  int n7[] = {8, -1};
  int n8[] = {7, -1};
  int n9[] = {6, -1};

  int num_nodes = 10;
  int *all_nodes[] = {n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, 0};

  vector<vector<int> > graph;
  for(int i = 0; all_nodes[i] != 0; i++) {
    vector<int> successors;
    for(int j = 0; all_nodes[i][j] != -1; j++)
      successors.push_back(all_nodes[i][j]);
    graph.push_back(successors);
  }

  SCC scc (graph);
  vector<vector<int> > sccs = scc.get_sccs();
  for(int i = 0; i < sccs.size(); i++) {
    for(int j = 0; j < sccs[i].size(); j++)
      cout << " " << sccs[i][j];
    cout << endl;
  }

  cout << "vars_scc: ";
  for(int i = 0; i < scc.get_vars_scc().size(); i++){
    cout << scc.get_vars_scc()[i] << " ";
  }
  cout << endl;

  cout << "scc_layer: ";
  for(int i = 0; i < scc.get_scc_layer().size(); i++){
    cout << scc.get_scc_layer()[i] << " ";
  }
  cout << endl;

  cout << "vars_layer: ";
  for(int i = 0; i < scc.get_vars_layer().size(); i++){
    cout << scc.get_vars_layer()[i] << " ";
  }
  cout << endl;
}
*/

}
