#include "transition_generator.h"

#include "../algorithms/int_packer.h"
#include "../global_operator.h"
#include "../global_state.h"
#include "../heuristic.h"
#include "../state_registry.h"
#include "probabilistic_operator.h"

#include <algorithm>
#include <cassert>

#define TGEN_DEBUG_MSG(x) // x

namespace probabilistic {
namespace algorithms {

void
TransitionGenerator<GlobalState, const ProbabilisticOperator*>::Statistics::
    print(std::ostream& out) const
{
    out << "Transition generator calls: " << calls << std::endl;
#if defined(EXPENSIVE_STATISTICS)
    out << "Transition generation time: " << time << std::endl;
#endif
    out << "Transitions generated: " << transitions << std::endl;
}

TransitionGenerator<GlobalState, const ProbabilisticOperator*>::
    TransitionGenerator(
        StateRegistry* state_registry,
        const std::vector<ProbabilisticOperator>& ops,
        const std::vector<std::shared_ptr<Heuristic>>&
            path_dependent_heuristics,
        bool cache_transitions)
    : notify_(path_dependent_heuristics)
    , cache_transitions_(cache_transitions)
    , is_fh_(false)
    , fh_var_(-1)
    , cost_type_()
    , state_registry_(state_registry)
    , op_ptr_(ops.empty() ? nullptr : &ops[0])
    , last_entry_(nullptr)
    , last_access_(StateID::no_state)
{
}

TransitionGenerator<GlobalState, const ProbabilisticOperator*>::
    TransitionGenerator(
        StateRegistry* state_registry,
        const std::vector<ProbabilisticOperator>& ops,
        const std::vector<std::shared_ptr<Heuristic>>&
            path_dependent_heuristics,
        OperatorCost cost_type,
        int fh_var,
        bool cache_transitions)
    : notify_(path_dependent_heuristics)
    , cache_transitions_(cache_transitions)
    , is_fh_(fh_var != -1)
    , fh_var_(fh_var)
    , cost_type_(cost_type)
    , state_registry_(state_registry)
    , op_ptr_(ops.empty() ? nullptr : &ops[0])
    , last_entry_(nullptr)
    , last_access_(StateID::no_state)
{
}

Distribution<GlobalState>
TransitionGenerator<GlobalState, const ProbabilisticOperator*>::operator()(
    const GlobalState& state,
    const ProbabilisticOperator* op)
{
    statistics_.calls++;
#if defined(EXPENSIVE_STATISTICS)
    statistics_.time.resume();
#endif

    assert(is_applicable(op, state));

    if (cache_transitions_) {
        if (state.get_id() != last_access_) {
            last_access_ = state.get_id();
            last_entry_ = &cache_[state.get_id().hash()];
        }
        assert(last_entry_->naops > 0);
        uint32_t* aop = std::lower_bound(
            last_entry_->aops + 1,
            last_entry_->aops + last_entry_->naops + 1,
            op->get_id() << CacheEntry::SPLIT);
        assert((*aop >> CacheEntry::SPLIT) == op->get_id());
        const uint32_t i = (*aop) & CacheEntry::MASK;
        uint32_t j = std::min(
            *last_entry_->aops,
            aop == (last_entry_->aops + last_entry_->naops)
                ? CacheEntry::MASK
                : *(aop + 1) & CacheEntry::MASK);
        TGEN_DEBUG_MSG(std::cout
                           << "cache hit for " << state.get_id() << " and op#"
                           << op->get_id() << " (is_last="
                           << (aop == (last_entry_->aops + last_entry_->naops))
                           << ")"
                           << " -> [" << i << ", " << j << ")" << std::endl;)
        const uint32_t* id = last_entry_->succs + i;
        const value_type::value_t* prob = last_entry_->probs + i;
        Distribution<GlobalState> result;
        for (; j > i; --j, ++id, ++prob) {
            result.add(state_registry_->lookup_state(StateID(*id)), *prob);
        }
        assert(result.is_normalized());
        return result;
    }

    const Distribution<GlobalState> res = compute_transition(state, op);

#if defined(EXPENSIVE_STATISTICS)
    statistics_.time.stop();
#endif

    return res;

#if 0

    Distribution<GlobalState> result;

    if (cache_transitions_) {
        uint32_t* succs = last_entry_->succs;
        value_type::value_t* probs = last_entry_->probs;
        last_entry_->succs = new uint32_t[last_entry_->aops[0] + result.size()];
        last_entry_->probs =
            new value_type::value_t[last_entry_->aops[0] + result.size()];
        if (succs == nullptr) {
            succs = last_entry_->succs;
            probs = last_entry_->probs;
        } else {
            uint32_t *a, *c;
            value_type::value_t *b, *d;
            a = c = succs;
            b = d = probs;
            succs = last_entry_->succs;
            probs = last_entry_->probs;
            for (int i = last_entry_->aops[0]; i > 0;
                 --i, ++succs, ++probs, ++a, ++b) {
                *succs = *a;
                *probs = *b;
            }
            delete[](c);
            delete[](d);
        }
        assert(result.size() + last_entry_->aops[0] < CacheEntry::MASK);
        *aop = (*aop & ~CacheEntry::MASK) | (last_entry_->aops[0]);
        last_entry_->aops[0] = last_entry_->aops[0] + result.size();
        assert(
            (last_entry_->aops[0] - result.size() - (*aop & CacheEntry::MASK))
            == 0);
        std::cout << "cached entry for " << state.get_id() << " and op#"
                  << op->get_id() << " (" << (*aop >> CacheEntry::SPLIT)
                  << " -> [" << (*aop & CacheEntry::MASK) << ", "
                  << last_entry_->aops[0] << ") size = " << result.size()
                  << std::endl;
        auto it = result.begin();
        for (int i = result.size(); i > 0; --i, ++succs, ++probs, ++it) {
            *succs = it->first.get_id().hash();
            *probs = it->second;
        }
    }

    return result;
#endif
}

Distribution<GlobalState>
TransitionGenerator<GlobalState, const ProbabilisticOperator*>::
    compute_transition(
        const GlobalState& state,
        const ProbabilisticOperator* op)
{
    Distribution<GlobalState> result;
    if (is_fh_) {
        const int_packer::IntPacker* packer =
            state_registry_->get_state_packer();
        for (int i = op->num_outcomes() - 1; i >= 0; i--) {
            const auto& out = op->get(i);
            const GlobalOperator& det_op = *(out.op);
            int newb =
                state[fh_var_] - get_adjusted_action_cost(det_op, cost_type_);
            assert(newb >= 0);
            PackedStateBin* succ_data =
                state_registry_->get_temporary_successor_state(state, det_op);
            packer->set(succ_data, fh_var_, newb);
            GlobalState succ = state_registry_->make_permanent();
            for (int j = notify_.size() - 1; j >= 0; j--) {
                notify_[j]->reach_state(state, det_op, succ);
            }
            result.add(succ, out.prob);
        }
    } else {
        for (int i = op->num_outcomes() - 1; i >= 0; i--) {
            const auto& out = op->get(i);
            const GlobalOperator& det_op = *(out.op);
            GlobalState succ =
                state_registry_->get_successor_state(state, det_op);
            for (int j = notify_.size() - 1; j >= 0; j--) {
                notify_[j]->reach_state(state, det_op, succ);
            }
            result.add(succ, out.prob);
        }
    }
    ++statistics_.transitions;
    return result;
}

void
TransitionGenerator<GlobalState, const ProbabilisticOperator*>::
    notify_applicable_actions(
        const GlobalState& s,
        const std::vector<const ProbabilisticOperator*>& aops)
{
    assert(cache_transitions_);
    if (aops.empty()) {
        return;
    }
#if defined(EXPENSIVE_STATISTICS)
    statistics_.time.resume();
#endif
    last_access_ = s.get_id();
    last_entry_ = &cache_[last_access_.hash()];
    if (last_entry_->naops == 0) {
        last_entry_->naops = aops.size();
        last_entry_->aops = new uint32_t[aops.size() + 1];
        last_entry_->aops[0] = 0;
        for (int i = aops.size() - 1; i >= 0; --i) {
            assert(aops[i]->get_id() < CacheEntry::MASK);
            last_entry_->aops[i + 1] = aops[i]->get_id();
        }
        std::sort(last_entry_->aops + 1, last_entry_->aops + 1 + aops.size());

        TGEN_DEBUG_MSG(std::cout << "creating new cache entry for state "
                                 << s.get_id() << " size=" << aops.size()
                                 << std::endl;)

        Distribution<GlobalState> t;
        std::vector<uint32_t> ids;
        std::vector<value_type::value_t> probs;
        uint32_t* opid = last_entry_->aops + 1;
        for (int i = aops.size(); i > 0; --i, ++opid) {
            TGEN_DEBUG_MSG(std::cout << " . " << s.get_id() << " op" << (*opid)
                                     << " -> [" << last_entry_->aops[0];)
            t = compute_transition(s, op_ptr_ + *opid);
            assert((op_ptr_ + *opid)->get_id() == *opid);
            *opid = (*opid << CacheEntry::SPLIT) | last_entry_->aops[0];
            assert(last_entry_->aops[0] + t.size() < CacheEntry::MASK);
            last_entry_->aops[0] += t.size();
            TGEN_DEBUG_MSG(std::cout << ", " << last_entry_->aops[0] << ")"
                                     << std::endl;)
            for (auto it = t.begin(); it != t.end(); ++it) {
                ids.push_back(it->first.get_id().hash());
                probs.push_back(it->second);
            }
        }
        last_entry_->succs = new uint32_t[ids.size()];
        last_entry_->probs = new value_type::value_t[ids.size()];
        uint32_t* a = last_entry_->succs;
        uint32_t* b = &ids[0];
        value_type::value_t* c = last_entry_->probs;
        value_type::value_t* d = &probs[0];
        for (int i = ids.size(); i > 0; --i, ++a, ++b, ++c, ++d) {
            *a = *b;
            *c = *d;
        }
    }
#if defined(EXPENSIVE_STATISTICS)
    statistics_.time.stop();
#endif
}

} // namespace algorithms
} // namespace probabilistic

#undef TGEN_DEBUG_MSG
