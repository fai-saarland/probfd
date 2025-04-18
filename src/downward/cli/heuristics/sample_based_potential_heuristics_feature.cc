#include "downward/cli/plugins/plugin.h"

#include "downward/cli/potentials/potential_options.h"
#include "downward/cli/utils/rng_options.h"

#include "downward/potentials/potential_function.h"
#include "downward/potentials/potential_max_heuristic.h"
#include "downward/potentials/potential_optimizer.h"
#include "downward/potentials/util.h"

#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

#include "downward/tasks/root_task.h"

#include "downward/task_transformation.h"

#include "downward/task_dependent_factory.h"

#include <memory>
#include <vector>

using namespace std;
using namespace downward;
using namespace downward::utils;
using namespace downward::potentials;

using namespace downward::cli::potentials;
using namespace downward::cli::plugins;
using namespace downward::cli::utils;

namespace {

class PotentialMaxHeuristicFactory : public TaskDependentFactory<Evaluator> {
    std::shared_ptr<TaskTransformation> transformation;
    bool cache_estimates;
    std::string description;
    utils::Verbosity verbosity;
    int num_samples;
    int num_heuristics;
    double max_potential;
    lp::LPSolverType lp_solver;
    int random_seed;

public:
    PotentialMaxHeuristicFactory(
        shared_ptr<TaskTransformation> transformation,
        bool cache_estimates,
        string description,
        utils::Verbosity verbosity,
        int num_samples,
        int num_heuristics,
        double max_potential,
        lp::LPSolverType lp_solver,
        int random_seed)
        : transformation(std::move(transformation))
        , cache_estimates(cache_estimates)
        , description(std::move(description))
        , verbosity(verbosity)
        , num_samples(num_samples)
        , num_heuristics(num_heuristics)
        , max_potential(max_potential)
        , lp_solver(lp_solver)
        , random_seed(random_seed)
    {
    }

    unique_ptr<Evaluator>
    create_object(const std::shared_ptr<AbstractTask>& task) override
    {
        auto transformation_result = transformation->transform(task);

        vector<unique_ptr<PotentialFunction>> functions;
        PotentialOptimizer optimizer(
            transformation_result.transformed_task,
            lp_solver,
            max_potential);
        const shared_ptr rng(get_rng(random_seed));

        for (int i = 0; i < num_heuristics; ++i) {
            vector<State> samples =
                sample_without_dead_end_detection(optimizer, num_samples, *rng);
            if (!optimizer.potentials_are_bounded()) {
                assert(!optimizer.potentials_are_bounded());
                vector<State> non_dead_end_samples;
                for (const State& sample : samples) {
                    optimizer.optimize_for_state(sample);
                    if (optimizer.has_optimal_solution())
                        non_dead_end_samples.push_back(sample);
                }
                swap(samples, non_dead_end_samples);
            }
            optimizer.optimize_for_samples(samples);
            functions.push_back(optimizer.get_potential_function());
        }

        return std::make_unique<PotentialMaxHeuristic>(
            std::move(functions),
            task,
            std::move(transformation_result),
            cache_estimates,
            description,
            verbosity);
    }
};

class SampleBasedPotentialMaxHeuristicFeature
    : public TypedFeature<
          TaskDependentFactory<Evaluator>,
          PotentialMaxHeuristicFactory> {
public:
    SampleBasedPotentialMaxHeuristicFeature()
        : TypedFeature("sample_based_potentials")
    {
        document_subcategory("heuristics_potentials");
        document_title("Sample-based potential heuristics");
        document_synopsis(
            "Maximum over multiple potential heuristics optimized for "
            "samples. " +
            get_admissible_potentials_reference());
        add_option<int>(
            "num_heuristics",
            "number of potential heuristics",
            "1",
            Bounds("0", "infinity"));
        add_option<int>(
            "num_samples",
            "Number of states to sample",
            "1000",
            Bounds("0", "infinity"));
        add_admissible_potentials_options_to_feature(
            *this,
            "sample_based_potentials");
        add_rng_options_to_feature(*this);
    }

    shared_ptr<PotentialMaxHeuristicFactory>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared<PotentialMaxHeuristicFactory>(
            opts.get<shared_ptr<TaskTransformation>>("transform"),
            opts.get<bool>("cache_estimates"),
            opts.get<string>("description"),
            opts.get<Verbosity>("verbosity"),
            opts.get<int>("num_samples"),
            opts.get<int>("num_heuristics"),
            opts.get<double>("max_potential"),
            opts.get<downward::lp::LPSolverType>("lpsolver"),
            opts.get<int>("random_seed"));
    }
};

FeaturePlugin<SampleBasedPotentialMaxHeuristicFeature> _plugin;

} // namespace
