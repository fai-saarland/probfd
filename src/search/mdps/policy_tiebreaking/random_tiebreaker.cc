#include "random_tiebreaker.h"

#include "../../globals.h"
#include "../../option_parser.h"
#include "../../plugin.h"
#include "../../utils/rng.h"

namespace probabilistic {
namespace policy_tiebreaking {

RandomTiebreaker::RandomTiebreaker(const options::Options&) { }

void
RandomTiebreaker::add_options_to_parser(options::OptionParser&)
{
}

unsigned
RandomTiebreaker::choose(
    const GlobalState&,
    const ProbabilisticOperator*,
    const std::vector<
        std::pair<const ProbabilisticOperator*, Distribution<GlobalState>>>&
        options)
{
    return ::g_rng(options.size());
}

unsigned
RandomTiebreaker::choose(
    const QuotientState&,
    const QuotientAction&,
    const std::vector<std::pair<QuotientAction, Distribution<QuotientState>>>&
        options)
{
    return ::g_rng(options.size());
}

PersistentRandomTiebreaker::PersistentRandomTiebreaker(const options::Options&)
{
}

void
PersistentRandomTiebreaker::add_options_to_parser(options::OptionParser&)
{
}

unsigned
PersistentRandomTiebreaker::choose(
    const GlobalState&,
    const std::vector<
        std::pair<const ProbabilisticOperator*, Distribution<GlobalState>>>&
        options)
{
    return ::g_rng(options.size());
}

unsigned
PersistentRandomTiebreaker::choose(
    const QuotientState&,
    const std::vector<std::pair<QuotientAction, Distribution<QuotientState>>>&
        options)
{
    return ::g_rng(options.size());
}

static Plugin<PolicyChooser> _plugin(
    "random_tiebreaker",
    options::parse<PolicyChooser, RandomTiebreaker>);

static Plugin<PolicyChooser> _pluginp(
    "random_persistent_tiebreaker",
    options::parse<PolicyChooser, PersistentRandomTiebreaker>);

} // namespace policy_tiebreaking
} // namespace probabilistic
