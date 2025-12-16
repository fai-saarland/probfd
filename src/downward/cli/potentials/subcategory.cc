#include "downward/cli/potentials/subcategory.h"

#include "language/plugins/internal_function_definition.h"
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

void add_potential_heuristics_subcategory(language::plugins::Registry& registry)
{
    registry.insert_documentation_topic<PotentialHeuristicsGroupPlugin>();
}

}
