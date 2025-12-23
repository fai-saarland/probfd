#include "downward/cli/heuristics/hm_heuristic_feature.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/cli/heuristics/heuristic_options.h"

#include "downward/heuristics/hm_heuristic.h"

#include "downward/utils/logging.h"

#include "downward/task_dependent_factory.h"
#include "downward/task_transformation.h"

using namespace std;
using namespace downward;
using namespace downward::hm_heuristic;
using namespace downward::utils;

using namespace language::plugins;

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

} // namespace

namespace downward::cli::heuristics {

InternalFunctionDefinitionBase& add_hm_heuristic_features(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "hm",
        &language::plugins::construct_shared<
            TaskDependentFactory<Evaluator>,
            HMHeuristicFactory,
            shared_ptr<TaskTransformation>,
            bool,
            string,
            Verbosity,
            int>);

    f.document_title("h^m heuristic");

    f.document_language_support("action costs", "supported");
    f.document_language_support("conditional effects", "ignored");
    f.document_language_support("axioms", "ignored");

    f.document_property(
        "admissible",
        "yes for tasks without conditional effects or axioms");
    f.document_property(
        "consistent",
        "yes for tasks without conditional effects or axioms");
    f.document_property(
        "safe",
        "yes for tasks without conditional effects or axioms");
    f.document_property("preferred operators", "no");

    f.make_optional_argument_with_default(0, "m", "2", "subset size");
    add_heuristic_options_to_feature(f, "hm", 1);

    return f;
}

} // namespace downward::cli::heuristics
