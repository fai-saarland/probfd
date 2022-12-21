#ifndef LEGACY_OPERATOR_COST_H
#define LEGACY_OPERATOR_COST_H

#include <string>

namespace options {
class OptionParser;
}

namespace legacy {

class GlobalOperator;

enum OperatorCost {
    NORMAL = 0,
    ONE = 1,
    PLUSONE = 2,
    ZERO = 3,
    MINONE = 4,
    MAX_OPERATOR_COST
};

int get_adjusted_action_cost(
    int cost,
    OperatorCost cost_type,
    bool is_unit_cost = false);
int get_adjusted_action_reward(int reward, OperatorCost cost_type);
int get_adjusted_action_cost(const GlobalOperator& op, OperatorCost cost_type);
void add_cost_type_option_to_parser(
    options::OptionParser& parser,
    const std::string& option_name = "cost_type");

} // namespace legacy

#endif
