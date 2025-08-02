#include "downward/cli/evaluators/pref_evaluator_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/cli/evaluators/evaluator_options.h"

#include "downward/evaluators/pref_evaluator.h"

#include "downward/utils/logging.h"

#include "downward/task_dependent_factory.h"

using namespace std;
using namespace downward;
using namespace downward::pref_evaluator;
using namespace downward::utils;

using namespace downward::cli::plugins;

using downward::cli::add_evaluator_options_to_feature;
using downward::cli::get_evaluator_arguments_from_options;

namespace {

class PrefEvaluatorFactory : public TaskDependentFactory<Evaluator> {
    std::string description;
    Verbosity verbosity;

public:
    PrefEvaluatorFactory(std::string description, Verbosity verbosity)
        : description(std::move(description))
        , verbosity(verbosity)
    {
    }

    unique_ptr<Evaluator>
    create_object(const SharedAbstractTask&) override
    {
        return std::make_unique<PrefEvaluator>(description, verbosity);
    }
};

class PrefEvaluatorFeature
    : public TypedFeature<
          TaskDependentFactory<Evaluator>,
          PrefEvaluatorFactory> {
public:
    PrefEvaluatorFeature()
        : TypedFeature("pref")
    {
        document_subcategory("evaluators_basic");
        document_title("Preference evaluator");
        document_synopsis("Returns 0 if preferred is true and 1 otherwise.");

        add_evaluator_options_to_feature(*this, "pref");
    }

    shared_ptr<PrefEvaluatorFactory>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<PrefEvaluatorFactory>(
            get_evaluator_arguments_from_options(opts));
    }
};

}

namespace downward::cli::evaluators {

void add_pref_evaluator_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<PrefEvaluatorFeature>();
}

} // namespace
