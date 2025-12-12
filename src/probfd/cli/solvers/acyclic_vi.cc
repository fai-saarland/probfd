#include "probfd/cli/solvers/acyclic_vi.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include "probfd/cli/solvers/mdp_solver_options.h"

#include "probfd/solvers/mdp_solver.h"
#include "probfd/solvers/statistical_mdp_algorithm.h"

#include "probfd/algorithms/acyclic_value_iteration.h"

#include "downward/operator_id.h"
#include "downward/state.h"

#include <memory>
#include <string>

using namespace downward;
using namespace utils;

using namespace probfd;
using namespace probfd::solvers;
using namespace probfd::algorithms;
using namespace probfd::algorithms::acyclic_vi;

using namespace probfd::cli::solvers;

using namespace language::plugins;

namespace {
class AcyclicVIWithStatistics : public StatisticalMDPAlgorithm {
    AcyclicValueIteration<State, OperatorID> algorithm;

    unsigned long long state_expansions = 0;
    unsigned long long goal_states = 0;
    unsigned long long terminal_states = 0;
    unsigned long long pruned_states = 0;

public:
    AcyclicVIWithStatistics() { algorithm.registerObserverForSupported(*this); }

    std::unique_ptr<PolicyType> compute_policy(
        MDPType& mdp,
        HeuristicType& heuristic,
        ParamType<State> state,
        ProgressReport progress,
        FSeconds max_time) override
    {
        return algorithm
            .compute_policy(mdp, heuristic, state, progress, max_time);
    }

    void handleEvent(const StateExpansion&) { ++state_expansions; }

    void handleEvent(const GoalStateExpansion&) { ++goal_states; }

    void handleEvent(const TerminalStateExpansion&) { ++terminal_states; }

    void handleEvent(const PruneStateExpansion&) { ++pruned_states; }

    void print_statistics(std::ostream& out) const override
    {
        std::println(out, "  Expanded state(s): {}", state_expansions);
        std::println(out, "  Pruned state(s): {}", pruned_states);
        std::println(out, "  Terminal state(s): {}", terminal_states);
        std::println(out, "  Goal state(s): {}", goal_states);
    }
};

class AcyclicVISolver : public StatisticalMDPAlgorithmFactory {
public:
    std::string get_algorithm_name() const override
    {
        return "acyclic_value_iteration";
    }

    std::unique_ptr<StatisticalMDPAlgorithm>
    create_algorithm(const SharedProbabilisticTask&) override
    {
        return std::make_unique<AcyclicVIWithStatistics>();
    }
};

std::shared_ptr<TaskSolverFactory> create_acyclic_vi_solver(
    std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
    std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
    std::string policy_filename,
    bool print_fact_names,
    value_t report_epsilon,
    bool report_enabled,
    Verbosity verbosity)
{
    return make_shared_from_arg_tuples<MDPSolver>(
        std::make_shared<AcyclicVISolver>(),
        std::move(task_state_space_factory),
        std::move(heuristic_factory),
        std::move(policy_filename),
        print_fact_names,
        report_epsilon,
        report_enabled,
        verbosity);
}

InternalFunctionDefinitionBase& add_state_equation_constraints_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "acyclic_value_iteration",
        &create_acyclic_vi_solver);
    f.document_title("Acyclic Value Iteration");
    add_base_solver_options_except_algorithm_to_feature(f, 0);

    return f;
}

} // namespace

namespace probfd::cli::solvers {

void add_acyclic_value_iteration_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_state_equation_constraints_to_namespace(n);
}

} // namespace probfd::cli::solvers
