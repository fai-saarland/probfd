#include "probfd/quotient_system.h"

#include "state_registry.h"

namespace probfd {
namespace quotients {

using namespace engine_interfaces;

QuotientSystem<OperatorID>::const_iterator::const_iterator(
    const QuotientSystem<OperatorID>* qs,
    DefaultQuotientSystem<OperatorID>::const_iterator x)
    : qs_(qs)
    , i(x)
{
}

QuotientSystem<OperatorID>::const_iterator&
QuotientSystem<OperatorID>::const_iterator::operator++()
{
    if (qs_->cache_) {
        while (++i.i.id < qs_->state_infos_.size()) {
            const StateID::size_type ref = qs_->state_infos_[i.i].states[0];
            if (ref == i.i) {
                break;
            }
        }
    } else {
        ++i;
    }
    return *this;
}

QuotientSystem<OperatorID>::const_iterator
QuotientSystem<OperatorID>::const_iterator::operator++(int)
{
    auto r = *this;
    ++(*this);
    return r;
}

bool operator==(
    const QuotientSystem<OperatorID>::const_iterator& left,
    const QuotientSystem<OperatorID>::const_iterator& right)
{
    return left.i == right.i;
}

StateID QuotientSystem<OperatorID>::const_iterator::operator*() const
{
    return i.i;
}

QuotientSystem<OperatorID>::QuotientSystem(
    engine_interfaces::ActionIDMap<OperatorID>* aid,
    engine_interfaces::TransitionGenerator<OperatorID>* transition_gen)
    : cache_(transition_gen->caching_)
    , gen_(transition_gen)
    , fallback_(nullptr)
{
    if (!cache_) {
        fallback_.reset(
            new DefaultQuotientSystem<OperatorID>(aid, transition_gen));
    } else {
        state_infos_.push_back(QuotientInformation(0));
    }
}

unsigned QuotientSystem<OperatorID>::quotient_size(StateID state_id) const
{
    if (cache_) {
        const QuotientInformation* info = get_infos(state_id);
        assert(info || info->states[0] == state_id);
        return info ? static_cast<int>(info->states.size()) : 1;
    }

    return fallback_->quotient_size(state_id);
}

QuotientSystem<OperatorID>::const_iterator
QuotientSystem<OperatorID>::begin() const
{
    if (cache_) {
        using Default = DefaultQuotientSystem<OperatorID>;
        return const_iterator(this, Default::const_iterator(nullptr, 0));
    }

    return const_iterator(this, fallback_->begin());
}

QuotientSystem<OperatorID>::const_iterator
QuotientSystem<OperatorID>::end() const
{
    if (cache_) {
        return const_iterator(
            this,
            DefaultQuotientSystem<OperatorID>::const_iterator(
                nullptr,
                state_infos_.size()));
    }

    return const_iterator(this, fallback_->end());
}

std::ranges::subrange<QuotientSystem<OperatorID>::QuotientStateIDIterator>
QuotientSystem<OperatorID>::quotient_range(StateID state_id) const
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

        return std::ranges::subrange<QuotientStateIDIterator>(
            QuotientStateIDIterator(start),
            QuotientStateIDIterator(end));
    }

    return this->fallback_->quotient_range(state_id);
}

StateID QuotientSystem<OperatorID>::translate_state_id(StateID sid) const
{
    if (cache_) {
        const QuotientInformation* i = get_infos(sid);
        return i ? i->states[0] : sid;
    }

    return this->fallback_->translate_state_id(sid);
}

void QuotientSystem<OperatorID>::generate_applicable_ops(
    StateID sid,
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

        ++gen_->statistics_.aops_generator_calls;
        gen_->statistics_.generated_operators += result.size();
    } else {
        this->fallback_->generate_applicable_ops(sid, result);
    }
}

void QuotientSystem<OperatorID>::generate_successors(
    StateID sid,
    const QAction& a,
    Distribution<StateID>& result)
{
    if (cache_) {
        assert(state_infos_[sid].states[0] == sid);
        auto& cached = lookup(a.state_id);
        assert(a.action_id < cached.naops);
        const StateID* succ = cached.succs;
        const OperatorID* aop = cached.aops;

        const auto operators = gen_->task_proxy.get_operators();

        auto aop_end = aop + a.action_id;
        for (; aop != aop_end; ++aop) {
            succ += operators[*aop].get_outcomes().size();
        }

        for (const auto outcome : operators[*aop].get_outcomes()) {
            result.add(*succ, outcome.get_probability());
            assert(state_infos_[*succ].states[0] == *succ);
            ++succ;
        }

        ++gen_->statistics_.single_transition_generator_calls;
        gen_->statistics_.generated_states += result.size();
    } else {
        fallback_->generate_successors(sid, a, result);
    }
}

void QuotientSystem<OperatorID>::generate_all_successors(
    StateID sid,
    std::vector<QAction>& aops,
    std::vector<Distribution<StateID>>& successors)
{
    if (cache_) {
        const auto& qstates = this->get_infos(sid)->states;
        assert(qstates[0] == sid);

        const auto operators = gen_->task_proxy.get_operators();

        for (unsigned i = 0; i < qstates.size(); ++i) {
            auto& cached = lookup(qstates[i]);
            const OperatorID* aop = cached.aops;
            const StateID* succ = cached.succs;
            for (unsigned k = 0; k < cached.naops; ++k, ++aop) {
                aops.emplace_back(qstates[i], k);
                successors.emplace_back();
                Distribution<StateID>& succs = successors.back();
                for (const auto outcome : operators[*aop].get_outcomes()) {
                    succs.add(*succ, outcome.get_probability());
                    assert(state_infos_[*succ].states[0] == *succ);
                    ++succ;
                }

                gen_->statistics_.generated_states += succs.size();
            }
        }

        ++gen_->statistics_.all_transitions_generator_calls;
        gen_->statistics_.generated_operators += aops.size();
        return;
    }

    fallback_->generate_all_successors(sid, aops, successors);
}

QuotientSystem<OperatorID>::QAction
QuotientSystem<OperatorID>::get_action(StateID sid, ActionID aid) const
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

ActionID
QuotientSystem<OperatorID>::get_action_id(StateID sid, const QAction& a) const
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

OperatorID
QuotientSystem<OperatorID>::get_original_action(StateID sid, const QAction& a)
    const
{
    if (cache_) {
        assert(
            state_infos_[sid].states[0] == sid &&
            utils::contains(state_infos_[sid].states, sid));
        const auto& entry = lookup(a.state_id);
        assert(a.action_id < entry.naops);
        return entry.aops[a.action_id];
    }

    return fallback_->get_original_action(sid, a);
}

ActionID
QuotientSystem<OperatorID>::get_original_action_id(StateID sid, ActionID a)
    const
{
    if (cache_) {
        const auto& qstates = get_infos(sid)->states;
        unsigned offset = a;
        for (StateID qstate : qstates) {
            const auto& cached = lookup(qstate);
            if (offset < cached.naops) {
                return cached.aops[offset].get_index();
            }
            offset -= cached.naops;
        }

        abort();
    }

    return fallback_->get_original_action_id(sid, a);
}

const QuotientSystem<OperatorID>::QuotientInformation*
QuotientSystem<OperatorID>::get_infos(StateID sid) const
{
    assert(sid < state_infos_.size());
    return &state_infos_[sid];
}

const TransitionGenerator<OperatorID>::CacheEntry&
QuotientSystem<OperatorID>::lookup(StateID sid) const
{
    const auto& entry = gen_->cache_[sid];
    assert(entry.is_initialized());
    return entry;
}

TransitionGenerator<OperatorID>::CacheEntry&
QuotientSystem<OperatorID>::lookup(StateID sid)
{
    bool cache_setup = false;
    auto& entry = gen_->lookup(sid, cache_setup);

    if (!cache_setup) {
#ifndef NDEBUG
        // std::cout << "looked up " << sid << std::endl;
        verify_cache_consistency();
#endif

        return entry;
    }

    for (auto i = state_infos_.size(); i < gen_->state_registry_->size(); ++i) {
        state_infos_.push_back(QuotientInformation(StateID(i)));
    }

    std::unordered_set<StateID> unique_successors;
    const auto operators = gen_->task_proxy.get_operators();

    {
        const OperatorID* aop = entry.aops;
        const OperatorID* aop_end = entry.aops + entry.naops;
        StateID* succ = entry.succs;
        for (; aop != aop_end; ++aop) {
            auto succ_end = succ + operators[*aop].get_outcomes().size();
            for (; succ != succ_end; ++succ) {
                const StateID x = state_infos_[*succ].states[0];
                *succ = x;
                unique_successors.insert(x);
            }
        }
    }

    for (const StateID succ : unique_successors) {
        if (succ != sid) {
            assert(!utils::contains(state_infos_[succ].parents, sid));
            state_infos_[succ].parents.push_back(sid);
        }
    }

#ifndef NDEBUG
    // std::cout << "looked up " << sid << std::endl;
    verify_cache_consistency();
#endif

    return entry;
}

void QuotientSystem<OperatorID>::update_cache(
    const std::vector<OperatorID>& exclude,
    engine_interfaces::TransitionGenerator<OperatorID>::CacheEntry& entry,
    const StateID rid,
    const std::unordered_set<StateID>& quotient_states)
{
    unsigned new_size = 0;
    OperatorID* aops_src = entry.aops;
    OperatorID* aops_dest = entry.aops;
    StateID* succ_src = entry.succs;
    StateID* succ_dest = entry.succs;

    auto aops_src_end = aops_src + entry.naops;
    for (; aops_src != aops_src_end; ++aops_src) {
        OperatorID op_id = *aops_src;
        if (utils::contains(exclude, op_id)) {
            continue;
        }

        bool self_loop = true;
        StateID* k = succ_dest;

        const ProbabilisticOperatorProxy op =
            gen_->task_proxy.get_operators()[op_id];

        auto succ_src_end = succ_src + op.get_outcomes().size();
        for (; succ_src != succ_src_end; ++succ_src, ++succ_dest) {
            const bool member = quotient_states.contains(*succ_src);
            *succ_dest = member ? rid : *succ_src;
            self_loop = self_loop && (*succ_dest == rid);
            succ_dest = k;
            *aops_dest = *aops_src;
            ++aops_dest;
            ++new_size;
        }
    }

    entry.naops = new_size;
}

#ifndef NDEBUG
void QuotientSystem<OperatorID>::verify_cache_consistency()
{
    const auto operators = gen_->task_proxy.get_operators();

    for (unsigned i = 0; i < gen_->cache_.size(); ++i) {
        const auto& entry = gen_->cache_[i];

        if (!entry.is_initialized()) {
            continue;
        }

        assert(i < state_infos_.size() && !state_infos_[i].states.empty());

        const OperatorID* opid = entry.aops;
        const StateID* succ = entry.succs;
        for (int j = entry.naops - 1; j >= 0; --j, ++opid) {
            const auto aop = operators[*opid];
            for (int k = aop.get_outcomes().size() - 1; k >= 0; --k, ++succ) {
                assert(*succ < state_infos_.size());
                const QuotientInformation& info = state_infos_[*succ];
                assert(!info.states.empty() && info.states[0] == *succ);
            }
        }
    }
}
#endif

} // namespace quotients
} // namespace probfd
