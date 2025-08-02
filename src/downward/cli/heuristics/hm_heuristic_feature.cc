#include "downward/cli/heuristics/hm_heuristic_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/cli/heuristics/heuristic_options.h"

#include "downward/heuristics/hm_heuristic.h"

#include "downward/utils/logging.h"

#include "downward/task_dependent_factory.h"
#include "downward/task_transformation.h"

using namespace std;
using namespace downward;
using namespace downward::hm_heuristic;
using namespace downward::utils;

using namespace downward::cli::plugins;

using downward::cli::add_heuristic_options_to_feature;
using downward::cli::get_heuristic_arguments_from_options;

namespace {
class HMHeuristicFactory : public TaskDependentFactory<Evaluator> {
    std::shared_ptr<TaskTransformation> transformation;
    bool cache_estimates;
    std::string description;
    utils::Verbosity verbosity;
    int m;

public:
    HMHeuristicFactory(
        shared_ptr<TaskTransformation> transformation,
        bool cache_estimates,
        string description,
        utils::Verbosity verbosity,
        int m)
        : transformation(std::move(transformation))
        , cache_estimates(cache_estimates)
        , description(std::move(description))
        , verbosity(verbosity)
        , m(m)
    {
    }

    unique_ptr<Evaluator>
    create_object(const SharedAbstractTask& task) override
    {
        auto transformation_result = transformation->transform(task);
        return std::make_unique<HMHeuristic>(
            m,
            task,
            std::move(transformation_result),
            cache_estimates,
            description,
            verbosity);
    }
};

class HMHeuristicFeature
    : public TypedFeature<TaskDependentFactory<Evaluator>, HMHeuristicFactory> {
public:
    HMHeuristicFeature()
        : TypedFeature("hm")
    {
        document_title("h^m heuristic");

        add_option<int>("m", "subset size", "2", Bounds("1", "infinity"));
        add_heuristic_options_to_feature(*this, "hm");

        document_language_support("action costs", "supported");
        document_language_support("conditional effects", "ignored");
        document_language_support("axioms", "ignored");

        document_property(
            "admissible",
            "yes for tasks without conditional effects or axioms");
        document_property(
            "consistent",
            "yes for tasks without conditional effects or axioms");
        document_property(
            "safe",
            "yes for tasks without conditional effects or axioms");
        document_property("preferred operators", "no");
    }

    shared_ptr<HMHeuristicFactory>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<HMHeuristicFactory>(
            get_heuristic_arguments_from_options(opts),
            opts.get<int>("m"));
    }
};
}

namespace downward::cli::heuristics {

void add_hm_heuristic_features(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<HMHeuristicFeature>();
}

} // namespace
