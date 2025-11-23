#include "downward/cli/heuristics/cg_heuristic_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

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
        if (max_cache_size < 0) {
            throw std::domain_error("max_cache_size must be >= 0.");
        }
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

InternalFunctionDefinitionBase& add_causal_graph_heuristic_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "cg",
        &downward::cli::plugins::construct_shared<
            TaskDependentFactory<Evaluator>,
            CGHeuristicFactory,
            shared_ptr<TaskTransformation>,
            bool,
            string,
            Verbosity,
            int>);

    f.document_title("Causal graph heuristic");

    f.document_language_support("action costs", "supported");
    f.document_language_support("conditional effects", "supported");
    f.document_language_support(
        "axioms",
        "supported (in the sense that the planner won't complain -- "
        "handling of axioms might be very stupid "
        "and even render the heuristic unsafe)");

    f.document_property("admissible", "no");
    f.document_property("consistent", "no");
    f.document_property("safe", "no");
    f.document_property("preferred operators", "yes");

    f.make_optional_argument_with_default(
        0,
        "max_cache_size",
        "1000000",
        "maximum number of cached entries per variable (set to 0 to "
        "disable cache)");
    add_heuristic_options_to_feature(f, "cg", 1);

    return f;
}

} // namespace

namespace downward::cli::heuristics {

void add_cg_heuristic_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_causal_graph_heuristic_to_namespace(n);
}

} // namespace downward::cli::heuristics
