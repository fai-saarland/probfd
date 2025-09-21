#include "downward/cli/heuristics/pdb_heuristic_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

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
using downward::cli::get_heuristic_arguments_from_options;

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
    : public TypedFeature<TaskDependentFactory<Evaluator>> {
public:
    PDBHeuristicFeature()
        : TypedFeature("pdb")
    {
        document_subcategory("heuristics_pdb");
        document_title("Pattern database heuristic");
        document_synopsis("TODO");

        add_option<shared_ptr<PatternGenerator>>(
            "pattern",
            "pattern generation method",
            "greedy()");
        add_heuristic_options_to_feature(*this, "pdb");

        document_language_support("action costs", "supported");
        document_language_support("conditional effects", "not supported");
        document_language_support("axioms", "not supported");

        document_property("admissible", "yes");
        document_property("consistent", "yes");
        document_property("safe", "yes");
        document_property("preferred operators", "no");
    }

    shared_ptr<TaskDependentFactory<Evaluator>>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<PDBHeuristicFactory>(
            get_heuristic_arguments_from_options(opts),
            opts.get<shared_ptr<PatternGenerator>>("pattern"));
    }
};
} // namespace

namespace downward::cli::heuristics {

void add_pdb_heuristic_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<PDBHeuristicFeature>();
}

} // namespace downward::cli::heuristics
