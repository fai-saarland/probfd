#include "globals.h"

#include "../algorithms/int_packer.h"
#include "../global_operator.h"
#include "../globals.h"
#include "../successor_generator.h"
#include "../utils/hash.h"
#include "../utils/timer.h"
#include "analysis_objectives/analysis_objective.h"

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

static std::vector<GlobalOperator> g_dummy_outcomes;

static std::pair<std::pair<int, int>, std::string>
parse_operator_outcome_name(const std::string& name)
{
    size_t det_pos = name.find("_DETDUP_");
    if (det_pos != std::string::npos) {
        // int det_num_pos = det_pos + 8;
        size_t weight_pos = name.find("_WEIGHT_");
        if (weight_pos != std::string::npos) {
            size_t weight_num_pos = weight_pos + 8;
            std::string tmp = name.substr(weight_num_pos, std::string::npos);
            weight_num_pos = tmp.find("_");
            int numerator, denominator;
            std::istringstream(tmp.substr(0, weight_num_pos)) >> numerator;
            std::istringstream(
                tmp.substr(weight_num_pos + 1, std::string::npos)) >>
                denominator;
            return std::make_pair(
                std::make_pair((int)numerator, (int)denominator),
                name.substr(0, det_pos) +
                    name.substr(name.find(" "), std::string::npos));
        } else {
            return std::make_pair(
                std::make_pair(1, 1),
                name.substr(0, det_pos) +
                    name.substr(name.find(" "), std::string::npos));
        }
    } else {
        return std::make_pair(std::make_pair(1, 1), name);
    }
}

static void assign_global_operators_to_probabilistic_operators(
    std::vector<std::string>& prob_operator_names,
    std::vector<std::vector<unsigned>>& outcomes,
    std::vector<std::pair<int, int>>& probabilities)
{
    // reconstructing probabilistic operators from outcomes
    std::vector<std::vector<std::pair<int, int>>> preconditions;
    struct PreHash {
        PreHash(
            const std::vector<std::vector<std::pair<int, int>>>& preconditions)
            : preconditions(preconditions)
        {
        }
        std::size_t operator()(unsigned i) const
        {
            return utils::get_hash(preconditions[i]);
        }
        const std::vector<std::vector<std::pair<int, int>>>& preconditions;
    };
    struct PreEqual {
        PreEqual(
            const std::vector<std::vector<std::pair<int, int>>>& preconditions)
            : preconditions(preconditions)
        {
        }
        bool operator()(unsigned i, unsigned j) const
        {
            return preconditions[i] == preconditions[j];
        }
        const std::vector<std::vector<std::pair<int, int>>>& preconditions;
    };
    std::unordered_set<unsigned, PreHash, PreEqual> precond_set(
        ::g_operators.size(),
        PreHash(preconditions),
        PreEqual(preconditions));
    std::map<std::pair<unsigned, std::string>, unsigned> name_to_i;
    unsigned cur_i = 0;
    for (unsigned j = 0; j < ::g_operators.size(); j++) {
        const GlobalOperator& op = ::g_operators[j];

        std::pair<std::pair<int, int>, std::string> data =
            parse_operator_outcome_name(op.get_name());
        probabilities.emplace_back(data.first);

        if (data.first.first == data.first.second) {
            prob_operator_names.push_back(std::move(data.second));
            outcomes.push_back({j});
            ++cur_i;
            continue;
        }

        std::vector<std::pair<int, int>> pre;
        for (const auto& p : op.get_preconditions()) {
            pre.emplace_back(p.var, p.val);
        }
        std::sort(pre.begin(), pre.end());

        preconditions.push_back(std::move(pre));
        auto precond_id = precond_set.insert(preconditions.size() - 1);
        if (!precond_id.second) {
            preconditions.pop_back();
        }

        std::pair<unsigned, std::string> key(*precond_id.first, data.second);

        unsigned i;
        auto it = name_to_i.find(key);
        if (it == name_to_i.end()) {
            i = cur_i;
            name_to_i[key] = cur_i++;
            prob_operator_names.push_back(std::move(data.second));
            outcomes.emplace_back();
        } else {
            i = it->second;
        }
        outcomes[i].push_back(j);
    }
}

static std::vector<ProbabilisticOutcome> create_probabilistic_outcomes(
    const std::vector<unsigned>& global_operator_refs,
    const std::vector<std::pair<int, int>>& probabilities,
    unsigned& dummy_refs)
{
    std::vector<ProbabilisticOutcome> result;
    result.reserve(global_operator_refs.size());
    int denom = 1;
    for (const unsigned& j : global_operator_refs) {
        denom = std::lcm(denom, probabilities[j].second);
    }
    int sum = 0;
    int cost = std::numeric_limits<int>::max();
    for (const unsigned& j : global_operator_refs) {
        int nom = (denom / probabilities[j].second) * probabilities[j].first;
        result.emplace_back(
            &::g_operators[j],
            value_type::from_fraction(nom, denom));
        sum += nom;
        cost = std::min(::g_operators[j].get_cost(), cost);
    }
    assert(sum <= denom);
    if (sum < denom) {
        auto& dummy = g_dummy_outcomes.emplace_back();
        dummy.set_cost(cost);
        dummy.set_id(::g_operators.size() + g_dummy_outcomes.size() - 1);

        const auto value = value_type::from_fraction(denom - sum, denom);
        result.emplace_back(&dummy, value);
        ++dummy_refs;
    }
    return result;
}

static void reconstruct_probabilistic_operators()
{
    assert(g_operators.empty());
    std::vector<std::string> names;
    std::vector<std::vector<unsigned>> outcome_assignment;
    std::vector<std::pair<int, int>> probabilities;
    assign_global_operators_to_probabilistic_operators(
        names,
        outcome_assignment,
        probabilities);
    g_operators.reserve(names.size());
    unsigned dummy_refs = 0;
    g_dummy_outcomes.reserve(names.size());
    for (unsigned i = 0; i < names.size(); i++) {
        g_operators.emplace_back(
            i,
            std::move(names[i]),
            create_probabilistic_outcomes(
                outcome_assignment[i],
                probabilities,
                dummy_refs));
    }
    std::cout << " introduced " << dummy_refs << " dummy outcome(s)...";
    // if (!dummy_refs) {
    //     ::g_operators.pop_back();
    // }
}

void prepare_globals(
    std::shared_ptr<analysis_objectives::AnalysisObjective> objective)
{
    g_analysis_objective = objective;

    std::cout << "reconstructing probabilistic operators..." << std::flush;
    reconstruct_probabilistic_operators();
    std::cout << "done! [t=" << utils::g_timer << "]" << std::endl;

    std::cout << "building probabilistic operator successor generator..."
              << std::flush;
    g_successor_generator =
        std::make_shared<successor_generator::SuccessorGenerator<
            const ProbabilisticOperator*>>();
    std::cout << "done! [t=" << utils::g_timer << "]" << std::endl;

    g_initial_state_values = ::g_initial_state_data;

    if (g_steps_bounded) {
        g_step_var = g_variable_domain.size();

        std::vector<int> extended_domains(g_variable_domain);
        extended_domains.push_back(g_step_bound + 1);
        g_state_packer = new int_packer::IntPacker(extended_domains);

        g_initial_state_values.push_back(g_step_bound);
    } else {
        g_state_packer = ::g_state_packer;
    }

    for (unsigned op_id = 0; op_id < ::g_operators.size(); ++op_id) {
        ::g_operators[op_id].set_id(op_id);
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
    for (int var = g_variable_domain.size() - 1; var >= 0; --var) {
        values += g_variable_domain[var];
        orig_values += g_variable_domain[var];
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
                  g_state_packer->get_bin_size_in_bytes())
              << " bytes" << std::endl;
    std::cout << "  Operators: " << g_operators.size() << std::endl;
    std::cout << "  Operators with stochastic effects: " << prob_ops
              << std::endl;
    std::cout << "  Operator outcomes: " << outcomes << " ("
              << ::g_operators.size() << " without dummy outcomes)"
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
OperatorCost g_step_cost_type = NORMAL;

std::vector<ProbabilisticOperator> g_operators;

int_packer::IntPacker* g_state_packer = nullptr;
std::vector<int> g_initial_state_values;

std::shared_ptr<
    successor_generator::SuccessorGenerator<const ProbabilisticOperator*>>
    g_successor_generator = nullptr;

std::shared_ptr<analysis_objectives::AnalysisObjective> g_analysis_objective;

} // namespace probfd
