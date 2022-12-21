#include "probfd/globals.h"

#include "probfd/analysis_objectives/analysis_objective.h"

#include "algorithms/int_packer.h"

#include "utils/hash.h"
#include "utils/timer.h"

#include "legacy/global_operator.h"
#include "legacy/globals.h"
#include "legacy/successor_generator.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_set>

namespace probfd {

void prepare_globals(
    std::shared_ptr<analysis_objectives::AnalysisObjective> objective)
{
    g_analysis_objective = objective;

    std::cout << "building probabilistic operator successor generator..."
              << std::flush;
    g_successor_generator =
        std::make_shared<legacy::successor_generator::SuccessorGenerator<
            const ProbabilisticOperator*>>();
    std::cout << "done! [t=" << utils::g_timer << "]" << std::endl;

    g_initial_state_values = legacy::g_initial_state_data;

    if (g_steps_bounded) {
        g_step_var = legacy::g_variable_domain.size();

        std::vector<int> extended_domains(legacy::g_variable_domain);
        extended_domains.push_back(g_step_bound + 1);
        g_state_packer = new int_packer::IntPacker(extended_domains);

        g_initial_state_values.push_back(g_step_bound);
    } else {
        g_state_packer = legacy::g_state_packer;
    }

    for (unsigned op_id = 0; op_id < legacy::g_operators.size(); ++op_id) {
        legacy::g_operators[op_id].set_id(op_id);
    }

    for (int i = g_operators.size() - 1; i >= 0; i--) {
#ifndef NDEBUG
        value_type::value_t sum = 0;
        for (int j = g_operators[i].num_outcomes() - 1; j >= 0; j--) {
            sum += g_operators[i][j].prob;
        }
        if (!value_type::approx_equal()(sum, value_type::one)) {
            std::cerr << "Error in probabilistic operator reconstruction!"
                      << std::endl;
            std::cerr << "Outcome probabilities do not sum up to one"
                      << std::endl;
            std::cerr << "Operator: " << g_operators[i].get_name() << std::endl;
            std::cerr << "Probability: " << sum << std::endl;
            for (auto it = g_operators[i].begin(); it != g_operators[i].end();
                 it++) {
                std::cerr << " outcome: " << it->op->get_name() << " ("
                          << it->prob << ")" << std::endl;
            }
        }
        assert(value_type::approx_equal()(sum, value_type::one));
#endif
    }
}

void print_task_info()
{
    unsigned prob_ops = 0;
    unsigned outcomes = 0;
    for (int i = g_operators.size() - 1; i >= 0; i--) {
        const ProbabilisticOperator& op = g_operators[i];
        outcomes += op.num_outcomes();
        prob_ops += (op.num_outcomes() > 1);
    }
    unsigned orig_values = 0;
    unsigned values = 0;
    for (int var = legacy::g_variable_domain.size() - 1; var >= 0; --var) {
        values += legacy::g_variable_domain[var];
        orig_values += legacy::g_variable_domain[var];
    }
    if (g_step_var >= 0) {
        values += g_step_bound + 1;
    }
    std::cout << std::endl;
    std::cout << "Planning task:" << std::endl;
    std::cout << "  State variables: " << g_initial_state_values.size()
              << std::endl;
    std::cout << "  State variable values: " << values << " (" << orig_values
              << " excluding step variable)" << std::endl;
    std::cout << "  State size: "
              << (g_state_packer->get_num_bins() *
                  sizeof(int_packer::IntPacker::Bin))
              << " bytes" << std::endl;
    std::cout << "  Operators: " << g_operators.size() << std::endl;
    std::cout << "  Operators with stochastic effects: " << prob_ops
              << std::endl;
    std::cout << "  Operator outcomes: " << outcomes << " ("
              << legacy::g_operators.size() << " without dummy outcomes)"
              << std::endl;

    std::cout << "  Step bound: ";
    if (g_steps_bounded)
        std::cout << g_step_bound;
    else
        std::cout << "unbounded";
    std::cout << std::endl;

    std::cout << "  Step operator cost type: " << g_step_cost_type << std::endl;

    std::cout << "  Epsilon: " << value_type::g_epsilon << std::endl;
}

bool g_steps_bounded = false;
const int g_unlimited_budget = std::numeric_limits<int>::max();
int g_step_bound = g_unlimited_budget;
int g_step_var = -1;
legacy::OperatorCost g_step_cost_type = legacy::NORMAL;

std::vector<ProbabilisticOperator> g_operators;

int_packer::IntPacker* g_state_packer = nullptr;
std::vector<int> g_initial_state_values;

std::shared_ptr<legacy::successor_generator::SuccessorGenerator<
    const ProbabilisticOperator*>>
    g_successor_generator = nullptr;

std::shared_ptr<analysis_objectives::AnalysisObjective> g_analysis_objective;

} // namespace probfd
