#include "additivity_none.h"

#include "../../../../../option_parser.h"
#include "../../../../../plugin.h"

namespace probabilistic {
namespace pdbs {
namespace additivity {

std::shared_ptr<std::vector<PatternClique>>
AdditivityNone::compute_additive_subcollections(
    const PatternCollection& patterns)
{
    std::shared_ptr<std::vector<PatternClique>> additive_subcollections(
        new std::vector<PatternClique>(patterns.size()));

    int size = static_cast<int>(patterns.size());
    for (int i = 0; i < size; ++i) {
        additive_subcollections->push_back({i});
    }

    return additive_subcollections;
}

static std::shared_ptr<AdditivityNone> _parse(OptionParser& parser)
{
    if (parser.dry_run()) return nullptr;

    return std::make_shared<AdditivityNone>();
}

static Plugin<AdditivityStrategy> _plugin("none", _parse);

} // namespace additivity
} // namespace pdbs
} // namespace probabilistic