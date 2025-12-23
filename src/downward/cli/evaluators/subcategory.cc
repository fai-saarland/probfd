#include "downward/cli/evaluators/subcategory.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

using namespace language::plugins;

namespace {
class EvaluatorGroupPlugin : public DocumentationTopic {
public:
    EvaluatorGroupPlugin()
        : DocumentationTopic("evaluators_basic")
    {
        document_title("Basic Evaluators");
    }
};
} // namespace

namespace downward::cli::evaluators {

DocumentationTopic& add_evaluator_subcategory(Registry& registry)
{
    return registry.insert_documentation_topic<EvaluatorGroupPlugin>();
}

} // namespace downward::cli::evaluators
