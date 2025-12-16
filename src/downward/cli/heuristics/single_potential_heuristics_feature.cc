#include "downward/cli/heuristics/single_potential_heuristics_feature.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

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
using namespace language::plugins;

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
    default: throw utils::CriticalError("Unknown optimization function");
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

shared_ptr<TaskDependentFactory<Evaluator>>
create_initial_state_potential_heuristic(
    shared_ptr<TaskTransformation> transformation,
    bool cache_estimates,
    string description,
    utils::Verbosity verbosity,
    double max_potential,
    lp::LPSolverType lp_solver)
{
    return make_shared<PotentialHeuristicFactory>(
        std::move(transformation),
        cache_estimates,
        std::move(description),
        verbosity,
        max_potential,
        lp_solver,
        OptimizeFor::INITIAL_STATE);
}

shared_ptr<TaskDependentFactory<Evaluator>>
create_all_states_potential_heuristic(
    shared_ptr<TaskTransformation> transformation,
    bool cache_estimates,
    string description,
    utils::Verbosity verbosity,
    double max_potential,
    lp::LPSolverType lp_solver)
{
    return make_shared<PotentialHeuristicFactory>(
        std::move(transformation),
        cache_estimates,
        std::move(description),
        verbosity,
        max_potential,
        lp_solver,
        OptimizeFor::ALL_STATES);
}

InternalFunctionDefinitionBase& add_initial_state_potential_heuristic_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "initial_state_potential",
        create_initial_state_potential_heuristic);

    f.document_title("Potential heuristic optimized for initial state");
    f.document_synopsis(get_admissible_potentials_reference());

    add_admissible_potentials_options_to_feature(
        f,
        "initial_state_potential",
        0);

    return f;
}

InternalFunctionDefinitionBase& add_all_states_potential_heuristic_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "all_states_potential",
        create_all_states_potential_heuristic);

    f.document_title("Potential heuristic optimized for all states");
    f.document_synopsis(get_admissible_potentials_reference());

    add_admissible_potentials_options_to_feature(f, "all_states_potential", 0);

    return f;
}

} // namespace

namespace downward::cli::heuristics {

void add_single_potential_heuristics_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    DocumentationTopic& subcategory =
        registry.get_topic_by_name("heuristics_potentials");
    const InternalFunctionDefinitionBase& f = add_initial_state_potential_heuristic_to_namespace(n);
    const InternalFunctionDefinitionBase& f2 = add_all_states_potential_heuristic_to_namespace(n);
    subcategory.add_function(f);
    subcategory.add_function(f2);
}

} // namespace downward::cli::heuristics
