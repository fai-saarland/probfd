#include "downward/cli/plugins/plugin.h"

#include "probfd/cli/solvers/mdp_solver.h"

#include "probfd/solvers/mdp_solver.h"

#include "probfd/algorithms/acyclic_value_iteration.h"

#include "downward/operator_id.h"
#include "downward/task_proxy.h"

#include <memory>
#include <string>

using namespace utils;

using namespace probfd;
using namespace probfd::solvers;
using namespace probfd::algorithms;
using namespace probfd::algorithms::acyclic_vi;

using namespace probfd::cli::solvers;

using namespace downward::cli::plugins;

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
        double max_time)
    {
        return algorithm
            .compute_policy(mdp, heuristic, state, progress, max_time);
    }

    void handleEvent(const StateExpansion&) { ++state_expansions; }
    void handleEvent(const GoalStateExpansion&) { ++goal_states; }
    void handleEvent(const TerminalStateExpansion&) { ++terminal_states; }
    void handleEvent(const PruneStateExpansion&) { ++pruned_states; }

    void print_statistics(std::ostream& out) const
    {
        out << "  Expanded state(s): " << state_expansions << std::endl;
        out << "  Pruned state(s): " << pruned_states << std::endl;
        out << "  Terminal state(s): " << terminal_states << std::endl;
        out << "  Goal state(s): " << goal_states << std::endl;
    }
};

class AcyclicVISolver : public MDPSolver {
public:
    using MDPSolver::MDPSolver;

    std::string get_algorithm_name() const override
    {
        return "acyclic_value_iteration";
    }

    std::unique_ptr<StatisticalMDPAlgorithm> create_algorithm(
        const std::shared_ptr<ProbabilisticTask>&,
        const std::shared_ptr<FDRCostFunction>&) override
    {
        return std::make_unique<AcyclicVIWithStatistics>();
    }
};

class AcyclicVISolverFeature
    : public TypedFeature<TaskSolverFactory, AcyclicVISolver> {
public:
    AcyclicVISolverFeature()
        : TypedFeature("acyclic_value_iteration")
    {
        document_title("Acyclic Value Iteration");
        add_base_solver_options_to_feature(*this);
    }

protected:
    std::shared_ptr<AcyclicVISolver>
    create_component(const Options& options, const Context&) const override
    {
        return make_shared_from_arg_tuples<AcyclicVISolver>(
            get_base_solver_args_from_options(options));
    }
};

FeaturePlugin<AcyclicVISolverFeature> _plugin;

} // namespace
