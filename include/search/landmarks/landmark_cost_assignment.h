#ifndef LANDMARKS_LANDMARK_COST_ASSIGNMENT_H
#define LANDMARKS_LANDMARK_COST_ASSIGNMENT_H


#include "lp/lp_solver.h"

#include "globals.h"
#include "operator_cost.h"

#include <set>
#include <vector>
#include <memory>

class LandmarkGraph;
class LandmarkNode;

class LandmarkCostAssignment {
    const std::set<int> empty;
protected:
    std::shared_ptr<LandmarkGraph> lm_graph;
    OperatorCost cost_type;

    const std::set<int> &get_achievers(int lmn_status,
                                       const LandmarkNode &lmn) const;
public:
    LandmarkCostAssignment(std::shared_ptr<LandmarkGraph> graph, OperatorCost cost_type_);
    virtual ~LandmarkCostAssignment();

    virtual double cost_sharing_h_value() = 0;
};

class LandmarkUniformSharedCostAssignment : public LandmarkCostAssignment {
    bool use_action_landmarks;
public:
    LandmarkUniformSharedCostAssignment(std::shared_ptr<LandmarkGraph> graph, bool use_action_landmarks_, OperatorCost cost_type_);
    virtual ~LandmarkUniformSharedCostAssignment();

    virtual double cost_sharing_h_value();
};

class LandmarkEfficientOptimalSharedCostAssignment : public LandmarkCostAssignment {
    lp::LPSolver lp_solver;

    std::vector<lp::LPVariable> lp_variables;
    std::vector<lp::LPConstraint> lp_constraints;
    std::vector<lp::LPConstraint> non_empty_lp_constraints;
public:
    LandmarkEfficientOptimalSharedCostAssignment(std::shared_ptr<LandmarkGraph> graph, OperatorCost cost_type, lp::LPSolverType solver_type);
    virtual ~LandmarkEfficientOptimalSharedCostAssignment();

    virtual double cost_sharing_h_value();
};


#endif
