#include "probfd/heuristics/pdbs/pattern_generator.h"

#include "plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

std::shared_ptr<utils::Printable> PatternCollectionGenerator::get_report() const
{
    return nullptr;
};

static PluginTypePlugin<PatternCollectionGenerator> _type_plugin(
    "PatternCollectionGeneratorProbabilistic",
    "Factory for pattern collections in probabilistic setting");

} // namespace pdbs
} // namespace heuristics
} // namespace probfd