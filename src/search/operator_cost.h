#ifndef OPERATOR_COST_H
#define OPERATOR_COST_H

#include <string>

class GlobalOperator;

namespace options {
class OptionParser;
}

enum OperatorCost {NORMAL = 0, ONE = 1, PLUSONE = 2, ZERO=3, MINONE=4, MAX_OPERATOR_COST};

int get_adjusted_action_cost(int cost, OperatorCost cost_type);
int get_adjusted_action_reward(int reward, OperatorCost cost_type);
int get_adjusted_action_cost(const GlobalOperator &op, OperatorCost cost_type);
void add_cost_type_option_to_parser(options::OptionParser &parser, const std::string& option_name = "cost_type");

#endif
