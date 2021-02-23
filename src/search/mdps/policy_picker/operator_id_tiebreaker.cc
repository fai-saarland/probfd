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

int
OperatorIdTiebreaker::pick(
        const StateID& ,
        const ActionID& ,
        const std::vector<const ProbabilisticOperator*>& choices,
        const std::vector<Distribution<StateID>>& )
{
    int min_id = std::numeric_limits<int>::max();
    unsigned min_idx = -1;
    for (int i = choices.size() - 1; i >= 0; i--) {
        int id = static_cast<int>(choices[i]->get_id()) * ascending_;
        if (id < min_id) {
            min_id = id;
            min_idx = i;
        }
    }
    return min_idx;
}

static Plugin<ProbabilisticOperatorPolicyPicker> _plugin(
    "operator_id_policy_tiebreaker",
    options::parse<ProbabilisticOperatorPolicyPicker, OperatorIdTiebreaker>);

} // namespace policy_tiebreaking
} // namespace probabilistic