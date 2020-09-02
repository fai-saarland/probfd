#include "hbased_tiebreaker.h"

#include "../../option_parser.h"
#include "../../plugin.h"
#include "../new_state_handlers/store_heuristic.h"

namespace probabilistic {
namespace policy_tiebreaking {

HBasedTiebreaker::HBasedTiebreaker(const options::Options& opts)
    : heuristic_(std::dynamic_pointer_cast<new_state_handlers::StoreHeuristic>(
        opts.get<std::shared_ptr<NewGlobalStateHandler>>("heuristic")))
{
}

void
HBasedTiebreaker::add_options_to_parser(options::OptionParser& p)
{
    p.add_option<std::shared_ptr<NewGlobalStateHandler>>("heuristic");
}

unsigned
HBasedTiebreaker::choose(
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
            const int h = heuristic_->get_cached_h_value(it->first);
            if (h < 0) {
                sum = value_type::inf;
                break;
            }
            sum += it->second * h;
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
HBasedTiebreaker::choose(
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
            const int h = heuristic_->get_cached_h_value(it->first);
            if (h < 0) {
                sum = value_type::inf;
                break;
            }
            sum += it->second * h;
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

static Plugin<PolicyChooser> _plugin(
    "hbased_tiebreaker",
    options::parse<PolicyChooser, HBasedTiebreaker>);

} // namespace policy_tiebreaking
} // namespace probabilistic
