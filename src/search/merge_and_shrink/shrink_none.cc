#include "shrink_none.h"

#include "abstraction.h"

#include "../option_parser.h"
#include "../plugin.h"

#include <cassert>
#include <iostream>
#include <limits>
#include <memory>

namespace merge_and_shrink {

ShrinkNone::ShrinkNone(const options::Options &opts) : ShrinkStrategy(opts),
    //Michael: For goal leading
    goal_leading(GoalLeading(opts.get_enum("goal_leading")))
{
}

ShrinkNone::~ShrinkNone()
{
}

string ShrinkNone::name() const
{
    return "no_shrink";
}

void ShrinkNone::dump_strategy_specific_options() const
{
    cout << "Dumping " ;
    if (!is_goal_leading()) {
        cout << "no";
    } else if (is_backward_pruned()) {
        cout << "backward pruned";
    } else {
        cout << "all";
    }

    cout << " goal leading actions" << endl;

}

bool ShrinkNone::reduce_labels_before_shrinking() const
{
    return false;
}

void ShrinkNone::shrink(Abstraction &, int, bool)
{
    return;
}

void ShrinkNone::shrink_atomic(Abstraction &)
{
    return;
}

void ShrinkNone::shrink_before_merge(
    Abstraction &, Abstraction &)
{
    return;
}


static std::shared_ptr<ShrinkStrategy> _parse(options::OptionParser &parser)
{
    ShrinkStrategy::add_options_to_parser(parser);

    //Michael: For goal leading
    vector<string> goal_lead;
    goal_lead.push_back("false");
    goal_lead.push_back("all");
    goal_lead.push_back("backward_pruning");
    parser.add_enum_option(
        "goal_leading", goal_lead,
        "catch all, all goal leading or all goal leading with backward pruning actions",
        "false");

    options::Options opts = parser.parse();

    if (!parser.dry_run()) {
        ShrinkStrategy::handle_option_defaults(opts);
        return std::make_shared<ShrinkNone>(opts);
    } else {
        return 0;
    }
}

static Plugin<ShrinkStrategy> _plugin("shrink_none", _parse);

}
