#include "downward_plugins/plugins/plugin.h"

namespace {

class PotentialHeuristicsGroupPlugin
    : public downward_plugins::plugins::SubcategoryPlugin {
public:
    PotentialHeuristicsGroupPlugin()
        : SubcategoryPlugin("heuristics_potentials")
    {
        document_title("Potential Heuristics");
    }
} _subcategory_plugin;

} // namespace
