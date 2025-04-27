#include "downward/cli/plugins/plugin.h"

#include "downward/cli/potentials/potential_options.h"

#include "downward/cli/utils/rng_options.h"

#include "downward/potentials/diverse_potential_heuristics.h"
#include "downward/potentials/potential_max_heuristic.h"
#include "downward/potentials/potential_function.h"

#include "downward/utils/logging.h"

#include "downward/tasks/root_task.h"

#include "downward/task_dependent_factory.h"
#include "downward/task_transformation.h"

using namespace std;
using namespace downward;
using namespace downward::utils;
using namespace downward::potentials;

using namespace downward::cli::plugins;
using namespace downward::cli::potentials;
using namespace downward::cli::utils;

namespace {

class DiversePotentialMaxHeuristicFactory
    : public TaskDependentFactory<Evaluator> {
    std::shared_ptr<TaskTransformation> transformation;
    bool cache_estimates;
    std::string description;
    utils::Verbosity verbosity;
    int num_samples;
    int max_num_heuristics;
    double max_potential;
    lp::LPSolverType lp_solver;
    int random_seed;

public:
    DiversePotentialMaxHeuristicFactory(
        shared_ptr<TaskTransformation> transformation,
        bool cache_estimates,
        string description,
        utils::Verbosity verbosity,
        int num_samples,
        int max_num_heuristics,
        double max_potential,
        lp::LPSolverType lp_solver,
        int random_seed)
        : transformation(std::move(transformation))
        , cache_estimates(cache_estimates)
        , description(std::move(description))
        , verbosity(verbosity)
        , num_samples(num_samples)
        , max_num_heuristics(max_num_heuristics)
        , max_potential(max_potential)
        , lp_solver(lp_solver)
        , random_seed(random_seed)
    {
    }

    unique_ptr<Evaluator>
    create_object(const SharedAbstractTask& task) override
    {
        auto transformation_result = transformation->transform(task);

        DiversePotentialHeuristics dph(
            num_samples,
            max_num_heuristics,
            max_potential,
            lp_solver,
            transformation_result.transformed_task,
            random_seed,
            verbosity);
        auto functions = dph.find_functions();

        return std::make_unique<PotentialMaxHeuristic>(
            std::move(functions),
            task,
            std::move(transformation_result),
            cache_estimates,
            description,
            verbosity);
    }
};

class DiversePotentialMaxHeuristicFeature
    : public TypedFeature<
          TaskDependentFactory<Evaluator>,
          DiversePotentialMaxHeuristicFactory> {
public:
    DiversePotentialMaxHeuristicFeature()
        : TypedFeature("diverse_potentials")
    {
        document_subcategory("heuristics_potentials");
        document_title("Diverse potential heuristics");
        document_synopsis(get_admissible_potentials_reference());

        add_option<int>(
            "num_samples",
            "Number of states to sample",
            "1000",
            Bounds("0", "infinity"));
        add_option<int>(
            "max_num_heuristics",
            "maximum number of potential heuristics",
            "infinity",
            Bounds("0", "infinity"));
        add_admissible_potentials_options_to_feature(
            *this,
            "diverse_potentials");
        add_rng_options_to_feature(*this);
    }

    shared_ptr<DiversePotentialMaxHeuristicFactory>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared<DiversePotentialMaxHeuristicFactory>(
            opts.get<shared_ptr<TaskTransformation>>("transform"),
            opts.get<bool>("cache_estimates"),
            opts.get<string>("description"),
            opts.get<Verbosity>("verbosity"),
            opts.get<int>("num_samples"),
            opts.get<int>("max_num_heuristics"),
            opts.get<double>("max_potential"),
            opts.get<lp::LPSolverType>("lpsolver"),
            opts.get<int>("random_seed"));
    }
};

FeaturePlugin<DiversePotentialMaxHeuristicFeature> _plugin;

} // namespace
