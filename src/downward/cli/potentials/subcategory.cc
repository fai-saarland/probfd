#include "downward/cli/plugins/plugin.h"

namespace {

class PotentialHeuristicsGroupPlugin
    : public downward::cli::plugins::SubcategoryPlugin {
public:
    PotentialHeuristicsGroupPlugin()
        : SubcategoryPlugin("heuristics_potentials")
    {
        document_title("Potential Heuristics");
    }
} _subcategory_plugin;

} // namespace
