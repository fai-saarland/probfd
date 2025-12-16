#include "downward/cli/heuristics/ff_heuristic_feature.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/cli/heuristics/heuristic_options.h"

#include "downward/heuristics/ff_heuristic.h"

#include "downward/utils/logging.h"

#include "downward/task_dependent_factory.h"
#include "downward/task_transformation.h"

using namespace std;
using namespace downward;
using namespace downward::ff_heuristic;
using namespace downward::utils;

using namespace language::plugins;

using downward::cli::add_heuristic_options_to_feature;

namespace {
class FFHeuristicFactory : public TaskDependentFactory<Evaluator> {
    std::shared_ptr<TaskTransformation> transformation;
    bool cache_estimates;
    std::string description;
    utils::Verbosity verbosity;

public:
    FFHeuristicFactory(
        shared_ptr<TaskTransformation> transformation,
        bool cache_estimates,
        string description,
        utils::Verbosity verbosity)
        : transformation(std::move(transformation))
        , cache_estimates(cache_estimates)
        , description(std::move(description))
        , verbosity(verbosity)
    {
    }

    unique_ptr<Evaluator> create_object(const SharedAbstractTask& task) override
    {
        auto transformation_result = transformation->transform(task);
        return std::make_unique<FFHeuristic>(
            task,
            std::move(transformation_result),
            cache_estimates,
            description,
            verbosity);
    }
};

InternalFunctionDefinitionBase& add_ff_heuristic_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "ff",
        &language::plugins::construct_shared<
            TaskDependentFactory<Evaluator>,
            FFHeuristicFactory,
            shared_ptr<TaskTransformation>,
            bool,
            string,
            Verbosity>);

    f.document_title("FF heuristic");

    f.document_language_support("action costs", "supported");
    f.document_language_support("conditional effects", "supported");
    f.document_language_support(
        "axioms",
        "supported (in the sense that the planner won't complain -- "
        "handling of axioms might be very stupid "
        "and even render the heuristic unsafe)");

    f.document_property("admissible", "no");
    f.document_property("consistent", "no");
    f.document_property("safe", "yes for tasks without axioms");
    f.document_property("preferred operators", "yes");

    add_heuristic_options_to_feature(f, "ff", 0);

    return f;
}

} // namespace

namespace downward::cli::heuristics {

void add_ff_heuristic_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_ff_heuristic_to_namespace(n);
}

} // namespace downward::cli::heuristics
