#include "downward/cli/evaluators/subcategory.h"

#include "language/ast/compilation_context.h"
#include "language/ast/internal_function_definition.h"

#include "language/documentation/topic.h"

using namespace language::parser;
using namespace language::documentation;

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

DocumentationTopic& add_evaluator_subcategory(CompilationContext& registry)
{
    return registry.insert_documentation_topic<EvaluatorGroupPlugin>();
}

} // namespace downward::cli::evaluators
