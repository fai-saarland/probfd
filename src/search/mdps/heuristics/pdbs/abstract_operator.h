#pragma once

#include "../../distribution.h"
#include "abstract_state.h"

#include <string>
#include <vector>

namespace probabilistic {

class ProbabilisticOperator;

namespace pdbs {

class AbstractOperator {
public:
    explicit AbstractOperator(unsigned id);
    unsigned original_operator_id;
    AbstractState pre;
    Distribution<AbstractState> outcomes;
};

class AbstractOperatorToString {
public:
    explicit AbstractOperatorToString(
        const std::vector<ProbabilisticOperator>* ops);
    std::string operator()(const AbstractOperator* op);

private:
    const std::vector<ProbabilisticOperator>* ops_;
};

} // namespace pdbs
} // namespace probabilistic
