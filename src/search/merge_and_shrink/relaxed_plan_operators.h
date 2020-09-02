#ifndef RELAXED_PLAN_OPERATORS_H
#define RELAXED_PLAN_OPERATORS_H

#include "ff_heuristic.h"
#include <cassert>
#include <set>
#include <vector>
//#include <ext/hash_map>
//using namespace __gnu_cxx;

namespace merge_and_shrink {

class RelaxedPlanOperators : public FFHeuristic
{

    //int set_propostions_costs();
    //void reset();
    //void mark_relaxed_operators(std::vector<bool>& labels, double bound);
    //void mark_minimal_set_of_relaxed_operators(std::vector<bool>& labels);


public:
    RelaxedPlanOperators(const options::Options &options);
    ~RelaxedPlanOperators();

    void get_relaxed_plan_operators(std::vector<bool> &labels);
};

}

#endif
