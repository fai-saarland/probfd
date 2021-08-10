#include "trivial_finder.h"

#include "../../../../option_parser.h"
#include "../../../../plugin.h"

namespace probabilistic {
namespace pdbs {

std::shared_ptr<std::vector<PatternSubCollection>>
TrivialFinder::compute_subcollections(const PatternCollection& patterns)
{
    std::shared_ptr<std::vector<PatternSubCollection>> additive_subcollections(
        new std::vector<PatternSubCollection>(patterns.size()));

    int size = static_cast<int>(patterns.size());
    for (int i = 0; i < size; ++i) {
        additive_subcollections->push_back({i});
    }

    return additive_subcollections;
}

std::vector<PatternSubCollection>
TrivialFinder::compute_subcollections_with_pattern(
    const PatternCollection& patterns,
    const std::vector<PatternSubCollection>& known_pattern_cliques,
    const Pattern& new_pattern)
{
    return {}; // There are no cliques.
}

static std::shared_ptr<TrivialFinder> _parse(OptionParser& parser)
{
    if (parser.dry_run()) return nullptr;

    return std::make_shared<TrivialFinder>();
}

static Plugin<SubCollectionFinder> _plugin("finder_trivial", _parse);

} // namespace pdbs
} // namespace probabilistic