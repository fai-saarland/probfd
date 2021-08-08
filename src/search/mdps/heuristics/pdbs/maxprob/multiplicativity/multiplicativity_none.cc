#include "multiplicativity_none.h"

#include "../../../../../option_parser.h"
#include "../../../../../plugin.h"

namespace probabilistic {
namespace pdbs {
namespace maxprob {
namespace multiplicativity {

std::shared_ptr<std::vector<PatternClique>>
MultiplicativityNone::compute_multiplicative_subcollections(
    const PatternCollection& patterns)
{
    std::shared_ptr<std::vector<PatternClique>> subcollections(
        new std::vector<PatternClique>(patterns.size()));

    int size = static_cast<int>(patterns.size());
    for (int i = 0; i < size; ++i) {
        subcollections->push_back({i});
    }

    return subcollections;
}

static std::shared_ptr<MultiplicativityNone> _parse(OptionParser& parser)
{
    if (parser.dry_run()) return nullptr;

    return std::make_shared<MultiplicativityNone>();
}

static Plugin<MultiplicativityStrategy> _plugin("none", _parse);

} // namespace multiplicativity
} // namespace maxprob
} // namespace pdbs
} // namespace probabilistic