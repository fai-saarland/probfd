#include "downward/cli/heuristics/hm_heuristic_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

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
        if (m < 1) { throw std::domain_error("m must be >= 1."); }
    }

    unique_ptr<Evaluator> create_object(const SharedAbstractTask& task) override
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
    : public SharedTypedFeature<
          TaskDependentFactory<Evaluator>,
          shared_ptr<TaskTransformation>,
          bool,
          string,
          utils::Verbosity,
          int> {
public:
    HMHeuristicFeature()
        : TypedFeature("hm", &HMHeuristicFeature::func)
    {
        document_title("h^m heuristic");

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

        make_optional_argument_with_default(0, "m", "2", "subset size");
        add_heuristic_options_to_feature(*this, "hm", 1);
    }

    static shared_ptr<TaskDependentFactory<Evaluator>> func(
        const Context&,
        shared_ptr<TaskTransformation> transformation,
        bool cache_estimates,
        string description,
        utils::Verbosity verbosity,
        int m)
    {
        return make_shared_from_arg_tuples<HMHeuristicFactory>(
            std::move(transformation),
            cache_estimates,
            std::move(description),
            verbosity,
            m);
    }
};
} // namespace

namespace downward::cli::heuristics {

void add_hm_heuristic_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<HMHeuristicFeature>();
}

} // namespace downward::cli::heuristics
