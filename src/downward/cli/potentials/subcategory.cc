#include "downward/cli/potentials/subcategory.h"

#include "language/plugins/registry.h"

namespace {

class PotentialHeuristicsGroupPlugin
    : public language::plugins::DocumentationTopic {
public:
    PotentialHeuristicsGroupPlugin()
        : DocumentationTopic("heuristics_potentials")
    {
        document_title("Potential Heuristics");
    }
};

} // namespace

namespace downward::cli::potentials {

language::plugins::DocumentationTopic&
add_potential_heuristics_subcategory(language::plugins::Registry& registry)
{
    return registry
        .insert_documentation_topic<PotentialHeuristicsGroupPlugin>();
}

} // namespace downward::cli::potentials
