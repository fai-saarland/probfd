#include "downward/cli/evaluators/subcategory.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

using namespace downward::cli::plugins;

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

void add_evaluator_subcategory(Registry& raw_registry)
{
    raw_registry.insert_subcategory_plugin<EvaluatorGroupPlugin>();
}

} // namespace
