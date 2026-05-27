#include "downward/cli/potentials/subcategory.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

namespace {

class PotentialHeuristicsGroupPlugin
    : public downward::cli::plugins::SubcategoryPlugin {
public:
    PotentialHeuristicsGroupPlugin()
        : SubcategoryPlugin("heuristics_potentials")
    {
        document_title("Potential Heuristics");
    }
};

} // namespace

namespace downward::cli::potentials {

void add_potential_heuristics_subcategory(plugins::Registry& registry)
{
    plugins::Namespace& n = registry.get_global_name_space();
    n.insert_subcategory_plugin<PotentialHeuristicsGroupPlugin>();
}

}
