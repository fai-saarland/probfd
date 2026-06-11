#include "downward_cli/potentials/subcategory.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

using namespace language;

namespace {

class PotentialHeuristicsGroupPlugin
    : public language::plugins::SubcategoryPlugin {
public:
    PotentialHeuristicsGroupPlugin()
        : SubcategoryPlugin("heuristics_potentials")
    {
        document_title("Potential Heuristics");
    }
};

} // namespace

namespace downward::cli::potentials {

void add_potential_heuristics_subcategory(plugins::RawRegistry& raw_registry)
{
    raw_registry.insert_subcategory_plugin<PotentialHeuristicsGroupPlugin>();
}

}
