#include "downward/plan_manager.h"

#include "downward/operator_cost_function.h"
#include "downward/operator_space.h"
#include "downward/state.h"

#include "downward/task_utils/task_properties.h"
#include "downward/utils/logging.h"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

namespace downward {

int calculate_plan_cost(
    const Plan& plan,
    const OperatorIntCostFunction& cost_function)
{
    int plan_cost = 0;
    for (OperatorID op_id : plan) {
        plan_cost += cost_function.get_operator_cost(op_id.get_index());
    }
    return plan_cost;
}

PlanManager::PlanManager()
    : plan_filename("sas_plan")
    , num_previously_generated_plans(0)
    , is_part_of_anytime_portfolio(false)
{
}

void PlanManager::set_plan_filename(const string& plan_filename_)
{
    plan_filename = plan_filename_;
}

void PlanManager::set_num_previously_generated_plans(
    int num_previously_generated_plans_)
{
    num_previously_generated_plans = num_previously_generated_plans_;
}

void PlanManager::set_is_part_of_anytime_portfolio(
    bool is_part_of_anytime_portfolio_)
{
    is_part_of_anytime_portfolio = is_part_of_anytime_portfolio_;
}

void PlanManager::save_plan(
    const Plan& plan,
    const OperatorSpace& operators,
    const OperatorIntCostFunction& cost_function,
    int plan_cost,
    bool generates_multiple_plan_files)
{
    ostringstream filename;
    filename << plan_filename;
    int plan_number = num_previously_generated_plans + 1;
    if (generates_multiple_plan_files || is_part_of_anytime_portfolio) {
        filename << "." << plan_number;
    } else {
        assert(plan_number == 1);
    }
    ofstream outfile(filename.str());
    if (outfile.rdstate() & ofstream::failbit) {
        throw utils::InputError("Failed to open plan file: {}", filename.str());
    }

    for (OperatorID op_id : plan) {
        cout << operators[op_id].get_name() << " ("
             << cost_function.get_operator_cost(op_id.get_index()) << ")"
             << endl;
        outfile << "(" << operators[op_id].get_name() << ")" << endl;
    }
    bool is_unit_cost = task_properties::is_unit_cost(operators, cost_function);
    outfile << "; cost = " << plan_cost << " ("
            << (is_unit_cost ? "unit cost" : "general cost") << ")" << endl;
    outfile.close();
    ++num_previously_generated_plans;
}

} // namespace downward