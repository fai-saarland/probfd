#ifndef MERGE_AND_SHRINK_MERGE_AND_SHRINK_HEURISTIC_H
#define MERGE_AND_SHRINK_MERGE_AND_SHRINK_HEURISTIC_H

#include "merge_and_shrink/linear_merge_strategy.h"
#include "merge_and_shrink/shrink_empty_labels.h"
#include "merge_and_shrink/shrink_strategy.h"
#include "merge_and_shrink/variable_order_finder.h"

#include "../heuristic.h"

#include <utility>
#include <vector>
#include <memory>

namespace merge_and_shrink {

class Abstraction;

class MergeAndShrinkHeuristic : public Heuristic
{
    const int abstraction_count;
    const std::vector<std::shared_ptr<MergeCriterion> > merge_criteria;
    const MergeOrder merge_order;
    const MergeStrategy merge_strategy;
    std::shared_ptr<ShrinkStrategy> shrink_strategy;
    const bool use_label_reduction;
    const bool prune_unreachable;
    const bool prune_irrelevant;
    const bool use_expensive_statistics;
    const bool use_label_inheritance;
    const bool new_approach;
    const bool use_uniform_distances;
    const bool check_unsolvability;
    const bool continue_if_unsolvable;
    std::shared_ptr<ShrinkEmptyLabels> empty_label_shrink_strategy;

    //Parameters for linear_multiple merge:
    const int merge_size_limit; // Avoid merging a variable if the result will pass this size
    const int max_branching_merge; // Maximum branching factor for multiple merge

    const bool c_release_memory;


    std::vector<Abstraction *> atomic_abstractions;
    std::vector<Abstraction *> abstractions;
    Abstraction *build_abstraction(bool is_first = true);

    //new methods for DFS
    void prepare_after_merge(Abstraction *abstraction,
                             bool force_label_reduction) const;
    void prepare_before_merge(Abstraction *abstraction,
                              Abstraction *other_abstraction) const;
    void build_atomic_abstractions();
    void build_multiple_abstractions(std::vector<Abstraction *> &res_abstractions);
    bool build_abstraction_dfs(Abstraction *abstraction,
                               MultipleLinearMergeStrategy &order,
                               std::set<std::set<int> > &explored_varsets,
                               std::vector<Abstraction *> &res_abstractions) const;
    //Filter irrelevant variables according to two criteria:
    //  a) Skip combinations already visited
    //  b) Skip merging variables whose size is surpassed
    void get_candidate_vars(std::vector<int> &res,
                            std::set<std::set<int> > &explored_varsets,
                            Abstraction *abstraction = 0) const;


    void dump_options() const;
    void warn_on_unusual_options() const;
protected:
    virtual void initialize();
    virtual int compute_heuristic(const GlobalState &state);
public:
    std::vector<Abstraction *> &get_abstractions()
    {
        return abstractions;
    }
    MergeAndShrinkHeuristic(const options::Options &opts);
    ~MergeAndShrinkHeuristic();
};

}

#endif
