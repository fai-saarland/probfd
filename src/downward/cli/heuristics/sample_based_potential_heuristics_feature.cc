#include "downward/cli/heuristics/sample_based_potential_heuristics_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

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
        if (num_samples < 0) {
            throw std::domain_error("num_samples must be >= 0.");
        }

        if (num_heuristics < 0) {
            throw std::domain_error("num_heuristics must be >= 0.");
        }

        if (max_potential < 0) {
            throw std::domain_error("max_potential must be >= 0.");
        }
    }

    unique_ptr<Evaluator> create_object(const SharedAbstractTask& task) override
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

Feature& add_sample_based_potential_heuristic_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_typed_feature_plugin(
        "sample_based_potentials",
        &cli::plugins::make_shared<
            TaskDependentFactory<Evaluator>,
            PotentialMaxHeuristicFactory,
            shared_ptr<TaskTransformation>,
            bool,
            string,
            Verbosity,
            int,
            int,
            double,
            lp::LPSolverType,
            int>);

    f.document_title("Sample-based potential heuristics");
    f.document_synopsis(
        "Maximum over multiple potential heuristics optimized for "
        "samples. " +
        get_admissible_potentials_reference());

    f.make_optional_argument_with_default(
        0,
        "num_heuristics",
        "1",
        "number of potential heuristics");
    f.make_optional_argument_with_default(
        1,
        "num_samples",
        "1000",
        "Number of states to sample");
    const auto n = add_admissible_potentials_options_to_feature(
        f,
        "sample_based_potentials",
        2);
    add_rng_options_to_feature(f, n + 2);

    return f;
}

} // namespace

namespace downward::cli::heuristics {

void add_sample_based_potential_heuristics_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    SubcategoryPlugin& subcategory =
        registry.get_subcategory_plugin("heuristics_potentials");
    const Feature& f = add_sample_based_potential_heuristic_to_namespace(n);
    subcategory.add_feature(f);
}

} // namespace downward::cli::heuristics
