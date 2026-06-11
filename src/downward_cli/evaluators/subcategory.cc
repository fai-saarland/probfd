#include "downward_cli/evaluators/subcategory.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

using namespace language::plugins;

namespace {
class EvaluatorGroupPlugin : public SubcategoryPlugin {
public:
    EvaluatorGroupPlugin()
        : SubcategoryPlugin("evaluators_basic")
    {
        document_title("Basic Evaluators");
    }
};
}

namespace downward::cli::evaluators {

void add_evaluator_subcategory(RawRegistry& raw_registry)
{
    raw_registry.insert_subcategory_plugin<EvaluatorGroupPlugin>();
}

} // namespace
