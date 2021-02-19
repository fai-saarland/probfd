#include "multiple_shrinking_strategies.h"
#include "../option_parser.h"
#include "../plugin.h"

#include <sstream>

namespace merge_and_shrink {

MultipleShrinkingStrategy::MultipleShrinkingStrategy(
            const options::Options& opts)
    : ShrinkStrategy(opts)
    , strategies_(opts.get_list<std::shared_ptr<ShrinkStrategy>>("strategies"))
{
}

std::string
MultipleShrinkingStrategy::name() const
{
    std::ostringstream out;
    out << "[";
    for (unsigned i = 0; i < strategies_.size(); ++i) {
        out << (i > 0 ? ", " : "") << strategies_[i]->name();
    }
    out << "]";
    return out.str();
}

bool
MultipleShrinkingStrategy::reduce_labels_before_shrinking() const
{
    bool res = true;
    for (unsigned i = 0; i < strategies_.size(); ++i) {
        res = res && strategies_[i]->reduce_labels_before_shrinking();
    }
    return res;
}

void
MultipleShrinkingStrategy::shrink(Abstraction& abs, int threshold, bool force)
{
    for (unsigned i = 0; i < strategies_.size(); ++i) {
        strategies_[i]->shrink(abs, threshold, force);
    }
}

void
MultipleShrinkingStrategy::shrink_atomic(Abstraction& abs1)
{
    for (unsigned i = 0; i < strategies_.size(); ++i) {
        strategies_[i]->shrink_atomic(abs1);
    }
}

void
MultipleShrinkingStrategy::shrink_before_merge(
    Abstraction& abs1,
    Abstraction& abs2)
{
    for (unsigned i = 0; i < strategies_.size(); ++i) {
        strategies_[i]->shrink_before_merge(abs1, abs2);
    }
}

bool
MultipleShrinkingStrategy::is_goal_leading() const
{
    bool res = true;
    for (unsigned i = 0; i < strategies_.size(); ++i) {
        res = res && strategies_[i]->is_goal_leading();
    }
    return res;
}

bool
MultipleShrinkingStrategy::is_backward_pruned() const
{
    bool res = true;
    for (unsigned i = 0; i < strategies_.size(); ++i) {
        res = res && strategies_[i]->is_backward_pruned();
    }
    return res;
}

bool
MultipleShrinkingStrategy::require_distances() const
{
    bool res = true;
    for (unsigned i = 0; i < strategies_.size(); ++i) {
        res = res && strategies_[i]->require_distances();
    }
    return res;
}

const std::vector<bool>&
MultipleShrinkingStrategy::get_caught_labels_set() const
{
    return strategies_.back()->get_caught_labels_set();
}


static std::shared_ptr<ShrinkStrategy> _parse(options::OptionParser &parser)
{
    parser.add_list_option<std::shared_ptr<ShrinkStrategy>>("strategies");
    ShrinkStrategy::add_options_to_parser(parser);
    options::Options opts = parser.parse();
    if (!parser.dry_run()) {
        ShrinkStrategy::handle_option_defaults(opts);
        return std::make_shared<MultipleShrinkingStrategy>(opts);
    } else {
        return 0;
    }
}

static Plugin<ShrinkStrategy> _plugin("shrink_multiple", _parse);

} // namespace merge_and_shrink
