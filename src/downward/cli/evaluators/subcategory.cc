#include "downward/cli/evaluators/subcategory.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

using namespace downward::cli::plugins;

namespace {
class EvaluatorGroupPlugin : public DocumentationTopic {
public:
    EvaluatorGroupPlugin()
        : DocumentationTopic("evaluators_basic")
    {
        document_title("Basic Evaluators");
    }
};
}

namespace downward::cli::evaluators {

void add_evaluator_subcategory(Registry& registry)
{
    registry.insert_documentation_topic<EvaluatorGroupPlugin>();
}

} // namespace
