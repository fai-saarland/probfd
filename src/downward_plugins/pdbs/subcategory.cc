#include "downward_plugins/plugins/plugin.h"

using namespace downward_plugins::plugins;

namespace {

class PDBGroupPlugin : public SubcategoryPlugin {
public:
    PDBGroupPlugin()
        : SubcategoryPlugin("heuristics_pdb")
    {
        document_title("Pattern Database Heuristics");
    }
} _subcategory_plugin;

} // namespace
