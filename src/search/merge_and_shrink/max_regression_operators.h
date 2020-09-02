#ifndef MAX_REGRESSION_OPERATORS_H
#define MAX_REGRESSION_OPERATORS_H

#include "../algorithms/priority_queues.h"
#include "max_heuristic.h"
#include <cassert>
#include <set>
#include <vector>

namespace merge_and_shrink {


class MaxRegressionOperators : public HSPMaxHeuristic
{
    std::set<Proposition *> reached_props;
    const double alpha;
    const bool all_best_supporters;

    bool enqueue_if_necessary(Proposition *prop, UnaryOperator *op)
    {
        int cost = (op == 0) ? 0 : op->cost ;
        assert(cost >= 0);
        bool res = false;
        if (prop->cost == -1 || prop->cost >= cost) {
            // Adding the operator to reached_by
            // in case when the cost of the proposition is decreased, need to empty
            // the previously added operators
            if (op == 0 || prop->cost > cost) {
                prop->marked_ops.clear();
            }

            if (op != 0) {
                prop->marked_ops.insert(op);
            }

            prop->cost = cost;
            reached_props.insert(prop);
            res = true;
        }
        assert(prop->cost != -1 && prop->cost <= cost);
        return res;
    }

    void dump_unary_operator(const UnaryOperator *op) const
    {
        std::cout << "ID: " << op->operator_no << " Cost: " << op->cost;
        if (op->precondition.size() > 0) {
            std::cout << " Preconditions:";
        }
        for (size_t i = 0; i < op->precondition.size(); i++) {
            std::cout << " " << op->precondition[i]->id;
        }
        if (op->effect) {
            std::cout << " Achieving: " << op->effect->id;
        }
        std::cout << std::endl;
    }

    int set_propostions_costs();
    void reset();
    void mark_relaxed_operators(std::vector<bool> &labels, double bound);
    void mark_minimal_set_of_relaxed_operators(std::vector<bool> &labels);


public:
    MaxRegressionOperators(const options::Options &options);
    ~MaxRegressionOperators();

    void get_relaxed_operators(std::vector<bool> &labels);
};

}

#endif
