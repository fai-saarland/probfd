#include "shrink_budget_based.h"

#include "../option_parser.h"
#include "../plugin.h"
#include "abstraction.h"

using namespace std;

namespace merge_and_shrink {

BudgetShrink::BudgetShrink(const Options& opts)
    : ShrinkStrategy(opts)
    , budget_(opts.get<int>("budget"))
{
}

void
BudgetShrink::prune(Abstraction& a) const
{
    EquivalenceRelation equiv;
    equiv.reserve(a.size());
    for (int state = 0; state < a.size(); state++) {
        if (a.get_init_distance(state) + a.get_goal_distance(state)
            <= budget_) {
            EquivalenceClass c;
            c.push_front(state);
            equiv.push_back(c);
        }
    }
    if (equiv.empty()) {
        EquivalenceClass c;
        c.push_front(a.get_initial_state());
        equiv.push_back(c);
    }
    if (equiv.size() < (unsigned)a.size()) {
        apply(a, equiv, equiv.size());
    }
}

std::string
BudgetShrink::name() const
{
    return "BudgetShrink";
}

static shared_ptr<ShrinkStrategy>
_parse(options::OptionParser& parser)
{
    ShrinkStrategy::add_options_to_parser(parser);
    Options opts = parser.parse();
    if (!parser.dry_run()) {
        return shared_ptr<ShrinkStrategy>(new BudgetShrink(opts));
    }
    return nullptr;
}

static Plugin<ShrinkStrategy> _plugin("budget", _parse);

} // namespace merge_and_shrink

