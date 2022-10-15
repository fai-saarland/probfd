#ifndef MERGE_AND_SHRINK_VARIABLE_ORDER_FINDER_H
#define MERGE_AND_SHRINK_VARIABLE_ORDER_FINDER_H

#include <vector>

//#include "merge_and_shrink_heuristic.h" // needed for MergeStrategy type;
// TODO: move that type somewhere else?

namespace merge_and_shrink {

enum MergeStrategy {
    MERGE_LINEAR_CG_GOAL_LEVEL,
    MERGE_LINEAR_CG_GOAL_RANDOM,
    MERGE_LINEAR_GOAL_CG_LEVEL,
    MERGE_LINEAR_RANDOM,
    MERGE_DFP,
    MERGE_LINEAR_LEVEL,
    MERGE_LINEAR_REVERSE_LEVEL,
    MERGE_LINEAR,
    MERGE_LINEAR_MULTIPLE_DFS,
    MAX_MERGE_STRATEGY
};

class Abstraction;

class VariableOrderFinderInterface {
 public:
  virtual ~VariableOrderFinderInterface(){}
  virtual bool done() const = 0;
  virtual int next(Abstraction * abstraction = 0) = 0;
  virtual bool reduce_labels_before_merge() const = 0;
};

class VariableOrderFinder : public VariableOrderFinderInterface {
    const MergeStrategy merge_strategy;
    std::vector<int> selected_vars;
    std::vector<int> remaining_vars;
    std::vector<bool> is_goal_variable;
    std::vector<bool> is_causal_predecessor;

    void select_next(int position, int var_no);
public:
    VariableOrderFinder(MergeStrategy merge_strategy, bool is_first = true);
    virtual ~VariableOrderFinder();
    virtual bool done() const;
    virtual int next(Abstraction * abstraction = 0);

    virtual bool reduce_labels_before_merge() const{
      return false;
    }
};

}

#endif
