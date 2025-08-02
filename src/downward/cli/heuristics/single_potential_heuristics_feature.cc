#include "downward/cli/heuristics/single_potential_heuristics_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/cli/potentials/potential_options.h"

#include "downward/potentials/potential_function.h"
#include "downward/potentials/potential_heuristic.h"
#include "downward/potentials/potential_optimizer.h"

#include "downward/utils/system.h"

#include "downward/tasks/root_task.h"

#include "downward/initial_state_values.h"
#include "downward/task_dependent_factory.h"
#include "downward/task_transformation.h"

using namespace std;
using namespace downward;
using namespace downward::potentials;

using namespace downward::cli::potentials;
using namespace downward::cli::plugins;

namespace {
enum class OptimizeFor {
    INITIAL_STATE,
    ALL_STATES,
};

unique_ptr<PotentialFunction> create_potential_function(
    const SharedAbstractTask& task,
    lp::LPSolverType lpsolver,
    double max_potential,
    OptimizeFor opt_func)
{
    PotentialOptimizer optimizer(task, lpsolver, max_potential);
    switch (opt_func) {
    case OptimizeFor::INITIAL_STATE: {
        const State& initial_state = get_init(task).get_initial_state();
        optimizer.optimize_for_state(initial_state);
        break;
    }
    case OptimizeFor::ALL_STATES: optimizer.optimize_for_all_states(); break;
    default: ABORT("Unkown optimization function");
    }
    return optimizer.get_potential_function();
}

class PotentialHeuristicFactory : public TaskDependentFactory<Evaluator> {
    std::shared_ptr<TaskTransformation> transformation;
    bool cache_estimates;
    std::string description;
    utils::Verbosity verbosity;
    double max_potential;
    lp::LPSolverType lp_solver;
    OptimizeFor mode;

public:
    PotentialHeuristicFactory(
        shared_ptr<TaskTransformation> transformation,
        bool cache_estimates,
        string description,
        utils::Verbosity verbosity,
        double max_potential,
        lp::LPSolverType lp_solver,
        OptimizeFor mode)
        : transformation(std::move(transformation))
        , cache_estimates(cache_estimates)
        , description(std::move(description))
        , verbosity(verbosity)
        , max_potential(max_potential)
        , lp_solver(lp_solver)
        , mode(mode)
    {
    }

    unique_ptr<Evaluator> create_object(const SharedAbstractTask& task) override
    {
        auto transformation_result = transformation->transform(task);

        auto function = create_potential_function(
            transformation_result.transformed_task,
            lp_solver,
            max_potential,
            mode);

        return std::make_unique<PotentialHeuristic>(
            std::move(function),
            task,
            std::move(transformation_result),
            cache_estimates,
            description,
            verbosity);
    }
};

class InitialStatePotentialHeuristicFeature
    : public TypedFeature<
          TaskDependentFactory<Evaluator>,
          PotentialHeuristicFactory> {
public:
    InitialStatePotentialHeuristicFeature()
        : TypedFeature("initial_state_potential")
    {
        document_subcategory("heuristics_potentials");
        document_title("Potential heuristic optimized for initial state");
        document_synopsis(get_admissible_potentials_reference());

        add_admissible_potentials_options_to_feature(
            *this,
            "initial_state_potential");
    }

    shared_ptr<PotentialHeuristicFactory>
    create_component(const Options& opts, const utils::Context&) const override
    {
        return make_shared<PotentialHeuristicFactory>(
            opts.get<shared_ptr<TaskTransformation>>("transform"),
            opts.get<bool>("cache_estimates"),
            opts.get<string>("description"),
            opts.get<utils::Verbosity>("verbosity"),
            opts.get<double>("max_potential"),
            opts.get<lp::LPSolverType>("lpsolver"),
            OptimizeFor::INITIAL_STATE);
    }
};

class AllStatesPotentialHeuristicFeature
    : public TypedFeature<
          TaskDependentFactory<Evaluator>,
          PotentialHeuristicFactory> {
public:
    AllStatesPotentialHeuristicFeature()
        : TypedFeature("all_states_potential")
    {
        document_subcategory("heuristics_potentials");
        document_title("Potential heuristic optimized for all states");
        document_synopsis(get_admissible_potentials_reference());

        add_admissible_potentials_options_to_feature(
            *this,
            "all_states_potential");
    }

    shared_ptr<PotentialHeuristicFactory>
    create_component(const Options& opts, const utils::Context&) const override
    {
        return make_shared<PotentialHeuristicFactory>(
            opts.get<shared_ptr<TaskTransformation>>("transform"),
            opts.get<bool>("cache_estimates"),
            opts.get<string>("description"),
            opts.get<utils::Verbosity>("verbosity"),
            opts.get<double>("max_potential"),
            opts.get<lp::LPSolverType>("lpsolver"),
            OptimizeFor::ALL_STATES);
    }
};
}

namespace downward::cli::heuristics {

void add_single_potential_heuristics_features(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<InitialStatePotentialHeuristicFeature>();
    raw_registry.insert_feature_plugin<AllStatesPotentialHeuristicFeature>();
}

} // namespace
