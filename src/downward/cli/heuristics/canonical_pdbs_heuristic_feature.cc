#include "../../../../include/downward/cli/plugins/plugin.h"

#include "../../../../include/downward/cli/heuristic_options.h"

#include "../../../../include/downward/cli/pdbs/canonical_pdbs_heuristic_options.h"

#include "../../../../include/downward/pdbs/canonical_pdbs_heuristic.h"

#include "downward/task_dependent_factory.h"
#include "downward/task_transformation.h"

using namespace std;
using namespace downward;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::plugins;

using downward::cli::add_heuristic_options_to_feature;
using downward::cli::get_heuristic_arguments_from_options;

using downward::cli::pdbs::add_canonical_pdbs_options_to_feature;
using downward::cli::pdbs::get_canonical_pdbs_arguments_from_options;

namespace {

class CanonicalPDBsHeuristicFactory : public TaskDependentFactory<Evaluator> {
    std::shared_ptr<TaskTransformation> transformation;
    bool cache_estimates;
    std::string description;
    utils::Verbosity verbosity;
    std::shared_ptr<PatternCollectionGenerator> generator;
    double max_time_dominance_pruning;

public:
    CanonicalPDBsHeuristicFactory(
        shared_ptr<TaskTransformation> transformation,
        bool cache_estimates,
        string description,
        utils::Verbosity verbosity,
        std::shared_ptr<PatternCollectionGenerator> generator,
        double max_time_dominance_pruning)
        : transformation(std::move(transformation))
        , cache_estimates(cache_estimates)
        , description(std::move(description))
        , verbosity(verbosity)
        , generator(std::move(generator))
        , max_time_dominance_pruning(max_time_dominance_pruning)
    {
    }

    unique_ptr<Evaluator>
    create_object(const SharedAbstractTask& task) override
    {
        auto transformation_result = transformation->transform(task);

        return std::make_unique<CanonicalPDBsHeuristic>(
            std::move(generator),
            max_time_dominance_pruning,
            task,
            std::move(transformation_result),
            cache_estimates,
            description,
            verbosity);
    }
};

class CanonicalPDBsHeuristicFeature
    : public TypedFeature<
          TaskDependentFactory<Evaluator>,
          CanonicalPDBsHeuristicFactory> {
public:
    CanonicalPDBsHeuristicFeature()
        : TypedFeature("cpdbs")
    {
        document_subcategory("heuristics_pdb");
        document_title("Canonical PDB");
        document_synopsis(
            "The canonical pattern database heuristic is calculated as "
            "follows. "
            "For a given pattern collection C, the value of the "
            "canonical heuristic function is the maximum over all "
            "maximal additive subsets A in C, where the value for one subset "
            "S in A is the sum of the heuristic values for all patterns in S "
            "for a given state.");

        add_option<shared_ptr<PatternCollectionGenerator>>(
            "patterns",
            "pattern generation method",
            "systematic(1)");
        add_canonical_pdbs_options_to_feature(*this);
        add_heuristic_options_to_feature(*this, "cpdbs");

        document_language_support("action costs", "supported");
        document_language_support("conditional effects", "not supported");
        document_language_support("axioms", "not supported");

        document_property("admissible", "yes");
        document_property("consistent", "yes");
        document_property("safe", "yes");
        document_property("preferred operators", "no");
    }

    shared_ptr<CanonicalPDBsHeuristicFactory>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<CanonicalPDBsHeuristicFactory>(
            get_heuristic_arguments_from_options(opts),
            opts.get<shared_ptr<PatternCollectionGenerator>>("patterns"),
            get_canonical_pdbs_arguments_from_options(opts));
    }
};

FeaturePlugin<CanonicalPDBsHeuristicFeature> _plugin;

} // namespace
