#include "downward/cli/plugins/plugin.h"

#include "downward/cli/evaluator_options.h"

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

    unique_ptr<Evaluator>
    create_object(const SharedAbstractTask&) override
    {
        return std::make_unique<GEvaluator>(description, verbosity);
    }
};

class GEvaluatorFeature
    : public TypedFeature<TaskDependentFactory<Evaluator>, GEvaluatorFactory> {
public:
    GEvaluatorFeature()
        : TypedFeature("g")
    {
        document_subcategory("evaluators_basic");
        document_title("g-value evaluator");
        document_synopsis(
            "Returns the g-value (path cost) of the search node.");
        add_evaluator_options_to_feature(*this, "g");
    }

    virtual shared_ptr<GEvaluatorFactory>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<GEvaluatorFactory>(
            get_evaluator_arguments_from_options(opts));
    }
};

FeaturePlugin<GEvaluatorFeature> _plugin;

} // namespace
