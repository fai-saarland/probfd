#ifndef MERGE_AND_SHRINK_LINEAR_MERGE_STRATEGY_H
#define MERGE_AND_SHRINK_LINEAR_MERGE_STRATEGY_H

// The new merge strategy is based on a list of criteria.  We start
// with the set of candidate variables and apply each criterion, that
// discards some variables, until only one variable is left. If more
// than one variable is left after applying all the criteria, the
// merge_order is used as final tie-breaking.
//
// CG: prefer v to v' if v has causal influence over already
// merged variables and v' not.
//
// GOAL: prefer goal variables to non-goal variables
//
// Relevant: prefer relevant variables to non-relevant variables. A
// variable is relevant if a) it is a goal or b) it has a causal
// influence over already merged variables.
//
// MinSCC: Same as CG, but applying tie-breaking in case that more
// than one variable is causally relevant: prefer variable v to v' if
// SCC(v) has a path to SCC(v') if SCC(v) has a path to SCC(v').
// options::Optionally, only one variable is selected per SCC
// (the one with smallest "level", i.e. closer to the SCC root)
//
// Empty: prefer variables that will make more labels of the current
// abstractionempty
//
// Num: prefer variables that apper in more labels of the current
// abstraction.

#include "merge_and_shrink/scc.h"
#include "merge_and_shrink/variable_order_finder.h"

#include <set>
#include <vector>
#include <string>
#include <limits>
#include <memory>

namespace options {
class Options;
class OptionParser;
}

namespace merge_and_shrink {

class Abstraction;

class MergeCriterion
{
protected:
    //Returns true in case that at least one variable fits the criterion
    bool filter(std::vector <int> &vars,
                const std::vector<bool> &criterion) const
    {
        std::vector<int> aux;
        for (size_t i = 0; i < vars.size(); i++) {
            if (criterion[vars[i]]) {
                aux.push_back(vars[i]);
            }
        }
        if (!aux.empty()) {
            vars.swap(aux);
            return true;
        }
        return false;
    }

    template<class T>
    void filter_best(std::vector <int> &vars,
                     const std::vector<T> &criterion,
                     bool minimize) const
    {
        std::vector<int> aux;
        double best = (minimize ? std::numeric_limits<double>::max() : 0.0);
        for (size_t i = 0; i < vars.size(); i++) {
            double score = criterion[vars[i]];
            if ((minimize && score < best) ||
                    (!minimize && score > best)) {
                std::vector<int>().swap(aux);
                best = score;
            }
            if (score == best) {
                aux.push_back(vars[i]);
            }
        }
        if (!aux.empty()) {
            vars.swap(aux);
        }
    }

    bool allow_incremental;
public:
    MergeCriterion() : allow_incremental(true) {}
    virtual ~MergeCriterion() = default;
    virtual void init() = 0;
    virtual void disable_incremental()
    {
        allow_incremental = false;
    }
    // Allows for incremental computation (currently used to compute
    // predecessor variables in the CG). However, it should only work if
    // we have no disabled this.
    virtual void select_next(int var_no) = 0;
    virtual void filter(std::vector <int> &vars, Abstraction *abstraction)  = 0;
    virtual std::string get_name() const = 0;
    virtual bool reduce_labels_before_merge() const
    {
        return false;
    }
};

class MergeCriterionCG : public MergeCriterion
{
protected:
    std::vector<bool> is_causal_predecessor;
public:
    virtual void init();
    virtual void select_next(int var_no);
    virtual void filter(std::vector <int> &vars, Abstraction *abstraction) ;
    virtual std::string get_name() const
    {
        return "CG";
    }
};

class MergeCriterionGoal : public MergeCriterion
{
    std::vector<bool> is_goal_variable;
public:
    virtual void init();
    virtual void select_next(int var_no);
    virtual void filter(std::vector <int> &vars, Abstraction *abstraction) ;
    virtual std::string get_name() const
    {
        return "GOAL";
    }
};

class MergeCriterionRelevant : public MergeCriterionCG
{
    virtual void init();
    virtual std::string get_name() const
    {
        return "RELEVANT";
    }
};

class MergeCriterionMinSCC : public MergeCriterion
{
    const bool reverse;
    const bool tie_by_level;

    std::vector<bool> is_causal_predecessor;
    SCC scc;

    void forbid_scc_descendants(int scc_index,
                                const std::vector<std::set<int> > &scc_graph,
                                std::vector<bool> &forbidden_sccs) const;
public:
    MergeCriterionMinSCC(bool reverse_ = false,
                         bool tie_by_level_ = true) :
        reverse(reverse_),
        tie_by_level(tie_by_level_)
    {
    }
    MergeCriterionMinSCC(const options::Options &opts);
    virtual void init();
    virtual void select_next(int var_no);
    virtual void filter(std::vector <int> &vars, Abstraction *abstraction) ;
    virtual std::string get_name() const
    {
        return "SCC";
    }
};

class MergeCriterionEmpty : public MergeCriterion
{
public:
    virtual void init();
    virtual void select_next(int var_no);
    virtual void filter(std::vector <int> &vars, Abstraction *abstraction) ;
    virtual std::string get_name() const
    {
        return "EMPTY";
    }
    virtual bool reduce_labels_before_merge() const
    {
        return true;
    }
};

class MergeCriterionEmptyGoal : public MergeCriterion
{
public:
    virtual void init();
    virtual void select_next(int var_no);
    virtual void filter(std::vector <int> &vars, Abstraction *abstraction) ;
    virtual std::string get_name() const
    {
        return "EMPTYGOAL";
    }
    virtual bool reduce_labels_before_merge() const
    {
        return true;
    }
};


class MergeCriterionNum : public MergeCriterion
{
public:
    virtual void init();
    virtual void select_next(int var_no);
    virtual void filter(std::vector <int> &vars, Abstraction *abstraction) ;
    virtual std::string get_name() const
    {
        return "NUM";
    }
    virtual bool reduce_labels_before_merge() const
    {
        return true;
    }
};

enum MergeOrder {
    LEVEL,
    REVERSE_LEVEL,
    RANDOM
};

class LinearMergeStrategy : public VariableOrderFinderInterface
{
    const std::vector <std::shared_ptr<MergeCriterion> > criteria;
    const MergeOrder order;

    std::vector<int> selected_vars;
    std::vector<int> remaining_vars;

    void select_next(int var_no);
public:
    LinearMergeStrategy(const std::vector <std::shared_ptr<MergeCriterion> > &merge_criteria,
                        const MergeOrder &merge_order, bool is_first);
    virtual ~LinearMergeStrategy();

    virtual bool done() const;
    virtual int next(Abstraction *abstraction = 0);

    virtual bool reduce_labels_before_merge() const
    {
        for (size_t i = 0; i < criteria.size(); ++i) {
            if (criteria[i]->reduce_labels_before_merge()) {
                return true;
            }
        }
        return false;
    }
};


// New class to make multiple abstractions in a DFS manner.
// Works similar to LinearMergeStrategy except that:
//   a) Returns a list of variables instead of a single var.
//       The list is ordered by preference.
//   b) It does not perform incremental computations:
//       (because we may remove variables)
class MultipleLinearMergeStrategy
{
    const std::vector <std::shared_ptr<MergeCriterion> > criteria;
    const MergeOrder order;

    void order_vars(Abstraction *abstraction,
                    std::vector<int> &selected_vars) const;

public:

    MultipleLinearMergeStrategy(const std::vector <std::shared_ptr<MergeCriterion> >
                                &merge_criteria,
                                const MergeOrder &merge_order, bool is_first);
    virtual ~MultipleLinearMergeStrategy();

    virtual bool reduce_labels_before_merge() const
    {
        for (size_t i = 0; i < criteria.size(); ++i) {
            if (criteria[i]->reduce_labels_before_merge()) {
                return true;
            }
        }
        return false;
    }

    bool cmp(const int v1, const int v2);
    void next_vars(std::vector<int> &selected_vars,
                   Abstraction *abstraction = 0) const;

};

}

#endif
