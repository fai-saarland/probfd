#include "downward/cli/potentials/subcategory.h"

#include "language/ast/compilation_context.h"

namespace {

class PotentialHeuristicsGroupPlugin
    : public language::documentation::DocumentationTopic {
public:
    PotentialHeuristicsGroupPlugin()
        : DocumentationTopic("heuristics_potentials")
    {
        document_title("Potential Heuristics");
    }
};

} // namespace

namespace downward::cli::potentials {

language::documentation::DocumentationTopic&
add_potential_heuristics_subcategory(
    language::parser::CompilationContext& registry)
{
    return registry
        .insert_documentation_topic<PotentialHeuristicsGroupPlugin>();
}

} // namespace downward::cli::potentials
