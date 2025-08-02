#include "downward/cli/pdbs/subcategory.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

using namespace downward::cli::plugins;

namespace {

class PDBGroupPlugin : public SubcategoryPlugin {
public:
    PDBGroupPlugin()
        : SubcategoryPlugin("heuristics_pdb")
    {
        document_title("Pattern Database Heuristics");
    }
};

} // namespace

namespace downward::cli::pdbs {

void add_pdb_heuristic_subcategory(RawRegistry& raw_registry)
{
    raw_registry.insert_subcategory_plugin<PDBGroupPlugin>();
}

} // namespace downward::cli::pdbs
