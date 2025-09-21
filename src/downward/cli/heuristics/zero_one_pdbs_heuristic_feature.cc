#include "downward/cli/heuristics/zero_one_pdbs_heuristic_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/cli/heuristics/heuristic_options.h"

#include "downward/pdbs/zero_one_pdbs_heuristic.h"

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
class ZOPDBsHeuristicFactory : public TaskDependentFactory<Evaluator> {
    std::shared_ptr<TaskTransformation> transformation;
    bool cache_estimates;
    std::string description;
    utils::Verbosity verbosity;
    std::shared_ptr<PatternCollectionGenerator> generator;

public:
    ZOPDBsHeuristicFactory(
        shared_ptr<TaskTransformation> transformation,
        bool cache_estimates,
        string description,
        utils::Verbosity verbosity,
        std::shared_ptr<PatternCollectionGenerator> generator)
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

        return std::make_unique<ZeroOnePDBsHeuristic>(
            std::move(generator),
            task,
            std::move(transformation_result),
            cache_estimates,
            description,
            verbosity);
    }
};

class ZeroOnePDBsHeuristicFeature
    : public TypedFeature<TaskDependentFactory<Evaluator>> {
public:
    ZeroOnePDBsHeuristicFeature()
        : TypedFeature("zopdbs")
    {
        document_subcategory("heuristics_pdb");
        document_title("Zero-One PDB");
        document_synopsis(
            "The zero/one pattern database heuristic is simply the sum of the "
            "heuristic values of all patterns in the pattern collection. In "
            "contrast "
            "to the canonical pattern database heuristic, there is no need to "
            "check "
            "for additive subsets, because the additivity of the patterns is "
            "guaranteed by action cost partitioning. This heuristic uses the "
            "most "
            "simple form of action cost partitioning, i.e. if an operator "
            "affects "
            "more than one pattern in the collection, its costs are entirely "
            "taken "
            "into account for one pattern (the first one which it affects) and "
            "set "
            "to zero for all other affected patterns.");

        add_option<shared_ptr<PatternCollectionGenerator>>(
            "patterns",
            "pattern generation method",
            "systematic(1)");
        add_heuristic_options_to_feature(*this, "zopdbs");

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
        return make_shared_from_arg_tuples<ZOPDBsHeuristicFactory>(
            get_heuristic_arguments_from_options(opts),
            opts.get<shared_ptr<PatternCollectionGenerator>>("patterns"));
    }
};
} // namespace

namespace downward::cli::heuristics {

void add_zero_one_pdbs_heuristic_features(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<ZeroOnePDBsHeuristicFeature>();
}

} // namespace downward::cli::heuristics
