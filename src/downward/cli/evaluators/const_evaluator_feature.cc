#include "downward/cli/evaluators/const_evaluator_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

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
        if (value < 0) {
            throw std::domain_error("constant value must be >= 0.");
        }
    }

    unique_ptr<Evaluator> create_object(const SharedAbstractTask&) override
    {
        return std::make_unique<ConstEvaluator>(value, description, verbosity);
    }
};

class ConstEvaluatorFeature
    : public SharedTypedFeature<
          TaskDependentFactory<Evaluator>,
          std::string,
          Verbosity,
          int> {
public:
    ConstEvaluatorFeature()
        : TypedFeature("const", &ConstEvaluatorFeature::func)
    {
        document_title("Constant evaluator");
        document_synopsis("Returns a constant value.");

        make_optional_argument_with_default(
            0,
            "value",
            "1",
            "the constant value");
        add_evaluator_options_to_feature(*this, "const", 1);
    }

    static shared_ptr<TaskDependentFactory<Evaluator>> func(
        const Context&,
        std::string description,
        Verbosity verbosity,
        int value)
    {
        return make_shared_from_arg_tuples<ConstEvaluatorFactory>(
            std::move(description),
            verbosity,
            value);
    }
};
} // namespace

namespace downward::cli::evaluators {

void add_const_evaluator_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    const Feature& f = n.insert_feature_plugin<ConstEvaluatorFeature>();

    SubcategoryPlugin& subcategory =
        registry.get_subcategory_plugin("evaluators_basic");
    subcategory.add_feature(f);
}

} // namespace downward::cli::evaluators
