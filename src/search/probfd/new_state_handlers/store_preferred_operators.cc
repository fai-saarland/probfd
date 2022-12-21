#include "probfd/new_state_handlers/store_preferred_operators.h"

#include "probfd/globals.h"
#include "probfd/probabilistic_operator.h"

#include "legacy/global_operator.h"
#include "legacy/global_state.h"
#include "legacy/globals.h"
#include "legacy/heuristic.h"

#include "option_parser.h"
#include "plugin.h"

#include <algorithm>
#include <cassert>

namespace probfd {
namespace new_state_handlers {

bool PrefOpsCacheEntry::contains(const ProbabilisticOperator* ptr) const
{
    const ProbabilisticOperator* fp = &g_operators[0];
    const unsigned id = ptr - fp;
    return contains(id);
}

bool PrefOpsCacheEntry::contains(unsigned i) const
{
    if (ops_ == nullptr) {
        return false;
    }
    const unsigned* start = ops_ + 1;
    const unsigned* end = ops_ + ops_[0];
    const unsigned* ptr = std::lower_bound(start, end, i);
    return ptr != end && (*ptr == i);
}

StorePreferredOperators::StorePreferredOperators(const options::Options& opts)
    : fetch_only_(opts.get<bool>("fetch_only"))
    , heuristic_(opts.get<std::shared_ptr<legacy::Heuristic>>("heuristic"))
{
    refs_.resize(legacy::g_operators.size(), -1);
    for (int i = g_operators.size() - 1; i >= 0; --i) {
        const ProbabilisticOperator& op = g_operators[i];
        for (int j = op.num_outcomes() - 1; j >= 0; --j) {
            const legacy::GlobalOperator* out = op[j].op;
            if (out->get_id() < legacy::g_operators.size()) {
                refs_[out->get_id()] = i;
            }
        }
    }
#if !defined(NDEBUG)
    for (int i = refs_.size() - 1; i >= 0; --i) {
        assert(refs_[i] < g_operators.size());
    }
#endif
}

void StorePreferredOperators::add_options_to_parser(
    options::OptionParser& parser)
{
    parser.add_option<std::shared_ptr<legacy::Heuristic>>("heuristic");
    parser.add_option<bool>("fetch_only", "", "false");
}

const PrefOpsCacheEntry&
StorePreferredOperators::get_cached_ops(const StateID& s)
{
    return store_[s];
}

void StorePreferredOperators::touch_goal(const legacy::GlobalState&)
{
}

void StorePreferredOperators::touch_dead_end(const legacy::GlobalState&)
{
}

void StorePreferredOperators::touch(const legacy::GlobalState& s)
{
    if (!fetch_only_) {
        heuristic_->evaluate(s);
    }

    if (heuristic_->is_dead_end()) {
        return;
    }

    heuristic_->get_preferred_operators(pref_);
    if (pref_.empty()) {
        return;
    }

    for (int i = pref_.size() - 1; i >= 0; --i) {
        const unsigned opidx = pref_[i]->get_id();
        assert(opidx < refs_.size());
        ids_.push_back(refs_[opidx]);
    }
    pref_.clear();

    std::sort(ids_.begin(), ids_.end());
    ids_.erase(std::unique(ids_.begin(), ids_.end()), ids_.end());

    PrefOpsCacheEntry& entry = store_[s.get_id().hash()];
    entry.ops_ = new unsigned[ids_.size() + 1];
    entry.ops_[0] = ids_.size();
    unsigned* id = entry.ops_ + 1;
    const unsigned* idd = &ids_[0];
    for (int i = ids_.size(); i > 0; --i, ++id, ++idd) {
        *id = *idd;
    }

    ids_.clear();
}

static Plugin<NewGlobalStateHandler> _plugin(
    "store_preferred_operators",
    options::parse<NewGlobalStateHandler, StorePreferredOperators>);
} // namespace new_state_handlers
} // namespace probfd
