#include "arbitrary_tiebreaker.h"

#include "../../option_parser.h"
#include "../../plugin.h"

namespace probabilistic {
namespace policy_tiebreaking {

ArbitraryTiebreaker::ArbitraryTiebreaker() { }

ArbitraryTiebreaker::ArbitraryTiebreaker(const options::Options&) { }

void
ArbitraryTiebreaker::add_options_to_parser(options::OptionParser&)
{
}

unsigned
ArbitraryTiebreaker::choose(
    const GlobalState&,
    const std::vector<
        std::pair<const ProbabilisticOperator*, Distribution<GlobalState>>>&)
{
    return 0;
}

unsigned
ArbitraryTiebreaker::choose(
    const QuotientState&,
    const std::vector<std::pair<QuotientAction, Distribution<QuotientState>>>&)
{
    return 0;
}

static Plugin<PolicyChooser> _plugin(
    "arbitrary_tiebreaker",
    options::parse<PolicyChooser, ArbitraryTiebreaker>);

} // namespace policy_tiebreaking
} // namespace probabilistic
