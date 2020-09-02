#include "operator_id_tiebreaker.h"

#include "../../option_parser.h"
#include "../../plugin.h"
#include "../probabilistic_operator.h"

#include <limits>

namespace probabilistic {
namespace policy_tiebreaking {

OperatorIdTiebreaker::OperatorIdTiebreaker(const options::Options& opts)
    : ascending_(opts.get<bool>("prefer_smaller") ? 1 : -1)
{
}

void
OperatorIdTiebreaker::add_options_to_parser(options::OptionParser& p)
{
    p.add_option<bool>("prefer_smaller", "", "true");
}

unsigned
OperatorIdTiebreaker::choose(
    const GlobalState& ,
    const std::vector<
        std::pair<const ProbabilisticOperator*, Distribution<GlobalState>>>&
        choices)
{
    int min_id = std::numeric_limits<int>::max();
    unsigned min_idx = -1;
    for (int i = choices.size() - 1; i >= 0; i--) {
        int id = static_cast<int>(choices[i].first->get_id()) * ascending_;
        if (id < min_id) {
            min_id = id;
            min_idx = i;
        }
    }
    return min_idx;
}

unsigned
OperatorIdTiebreaker::choose(
    const QuotientState& ,
    const std::vector<std::pair<QuotientAction, Distribution<QuotientState>>>&
        choices)
{
    int min_id = std::numeric_limits<int>::max();
    unsigned min_idx = -1;
    for (int i = choices.size() - 1; i >= 0; i--) {
        int id =
            static_cast<int>(choices[i].first.second->get_id()) * ascending_;
        if (id < min_id) {
            min_id = id;
            min_idx = i;
        }
    }
    return min_idx;
}

static Plugin<PolicyChooser> _plugin(
    "operator_id_tiebreaker",
    options::parse<PolicyChooser, OperatorIdTiebreaker>);

} // namespace policy_tiebreaking
} // namespace probabilistic
