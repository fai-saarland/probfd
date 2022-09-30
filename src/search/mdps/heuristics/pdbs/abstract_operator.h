#ifndef MDPS_HEURISTICS_PDBS_ABSTRACT_OPERATOR_H
#define MDPS_HEURISTICS_PDBS_ABSTRACT_OPERATOR_H

#include "../../distribution.h"
#include "abstract_state.h"

#include <string>
#include <vector>

namespace probabilistic {

class ProbabilisticOperator;

/// Namespace dedicated to probabilistic pattern databases.
namespace pdbs {

struct AbstractOperator {
    explicit AbstractOperator(unsigned id, int reward);
    unsigned original_operator_id;
    value_type::value_t reward;
    Distribution<AbstractState> outcomes;
};

struct AbstractRegressionOperator {
    explicit AbstractRegressionOperator(unsigned id, AbstractState effect);
    unsigned abstract_operator_id;
    AbstractState effect;
};

class AbstractOperatorToString {
public:
    explicit AbstractOperatorToString(
        const std::vector<ProbabilisticOperator>* ops);
    std::string operator()(const AbstractOperator* op) const;

private:
    const std::vector<ProbabilisticOperator>* ops_;
};

} // namespace pdbs
} // namespace probabilistic

#endif // __ABSTRACT_OPERATOR_H__