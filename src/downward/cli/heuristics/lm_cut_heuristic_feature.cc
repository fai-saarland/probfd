#include "downward/cli/heuristics/lm_cut_heuristic_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/heuristics/heuristic_options.h"

#include "downward/heuristics/lm_cut_heuristic.h"

#include "downward/utils/logging.h"

#include "downward/task_dependent_factory.h"
#include "downward/task_transformation.h"

using namespace std;
using namespace downward;
using namespace downward::lm_cut_heuristic;
using namespace downward::utils;

using namespace downward::cli::plugins;

using downward::cli::add_heuristic_options_to_feature;

namespace {
class LMCutHeuristicFactory : public TaskDependentFactory<Evaluator> {
    std::shared_ptr<TaskTransformation> transformation;
    bool cache_estimates;
    std::string description;
    utils::Verbosity verbosity;

public:
    LMCutHeuristicFactory(
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
        return std::make_unique<LandmarkCutHeuristic>(
            task,
            std::move(transformation_result),
            cache_estimates,
            description,
            verbosity);
    }
};

InternalFunctionDefinitionBase& add_landmark_cut_heuristic_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "lmcut",
        &downward::cli::plugins::construct_shared<
            TaskDependentFactory<Evaluator>,
            LMCutHeuristicFactory,
            shared_ptr<TaskTransformation>,
            bool,
            string,
            Verbosity>);

    f.document_title("Landmark-cut heuristic");

    f.document_language_support("action costs", "supported");
    f.document_language_support("conditional effects", "not supported");
    f.document_language_support("axioms", "not supported");

    f.document_property("admissible", "yes");
    f.document_property("consistent", "no");
    f.document_property("safe", "yes");
    f.document_property("preferred operators", "no");

    add_heuristic_options_to_feature(f, "lmcut", 0);

    return f;
}

} // namespace

namespace downward::cli::heuristics {

void add_landmark_cut_heuristic_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_landmark_cut_heuristic_to_namespace(n);
}

} // namespace downward::cli::heuristics
