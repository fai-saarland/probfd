#include "plan_manager.h"

#include "task_proxy.h"

#include "task_utils/task_properties.h"
#include "utils/logging.h"

#include "global_operator.h"

#include <fstream>
#include <iostream>
#include <sstream>


using namespace std;

int calculate_plan_cost(const Plan& plan)
{
    int plan_cost = 0;
    for (const GlobalOperator* op : plan) {
        plan_cost += op->get_cost();
    }
    return plan_cost;
}

PlanManager::PlanManager()
    : plan_filename("sas_plan"),
      num_previously_generated_plans(0),
      is_part_of_anytime_portfolio(false) {
}

void PlanManager::set_plan_filename(const string &plan_filename_) {
    plan_filename = plan_filename_;
}

void PlanManager::set_num_previously_generated_plans(int num_previously_generated_plans_) {
    num_previously_generated_plans = num_previously_generated_plans_;
}

void PlanManager::set_is_part_of_anytime_portfolio(bool is_part_of_anytime_portfolio_) {
    is_part_of_anytime_portfolio = is_part_of_anytime_portfolio_;
}

void PlanManager::save_plan(
    const Plan& plan,
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
        cerr << "Failed to open plan file: " << filename.str() << endl;
        utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
    }
    for (const GlobalOperator* op : plan) {
        cout << op->get_name() << " (" << op->get_cost() << ")" << endl;
        outfile << "(" << op->get_name() << ")" << endl;
    }
    int plan_cost = calculate_plan_cost(plan);
    bool is_unit_cost = task_properties::is_unit_cost();
    outfile << "; cost = " << plan_cost << " ("
            << (is_unit_cost ? "unit cost" : "general cost") << ")" << endl;
    outfile.close();
    utils::g_log << "Plan length: " << plan.size() << " step(s)." << endl;
    utils::g_log << "Plan cost: " << plan_cost << endl;
    ++num_previously_generated_plans;
}
