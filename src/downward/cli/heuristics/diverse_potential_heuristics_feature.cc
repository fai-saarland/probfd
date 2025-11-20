#include "downward/cli/heuristics/diverse_potential_heuristics_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/potentials/potential_options.h"

#include "downward/cli/utils/rng_options.h"

#include "downward/potentials/diverse_potential_heuristics.h"
#include "downward/potentials/potential_function.h"
#include "downward/potentials/potential_max_heuristic.h"

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
        if (max_num_heuristics < 0) {
            throw std::domain_error("max_num_heuristics must be >= 0.");
        }

        if (num_samples < 0) {
            throw std::domain_error("num_samples must be >= 0.");
        }

        if (max_potential < 0.0) {
            throw std::domain_error("max_potential must be >= 0.");
        }
    }

    unique_ptr<Evaluator> create_object(const SharedAbstractTask& task) override
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
    : public SharedTypedFeature<
          TaskDependentFactory<Evaluator>,
          shared_ptr<TaskTransformation>,
          bool,
          string,
          utils::Verbosity,
          int,
          int,
          double,
          lp::LPSolverType,
          int> {
public:
    DiversePotentialMaxHeuristicFeature()
        : TypedFeature(
              "diverse_potentials",
              &DiversePotentialMaxHeuristicFeature::func)
    {
        document_title("Diverse potential heuristics");
        document_synopsis(get_admissible_potentials_reference());

        make_optional_argument_with_default(
            0,
            "num_samples",
            "1000",
            "Number of states to sample");
        make_optional_argument_with_default(
            1,
            "max_num_heuristics",
            "infinity()",
            "maximum number of potential heuristics");
        const auto n = add_admissible_potentials_options_to_feature(
            *this,
            "diverse_potentials",
            2);
        add_rng_options_to_feature(*this, n + 2);
    }

    static shared_ptr<TaskDependentFactory<Evaluator>> func(
        shared_ptr<TaskTransformation> transformation,
        bool cache_estimates,
        string description,
        utils::Verbosity verbosity,
        int num_samples,
        int max_num_heuristics,
        double max_potential,
        lp::LPSolverType lp_solver,
        int random_seed)
    {
        return make_shared<DiversePotentialMaxHeuristicFactory>(
            std::move(transformation),
            cache_estimates,
            std::move(description),
            verbosity,
            num_samples,
            max_num_heuristics,
            max_potential,
            lp_solver,
            random_seed);
    }
};
} // namespace

namespace downward::cli::heuristics {

void add_diverse_potential_heuristics_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    const Feature& f =
        n.insert_feature_plugin<DiversePotentialMaxHeuristicFeature>();
    SubcategoryPlugin& subcategory =
        registry.get_subcategory_plugin("heuristics_potentials");
    subcategory.add_feature(f);
}

} // namespace downward::cli::heuristics
