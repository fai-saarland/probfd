#include "reduction_strategy.h"

#include "../../../../option_parser.h"
#include "../../../../plugin.h"

#include <memory>

namespace probabilistic {
namespace pdbs {

static std::shared_ptr<CombinationStrategy> _parse_add(OptionParser& parser)
{
    if (parser.dry_run()) return nullptr;
    return std::make_shared<ReductionStrategy<std::plus<value_type::value_t>>>(
        value_type::zero);
}

static std::shared_ptr<CombinationStrategy> _parse_mul(OptionParser& parser)
{
    if (parser.dry_run()) return nullptr;
    return std::make_shared<
        ReductionStrategy<std::multiplies<value_type::value_t>>>(
        value_type::one);
}

static Plugin<CombinationStrategy>
    _plugin_add("combinator_additivity", _parse_add);
static Plugin<CombinationStrategy>
    _plugin_mul("combinator_multiplicativity", _parse_mul);

} // namespace pdbs
} // namespace probabilistic
