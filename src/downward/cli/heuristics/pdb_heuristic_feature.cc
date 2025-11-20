#include "downward/cli/heuristics/pdb_heuristic_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/heuristics/heuristic_options.h"

#include "downward/pdbs/pdb_heuristic.h"

#include "downward/task_dependent_factory.h"
#include "downward/task_transformation.h"

using namespace std;
using namespace downward;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::plugins;

using downward::cli::add_heuristic_options_to_feature;

namespace {
class PDBHeuristicFactory : public TaskDependentFactory<Evaluator> {
    std::shared_ptr<TaskTransformation> transformation;
    bool cache_estimates;
    std::string description;
    utils::Verbosity verbosity;
    std::shared_ptr<PatternGenerator> generator;

public:
    PDBHeuristicFactory(
        shared_ptr<TaskTransformation> transformation,
        bool cache_estimates,
        string description,
        utils::Verbosity verbosity,
        std::shared_ptr<PatternGenerator> generator)
        : transformation(std::move(transformation))
        , cache_estimates(cache_estimates)
        , description(std::move(description))
        , verbosity(verbosity)
        , generator(std::move(generator))
    {
    }

    unique_ptr<Evaluator> create_object(const SharedAbstractTask& task) override
    {
        auto transformation_result = transformation->transform(task);

        return std::make_unique<PDBHeuristic>(
            std::move(generator),
            task,
            std::move(transformation_result),
            cache_estimates,
            description,
            verbosity);
    }
};

class PDBHeuristicFeature
    : public SharedTypedFeature<
          TaskDependentFactory<Evaluator>,
          shared_ptr<TaskTransformation>,
          bool,
          string,
          utils::Verbosity,
          std::shared_ptr<PatternGenerator>> {
public:
    PDBHeuristicFeature()
        : TypedFeature("pdb", &PDBHeuristicFeature::func)
    {
        document_title("Pattern database heuristic");
        document_synopsis("TODO");

        document_language_support("action costs", "supported");
        document_language_support("conditional effects", "not supported");
        document_language_support("axioms", "not supported");

        document_property("admissible", "yes");
        document_property("consistent", "yes");
        document_property("safe", "yes");
        document_property("preferred operators", "no");

        make_optional_argument_with_default(
            0,
            "pattern",
            "greedy()",
            "pattern generation method");
        add_heuristic_options_to_feature(*this, "pdb", 1);
    }

    static shared_ptr<TaskDependentFactory<Evaluator>> func(
        shared_ptr<TaskTransformation> transformation,
        bool cache_estimates,
        string description,
        Verbosity verbosity,
        std::shared_ptr<PatternGenerator> generator)
    {
        return make_shared_from_arg_tuples<PDBHeuristicFactory>(
            std::move(transformation),
            cache_estimates,
            std::move(description),
            verbosity,
            std::move(generator));
    }
};
} // namespace

namespace downward::cli::heuristics {

void add_pdb_heuristic_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    const Feature& f = n.insert_feature_plugin<PDBHeuristicFeature>();
    SubcategoryPlugin& subcategory =
        registry.get_subcategory_plugin("heuristics_pdb");
    subcategory.add_feature(f);
}

} // namespace downward::cli::heuristics
