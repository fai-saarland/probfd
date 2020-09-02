#ifndef OPERATOR_COUNTING_PHO_CONSTRAINTS_H
#define OPERATOR_COUNTING_PHO_CONSTRAINTS_H

#include "constraint_generator.h"

#include "../pdbs/types.h"

#include <memory>

namespace options {
class Options;
}

namespace pdbs {
class PatternCollectionGenerator;
}

namespace operator_counting {
class PhOConstraints : public ConstraintGenerator {
    std::shared_ptr<pdbs::PatternCollectionGenerator> pattern_generator;

    int constraint_offset;
    std::shared_ptr<pdbs::PDBCollection> pdbs;
public:
    explicit PhOConstraints(const options::Options &opts);

    virtual void initialize_constraints(
        OperatorCost cost_type,
        std::vector<lp::LPConstraint> &constraints,
        double infinity) override;
    virtual bool update_constraints(
        const GlobalState &state, lp::LPSolver &lp_solver) override;
};
}

#endif
