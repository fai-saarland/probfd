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
    explicit AbstractOperator(unsigned id, int cost);
    unsigned original_operator_id;
    value_type::value_t cost;
    Distribution<AbstractState> outcomes;
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
