#include "vdiff_tiebreaker.h"

#include "../../option_parser.h"
#include "../../plugin.h"
#include "../algorithms/heuristic_search_base.h"

namespace probabilistic {
namespace policy_tiebreaking {

using StateInfo =
    algorithms::heuristic_search_base::HeuristicSearchStateInformation<
        std::true_type>;

VDiffTiebreaker::VDiffTiebreaker(const options::Options&) { }

void
VDiffTiebreaker::add_options_to_parser(options::OptionParser&)
{
}

unsigned
VDiffTiebreaker::choose(
    const GlobalState&,
    const ProbabilisticOperator* prev,
    const std::vector<
        std::pair<const ProbabilisticOperator*, Distribution<GlobalState>>>&
        choices)
{
    value_type::value_t best = value_type::inf;
    unsigned choice = 1;
    const auto* ptr = &choices[0];
    for (int i = choices.size(); i > 0; --i, ++ptr) {
        value_type::value_t sum = value_type::zero;
        for (auto it = ptr->second.begin(); it != ptr->second.end(); ++it) {
            const StateInfo* info = static_cast<const StateInfo*>(
                hs_interface_->lookup_state_info(it->first));
            sum += it->second * value_type::abs(info->value - info->value2);
        }
        if (value_type::approx_less()(sum, best)) {
            best = sum;
            choice = i;
        } else if (
            value_type::approx_equal()(sum, best) && ptr->first == prev) {
            choice = i;
        }
    }
    return choices.size() - choice;
}

unsigned
VDiffTiebreaker::choose(
    const QuotientState&,
    const QuotientAction& prev,
    const std::vector<std::pair<QuotientAction, Distribution<QuotientState>>>&
        choices)
{
    value_type::value_t best = value_type::inf;
    unsigned choice = 1;
    const auto* ptr = &choices[0];
    for (int i = choices.size(); i > 0; --i, ++ptr) {
        value_type::value_t sum = value_type::zero;
        for (auto it = ptr->second.begin(); it != ptr->second.end(); ++it) {
            const StateInfo* info = static_cast<const StateInfo*>(
                hs_interface_->lookup_state_info(it->first));
            sum += it->second * value_type::abs(info->value - info->value2);
        }
        if (value_type::approx_less()(sum, best)) {
            best = sum;
            choice = i;
        } else if (
            value_type::approx_equal()(sum, best) && ptr->first == prev) {
            choice = i;
        }
    }
    return choices.size() - choice;
}

static Plugin<PolicyChooser>
    _plugin("vdiff_tiebreaker", options::parse<PolicyChooser, VDiffTiebreaker>);

} // namespace policy_tiebreaking
} // namespace probabilistic
