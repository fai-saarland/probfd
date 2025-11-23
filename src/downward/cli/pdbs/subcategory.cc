#include "downward/cli/pdbs/subcategory.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

using namespace downward::cli::plugins;

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
