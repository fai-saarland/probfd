#include "quotient_system.h"

#include "../state_registry.h"
#include "../utils/range_proxy.h"

#define DEBUG(x)

namespace probabilistic {
namespace quotient_system {

unsigned QuotientSystem<const ProbabilisticOperator*>::quotient_size(
    const StateID& state_id) const
{
    if (cache_) {
        const QuotientInformation* info = get_infos(state_id);
        assert(info || info->states[0] == state_id);
        return info ? static_cast<int>(info->states.size()) : 1;
    }

    return fallback_->quotient_size(state_id);
}

QuotientSystem<const ProbabilisticOperator*>::const_iterator
QuotientSystem<const ProbabilisticOperator*>::begin() const
{
    if (cache_) {
        using Default = DefaultQuotientSystem<const ProbabilisticOperator*>;
        return const_iterator(this, Default::const_iterator(nullptr, 0));
    }

    return const_iterator(this, fallback_->begin());
}

QuotientSystem<const ProbabilisticOperator*>::const_iterator
QuotientSystem<const ProbabilisticOperator*>::end() const
{
    if (cache_) {
        return const_iterator(
            this,
            DefaultQuotientSystem<const ProbabilisticOperator*>::const_iterator(
                nullptr,
                state_infos_.size()));
    }

    return const_iterator(this, fallback_->end());
}

utils::RangeProxy<
    QuotientSystem<const ProbabilisticOperator*>::QuotientStateIDIterator>
QuotientSystem<const ProbabilisticOperator*>::quotient_range(
    const StateID& state_id) const
{
    if (cache_) {
        const StateID* start = nullptr;
        const StateID* end = nullptr;
        const QuotientInformation* info = get_infos(state_id);

        if (info) {
            start = info->states.data();
            end = start + info->states.size();
        } else {
            start = &state_id;
            end = start + 1;
        }

        return utils::RangeProxy<QuotientStateIDIterator>(
            QuotientStateIDIterator(start),
            QuotientStateIDIterator(end));
    }

    return this->fallback_->quotient_range(state_id);
}

StateID QuotientSystem<const ProbabilisticOperator*>::translate_state_id(
    const StateID& sid) const
{
    if (cache_) {
        const QuotientInformation* i = get_infos(sid);
        return i ? i->states[0] : sid;
    }

    return this->fallback_->translate_state_id(sid);
}

void QuotientSystem<const ProbabilisticOperator*>::generate_applicable_ops(
    const StateID& sid,
    std::vector<QAction>& result)
{
    if (cache_) {
        const auto& qstates = get_infos(sid)->states;
        assert(qstates[0] == sid);
        for (unsigned i = 0; i < qstates.size(); ++i) {
            auto& cached = lookup(qstates[i]);
            for (unsigned j = 0; j < cached.naops; ++j) {
                result.emplace_back(qstates[i], ActionID(j));
            }
        }
        ++(gen_->statistics_.aops_generator_calls);
        gen_->statistics_.generated_operators += result.size();
    } else {
        this->fallback_->generate_applicable_ops(sid, result);
    }
}

void QuotientSystem<const ProbabilisticOperator*>::generate_successors(
    const StateID& sid,
    const QAction& a,
    Distribution<StateID>& result)
{
    if (cache_) {
        assert(state_infos_[sid].states[0] == sid);
        auto& cached = lookup(a.state_id);
        assert(a.action_id < cached.naops);
        const uint32_t* succ = cached.succs;
        const uint32_t* aop = cached.aops;

        auto aop_end = aop + a.action_id;
        for (; aop != aop_end; ++aop) {
            succ += gen_->first_op_[*aop].num_outcomes();
        }

        const ProbabilisticOperator& op = gen_->first_op_[*aop];
        for (auto out = op.begin(); out != op.end(); ++out, ++succ) {
            result.add(*succ, out->prob);
            assert(state_infos_[*succ].states[0] == *succ);
        }

        ++gen_->statistics_.single_transition_generator_calls;
        gen_->statistics_.generated_states += result.size();
    } else {
        fallback_->generate_successors(sid, a, result);
    }
}

void QuotientSystem<const ProbabilisticOperator*>::generate_all_successors(
    const StateID& sid,
    std::vector<QAction>& aops,
    std::vector<Distribution<StateID>>& successors)
{
    if (cache_) {
        const auto& qstates = this->get_infos(sid)->states;
        assert(qstates[0] == sid);
        for (unsigned i = 0; i < qstates.size(); ++i) {
            auto& cached = lookup(qstates[i]);
            const uint32_t* aop = cached.aops;
            const uint32_t* succ = cached.succs;
            for (unsigned k = 0; k < cached.naops; ++k, ++aop) {
                aops.emplace_back(qstates[i], k);
                successors.emplace_back();
                Distribution<StateID>& succs = successors.back();
                const ProbabilisticOperator* op = (gen_->first_op_ + *aop);
                for (unsigned j = 0; j < op->num_outcomes(); ++j, ++succ) {
                    succs.add(*succ, op->get(j).prob);
                    assert(state_infos_[*succ].states[0] == *succ);
                }
                gen_->statistics_.generated_states += succs.size();
            }
        }
        ++(gen_->statistics_.all_transitions_generator_calls);
        gen_->statistics_.generated_operators += aops.size();
    }

    fallback_->generate_all_successors(sid, aops, successors);
}

QuotientSystem<const ProbabilisticOperator*>::QAction
QuotientSystem<const ProbabilisticOperator*>::get_action(
    const StateID& sid,
    const ActionID& aid) const
{
    if (cache_) {
        const auto& qstates = get_infos(sid)->states;
        assert(qstates[0] == sid);
        unsigned offset = aid;
        for (unsigned i = 0; i < qstates.size(); ++i) {
            const auto& cached = lookup(qstates[i]);
            if (offset < cached.naops) {
                return QAction(qstates[i], offset);
            }
            offset -= cached.naops;
        }

        abort();
    }

    return fallback_->get_action(sid, aid);
}

ActionID QuotientSystem<const ProbabilisticOperator*>::get_action_id(
    const StateID& sid,
    const QAction& a) const
{
    if (cache_) {
        const auto& qstates = get_infos(sid)->states;
        assert(qstates[0] == sid);
        unsigned offset = 0;
        for (unsigned i = 0; i < qstates.size(); ++i) {
            if (a.state_id == qstates[i]) {
                offset += a.action_id;
                break;
            }
            const auto& cached = lookup(qstates[i]);
            offset += cached.naops;
        }
        return offset;
    }

    return fallback_->get_action_id(sid, a);
}

const ProbabilisticOperator*
QuotientSystem<const ProbabilisticOperator*>::get_original_action(
    const StateID& sid,
    const QAction& a) const
{
    if (cache_) {
        assert(
            state_infos_[sid].states[0] == sid &&
            std::count(
                state_infos_[sid].states.begin(),
                state_infos_[sid].states.end(),
                sid));
        const auto& entry = lookup(a.state_id);
        assert(a.action_id < entry.naops);
        return (gen_->first_op_ + entry.aops[a.action_id]);
    }

    return fallback_->get_original_action(sid, a);
}

ActionID QuotientSystem<const ProbabilisticOperator*>::get_original_action_id(
    const StateID& sid,
    const ActionID& a) const
{
    if (cache_) {
        const auto& qstates = get_infos(sid)->states;
        unsigned offset = a;
        for (const StateID& qstate : qstates) {
            const auto& cached = lookup(qstate);
            if (offset < cached.naops) {
                return cached.aops[offset];
            }
            offset -= cached.naops;
        }

        abort();
    }

    return fallback_->get_original_action_id(sid, a);
}

const QuotientSystem<const ProbabilisticOperator*>::QuotientInformation*
QuotientSystem<const ProbabilisticOperator*>::get_infos(
    const StateID& sid) const
{
    assert(sid < state_infos_.size());
    return &state_infos_[sid];
}

const TransitionGenerator<const ProbabilisticOperator*>::CacheEntry&
QuotientSystem<const ProbabilisticOperator*>::lookup(const StateID& sid) const
{
    const auto& entry = gen_->cache_[sid];
    assert(entry.is_initialized());
    return entry;
}

TransitionGenerator<const ProbabilisticOperator*>::CacheEntry&
QuotientSystem<const ProbabilisticOperator*>::lookup(const StateID& sid)
{
    bool cache_setup = false;
    auto& entry = gen_->lookup(sid, cache_setup);

    if (cache_setup) {
        DEBUG(std::cout << "cache for " << sid
                        << " has been initialized -> applying abstraction ..."
                        << std::endl;)
        for (unsigned i = state_infos_.size();
             i < gen_->state_registry_->size();
             ++i) {
            state_infos_.push_back(QuotientInformation(StateID(i)));
        }

        const uint32_t* aop = entry.aops;
        const uint32_t* aop_end = entry.aops + entry.naops;
        uint32_t* succ = entry.succs;
        std::unordered_set<StateID> unique_successors;
        DEBUG(std::cout << "   ";);
        for (; aop != aop_end; ++aop) {
            auto succ_end = succ + gen_->first_op_[*aop].num_outcomes();
            for (; succ != succ_end; ++succ) {
                const StateID x = state_infos_[*succ].states[0];
                DEBUG(std::cout << " " << *succ << ":=" << x);
                *succ = x;
                DEBUG(std::cout << "(" << *succ << ")");
                // QuotientInformation& succ_info = state_infos_[*succ];
                // if (succ_info.parents.empty() || succ_info.parents.back() !=
                // sid) {
                //     succ_info.parents.push_back(sid);
                // }
                unique_successors.insert(x);
            }
        }
        DEBUG(std::cout << std::endl;)
#if 1
        for (const StateID& succ : unique_successors) {
            if (succ != sid) {
                assert(!utils::contains(state_infos_[succ].parents, sid));
                state_infos_[succ].parents.push_back(sid);
            }
        }
#endif
    }
#ifndef NDEBUG
    {
        std::cout << "looked up " << sid << std::endl;
        verify_cache_consistency();
    }
#endif
    return entry;
}

#ifndef NDEBUG
void QuotientSystem<const ProbabilisticOperator*>::verify_cache_consistency()
{
    DEBUG(std::cout << "  current cache size: " << gen_->cache_.size()
                    << std::endl;)
    for (unsigned i = 0; i < gen_->cache_.size(); ++i) {
        const auto& entry = gen_->cache_[i];
        if (entry.is_initialized()) {
            DEBUG(std::cout << "  checking cache[" << i << "] ..."
                            << std::flush;)
            assert(i < state_infos_.size() && !state_infos_[i].states.empty());
            DEBUG(std::cout << " -> represented by "
                            << state_infos_[i].states[0]
                            << "; naops=" << entry.naops << ": " << std::flush;)
            const uint32_t* op = entry.aops;
            const uint32_t* succ = entry.succs;
            for (int j = entry.naops - 1; j >= 0; --j, ++op) {
                for (int k = (gen_->first_op_ + *op)->num_outcomes() - 1;
                     k >= 0;
                     --k, ++succ) {
                    assert(*succ < state_infos_.size());
                    const QuotientInformation& info = state_infos_[*succ];
                    DEBUG(std::cout << " {succ:" << (*succ) << "->"
                                    << info.states[0] << "}" << std::flush;)
                    assert(!info.states.empty() && info.states[0] == *succ);
                    // assert(
                    //     *succ == i
                    //     || std::count(
                    //         info.parents.begin(), info.parents.end(), i));
                    // assert(
                    //     *succ == i
                    //     || (std::count(
                    //             info.parents.begin(), info.parents.end(), i)
                    //         == 1));
                }
            }
            DEBUG(std::cout << std::endl;)
        }
    }
}
#endif

} // namespace quotient_system
} // namespace probabilistic

#undef DEBUG
