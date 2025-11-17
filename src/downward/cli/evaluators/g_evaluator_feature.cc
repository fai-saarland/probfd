#include "downward/cli/evaluators/g_evaluator_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/evaluators/evaluator_options.h"

#include "downward/evaluators/g_evaluator.h"

#include "downward/utils/logging.h"

#include "downward/task_dependent_factory.h"

using namespace std;
using namespace downward;
using namespace downward::utils;
using namespace downward::g_evaluator;

using namespace downward::cli;
using namespace downward::cli::plugins;

namespace {

class GEvaluatorFactory : public TaskDependentFactory<Evaluator> {
    std::string description;
    Verbosity verbosity;

public:
    GEvaluatorFactory(std::string description, Verbosity verbosity)
        : description(std::move(description))
        , verbosity(verbosity)
    {
    }

    unique_ptr<Evaluator> create_object(const SharedAbstractTask&) override
    {
        return std::make_unique<GEvaluator>(description, verbosity);
    }
};

class GEvaluatorFeature
    : public SharedTypedFeature<
          TaskDependentFactory<Evaluator>,
          std::string,
          Verbosity> {
public:
    GEvaluatorFeature()
        : TypedFeature("g", &GEvaluatorFeature::func)
    {
        document_title("g-value evaluator");
        document_synopsis(
            "Returns the g-value (path cost) of the search node.");
        add_evaluator_options_to_feature(*this, "g", 0);
    }

    static shared_ptr<TaskDependentFactory<Evaluator>>
    func(const Context&, std::string description, Verbosity verbosity)
    {
        return make_shared<GEvaluatorFactory>(
            std::move(description),
            verbosity);
    }
};

} // namespace

namespace downward::cli::evaluators {

void add_g_evaluator_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    const Feature& f = n.insert_feature_plugin<GEvaluatorFeature>();
    SubcategoryPlugin& subcategory =
        registry.get_subcategory_plugin("evaluators_basic");
    subcategory.add_feature(f);
}

} // namespace downward::cli::evaluators
