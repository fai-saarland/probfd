#include "downward/cli/evaluators/pref_evaluator_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

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

    unique_ptr<Evaluator> create_object(const SharedAbstractTask&) override
    {
        return std::make_unique<PrefEvaluator>(description, verbosity);
    }
};

Feature& add_pref_evaluator_feature_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_typed_feature_plugin(
        "pref",
        &cli::plugins::make_shared<
            TaskDependentFactory<Evaluator>,
            PrefEvaluatorFactory,
            std::string,
            Verbosity>);
    f.document_title("Preference evaluator");
    f.document_synopsis("Returns 0 if preferred is true and 1 otherwise.");
    add_evaluator_options_to_feature(f, "pref", 0);

    return f;
}

} // namespace

namespace downward::cli::evaluators {

void add_pref_evaluator_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    const Feature& f = add_pref_evaluator_feature_to_namespace(n);
    SubcategoryPlugin& subcategory =
        registry.get_subcategory_plugin("evaluators_basic");
    subcategory.add_feature(f);
}

} // namespace downward::cli::evaluators
