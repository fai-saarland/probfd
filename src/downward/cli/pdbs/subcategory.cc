#include "downward/cli/pdbs/subcategory.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

using namespace language::plugins;

namespace {

class PDBGroupPlugin : public DocumentationTopic {
public:
    PDBGroupPlugin()
        : DocumentationTopic("heuristics_pdb")
    {
        document_title("Pattern Database Heuristics");
    }
};

} // namespace

namespace downward::cli::pdbs {

void add_pdb_heuristic_subcategory(Registry& registry)
{
    registry.insert_documentation_topic<PDBGroupPlugin>();
}

} // namespace downward::cli::pdbs
