#include "downward/cli/evaluators/const_evaluator_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/cli/evaluators/evaluator_options.h"

#include "downward/evaluators/const_evaluator.h"

#include "downward/utils/logging.h"

#include "downward/task_dependent_factory.h"

using namespace std;
using namespace downward;
using namespace downward::utils;
using namespace downward::const_evaluator;

using namespace downward::cli::plugins;

using downward::cli::add_evaluator_options_to_feature;
using downward::cli::get_evaluator_arguments_from_options;

namespace {
class ConstEvaluatorFactory : public TaskDependentFactory<Evaluator> {
    std::string description;
    Verbosity verbosity;
    int value;

public:
    ConstEvaluatorFactory(
        std::string description,
        Verbosity verbosity,
        int value)
        : description(std::move(description))
        , verbosity(verbosity)
        , value(value)
    {
    }

    unique_ptr<Evaluator> create_object(const SharedAbstractTask&) override
    {
        return std::make_unique<ConstEvaluator>(value, description, verbosity);
    }
};

class ConstEvaluatorFeature
    : public TypedFeature<TaskDependentFactory<Evaluator>> {
public:
    ConstEvaluatorFeature()
        : TypedFeature("const")
    {
        document_subcategory("evaluators_basic");
        document_title("Constant evaluator");
        document_synopsis("Returns a constant value.");

        add_option<int>(
            "value",
            "the constant value",
            "1",
            Bounds("0", "infinity"));
        add_evaluator_options_to_feature(*this, "const");
    }

    shared_ptr<TaskDependentFactory<Evaluator>>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<ConstEvaluatorFactory>(
            get_evaluator_arguments_from_options(opts),
            opts.get<int>("value"));
    }
};
} // namespace

namespace downward::cli::evaluators {

void add_const_evaluator_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<ConstEvaluatorFeature>();
}

} // namespace downward::cli::evaluators
