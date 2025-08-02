#include "downward/cli/heuristics/cg_heuristic_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/cli/heuristics/heuristic_options.h"

#include "downward/heuristics/cg_heuristic.h"

#include "downward/utils/logging.h"

#include "downward/task_dependent_factory.h"
#include "downward/task_transformation.h"

using namespace std;
using namespace downward;
using namespace downward::cg_heuristic;
using namespace downward::utils;

using namespace downward::cli::plugins;

using downward::cli::add_heuristic_options_to_feature;
using downward::cli::get_heuristic_arguments_from_options;

namespace {
class CGHeuristicFactory : public TaskDependentFactory<Evaluator> {
    std::shared_ptr<TaskTransformation> transformation;
    bool cache_estimates;
    std::string description;
    utils::Verbosity verbosity;
    int max_cache_size;

public:
    CGHeuristicFactory(
        shared_ptr<TaskTransformation> transformation,
        bool cache_estimates,
        string description,
        utils::Verbosity verbosity,
        int max_cache_size)
        : transformation(std::move(transformation))
        , cache_estimates(cache_estimates)
        , description(std::move(description))
        , verbosity(verbosity)
        , max_cache_size(max_cache_size)
    {
    }

    unique_ptr<Evaluator> create_object(const SharedAbstractTask& task) override
    {
        auto transformation_result = transformation->transform(task);
        return std::make_unique<CGHeuristic>(
            max_cache_size,
            task,
            std::move(transformation_result),
            cache_estimates,
            description,
            verbosity);
    }
};

class CGHeuristicFeature
    : public TypedFeature<TaskDependentFactory<Evaluator>, CGHeuristicFactory> {
public:
    CGHeuristicFeature()
        : TypedFeature("cg")
    {
        document_title("Causal graph heuristic");

        add_option<int>(
            "max_cache_size",
            "maximum number of cached entries per variable (set to 0 to "
            "disable cache)",
            "1000000",
            Bounds("0", "infinity"));
        add_heuristic_options_to_feature(*this, "cg");

        document_language_support("action costs", "supported");
        document_language_support("conditional effects", "supported");
        document_language_support(
            "axioms",
            "supported (in the sense that the planner won't complain -- "
            "handling of axioms might be very stupid "
            "and even render the heuristic unsafe)");

        document_property("admissible", "no");
        document_property("consistent", "no");
        document_property("safe", "no");
        document_property("preferred operators", "yes");
    }

    shared_ptr<CGHeuristicFactory>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<CGHeuristicFactory>(
            get_heuristic_arguments_from_options(opts),
            opts.get<int>("max_cache_size"));
    }
};
}

namespace downward::cli::heuristics {

void add_cg_heuristic_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<CGHeuristicFeature>();
}

} // namespace
