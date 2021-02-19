#pragma once

#include "../algorithms/segmented_vector.h"
#include "probabilistic_operator.h"
#include "quotient_system/engine_interfaces.h"
#include "quotient_system/quotient_system.h"
#include "transition_generator.h"

#include <algorithm>
#include <cassert>
#include <memory>
#include <set>
#include <unordered_set>
#include <vector>

namespace probabilistic {
namespace quotient_system {

template<>
class QuotientSystem<const ProbabilisticOperator*> {
    friend class const_iterator;

public:
    using Action = const ProbabilisticOperator*;
    using QAction = QuotientAction<Action>;
    using QuotientStateIDIterator = const StateID*;

    struct const_iterator {
        using iterator_type = std::forward_iterator_tag;
        using value_type = StateID::size_type;
        using difference_type = int;
        using pointer = const StateID::size_type*;
        using reference = const StateID::size_type&;

        explicit const_iterator(
            const QuotientSystem* qs,
            DefaultQuotientSystem<const ProbabilisticOperator*>::const_iterator x)
            : qs_(qs)
            , i(x)
        {
        }

        const_iterator& operator++()
        {
            if (qs_->cache_) {
                while (++i.i < qs_->state_infos_.size()) {
                    const StateID::size_type ref =
                        qs_->state_infos_[i.i].states[0];
                    if (ref == i.i) {
                        break;
                    }
                }
            } else {
                ++i;
            }
            return *this;
        }

        const_iterator operator+=(int x)
        {
            const_iterator res = *this;
            while (x-- > 0)
                this->operator++();
            return res;
        }

        bool operator==(const const_iterator& other) const
        {
            return i == other.i;
        }

        bool operator!=(const const_iterator& other) const
        {
            return i != other.i;
        }

        reference operator*() const { return i.i; }

    private:
        const QuotientSystem* qs_;
        DefaultQuotientSystem<const ProbabilisticOperator*>::const_iterator i;
    };

    explicit QuotientSystem(
        ActionIDMap<Action>*,
        ApplicableActionsGenerator<Action>*,
        TransitionGenerator<Action>* transition_gen);

    unsigned quotient_size(const StateID& state_id) const;

    const_iterator begin() const;
    const_iterator end() const;

    std::pair<QuotientStateIDIterator, QuotientStateIDIterator>
    quotient_iterator(const StateID& state_id) const;

    StateID translate_state_id(const StateID& sid) const;

    void
    generate_applicable_ops(const StateID& sid, std::vector<QAction>& result);

    void generate_successors(
        const StateID&,
        const QAction& a,
        Distribution<StateID>& result);

    void generate_all_successors(
        const StateID& sid,
        std::vector<QAction>& aops,
        std::vector<Distribution<StateID>>& successors);

    QAction get_action(const StateID& sid, const ActionID& aid) const;
    ActionID get_action_id(const StateID& sid, const QAction& a) const;

    Action get_original_action(const StateID&, const QAction& a) const;
    ActionID
    get_original_action_id(const StateID& sid, const ActionID& a) const;

    template<
        typename StateIDIterator,
        typename IgnoreActionsIterator = const void**>
    void build_quotient(
        StateIDIterator begin,
        StateIDIterator end,
        const StateID& rid,
        IgnoreActionsIterator ignore_actions = nullptr)
    {
        if (!cache_) {
            fallback_->build_quotient(begin, end, rid, ignore_actions);
            return;
        }

        // std::cout << "Building new quotient represented by " << rid <<
        // std::endl;

        QuotientInformation& dest_info = state_infos_[rid];
        std::vector<StateID>& parents = dest_info.parents;
        std::vector<StateID>& states = dest_info.states;
        const unsigned old_qstates = states.size();

        std::set<uint32_t> uniqs;
        std::unordered_set<uint32_t> states_set;

        {
#ifndef NDEBUG
            bool rid_is_in_range = false;
#endif
            for (auto it = begin; it != end; ++it) {
                const StateID state_id = *it;

                if (state_id == rid) {
#ifndef NDEBUG
                    rid_is_in_range = true;
#endif
                    continue;
                }

                QuotientInformation& info = state_infos_[state_id];
                assert(info.states[0] == state_id);

                parents.insert(
                    parents.end(), info.parents.begin(), info.parents.end());
                info.parents.clear();
                info.parents.shrink_to_fit();

                states.insert(
                    states.end(), info.states.begin(), info.states.end());

                assert(std::count(states.begin(), states.end(), state_id));
            }
            assert(rid_is_in_range);

            states_set.insert(states.begin(), states.end());

            for (auto it = begin; it != end; ++it, ++ignore_actions) {
                const StateID state_id = *it;
                QuotientInformation& info = state_infos_[state_id];
                int i =
                    (state_id != rid ? info.states.size() : old_qstates) - 1;
                for (; i >= 0; --i) {
                    update_cache(
                        *ignore_actions,
                        gen_->lookup(info.states[i]),
                        rid,
                        states_set);
                    state_infos_[info.states[i]].states[0] = rid;
                }
                if (state_id != rid) {
                    info.states.resize(1);
                    info.states[0] = rid;
                    info.states.shrink_to_fit();
                }
            }
        }

#if 0
        {
            std::set<uint32_t> diff;
            std::set_difference(
                uniqs.begin(),
                uniqs.end(),
                states_set.begin(),
                states_set.end(),
                std::inserter(diff, diff.end()));
            diff.swap(uniqs);
        }

        for (auto it = uniqs.begin(); it != uniqs.end(); ++it) {
            QuotientInformation& info = state_infos_[*it];
            assert(info.states[0] == *it);
            assert(*it != rid);
            unsigned i = 0;
            bool in = false;
            for (unsigned j = 0; j < info.parents.size(); ++j) {
                if (!states_set.count(info.parents[j])) {
                    info.parents[i] = info.parents[j];
                    ++i;
                } else if (!in) {
                    info.parents[i] = rid;
                    ++i;
                    in = true;
                }
            }
            info.parents.resize(i);
            info.parents.shrink_to_fit();
#ifndef NDEBUG
            std::vector<StateID> p(info.parents);
            std::sort(p.begin(), p.end());
            assert(std::unique(p.begin(), p.end()) == p.end());
#endif
        }
#endif

        {
            unsigned i = 0;
            for (unsigned j = 0; j < parents.size(); ++j) {
                const StateID new_id = state_infos_[parents[j]].states[0];
                if (new_id != rid) {
                    parents[i++] = new_id;
                }
            }
            parents.resize(i);
            std::sort(parents.begin(), parents.end());
            parents.erase(
                std::unique(parents.begin(), parents.end()), parents.end());
            parents.shrink_to_fit();
        }
        for (int i = parents.size() - 1; i >= 0; --i) {
            const StateID parent = parents[i];
            assert(parent != rid);
            assert(state_infos_[parent].states[0] == parent);
            const std::vector<StateID>& parent_states =
                state_infos_[parent].states;
            for (int j = parent_states.size() - 1; j >= 0; --j) {
                auto& entry = gen_->lookup(parent_states[j]);
                const uint32_t* aop = entry.aops;
                uint32_t* succ = entry.succs;
                for (int k = entry.naops - 1; k >= 0; --k, ++aop) {
                    for (int l = (gen_->first_op_ + *aop)->num_outcomes() - 1;
                         l >= 0;
                         --l, ++succ) {
                        if (states_set.count(*succ)) {
                            *succ = rid;
                        }
                        // *succ = state_infos_[*succ].states[0];
                        assert(state_infos_[*succ].states[0] == *succ);
                    }
                }
            }
        }

#if 0
        {

            auto substitute =
                [](const ProbabilisticOperator* first_op,
                   const std::set<uint32_t>& states_set,
                   const StateID& rid,
                   TransitionGenerator<
                       const ProbabilisticOperator*>::CacheEntry& entry) {
                    const uint32_t* aop = entry.aops;
                    uint32_t* succ = entry.succs;
                    for (int k = entry.naops - 1; k >= 0; --k, ++aop) {
                        for (int l = (first_op + *aop)->num_outcomes() - 1;
                             l >= 0;
                             --l, ++succ) {
                            if (states_set.count(*succ)) {
                                *succ = rid;
                            }
                            // assert(state_infos_[*succ].states[0] == *succ);
                        }
                    }
                };

            unsigned i = 0;
            auto state = states_set.begin();
            auto parent = parents.begin();
            while (state != states_set.end() && parent != parents.end()) {
                if (*state < *parent) {
                    ++state;
                } else if (*state == *parent) {
                    ++state;
                    ++parent;
                } else {
                    if (i == 0 || parents[i - 1] != *parent) {
                        parents[i++] = *parent;
                        assert(*parent != rid);
                        assert(state_infos_[*parent].states[0] == *parent);
                        auto& entry = gen_->lookup(*parent);
                        substitute(gen_->first_op_, states_set, rid, entry);
                    }
                    ++parent;
                }
            }
            while (parent != parents.end()) {
                if (i == 0 || parents[i - 1] != *parent) {
                    parents[i++] = *parent;
                    assert(*parent != rid);
                    assert(state_infos_[*parent].states[0] == *parent);
                    auto& entry = gen_->lookup(*parent);
                    substitute(gen_->first_op_, states_set, rid, entry);
                }
                ++parent;
            }

            parents.resize(i);
            parents.shrink_to_fit();
        }
#endif

#ifndef NDEBUG
        {
            const QuotientInformation& qinfo = state_infos_[rid];
            assert(!qinfo.states.empty());
            assert(qinfo.states[0] == rid);
            assert(
                !std::count(qinfo.parents.begin(), qinfo.parents.end(), rid));
            std::vector<StateID> uqs = qinfo.states;
            std::sort(uqs.begin(), uqs.end());
            assert(std::unique(uqs.begin(), uqs.end()) == uqs.end());
            uqs = qinfo.parents;
            std::sort(uqs.begin(), uqs.end());
            assert(std::unique(uqs.begin(), uqs.end()) == uqs.end());
        }

#if 0
        std::cout << "new quotient #" << rid << " -> [";
        for (auto it = begin; it != end; ++it) {
            std::cout << (it != begin ? ", " : "") << (*it);
        }
        std::cout << "] === [";
        for (unsigned i = 0; i < states.size(); ++i) {
            std::cout << (i > 0 ? ", " : "") << states[i];
        }
        std::cout << "]" << std::endl;
#endif

        verify_cache_consistency();

#endif
    }

private:
    struct QuotientInformation {
        std::vector<StateID> parents;
        std::vector<StateID> states;
        explicit QuotientInformation(const StateID& s)
            : states({ s })
        {
        }
    };

    void update_cache(
        const void*,
        TransitionGenerator<Action>::CacheEntry& entry,
        const uint32_t rid,
        const std::unordered_set<uint32_t>& quotient_states)
    {
        unsigned new_size = 0;
        uint32_t* aops_src = entry.aops;
        uint32_t* aops_dest = entry.aops;
        uint32_t* succ_src = entry.succs;
        uint32_t* succ_dest = entry.succs;
        for (int i = entry.naops - 1; i >= 0; --i, ++aops_src) {
            bool self_loop = true;
            uint32_t* k = succ_dest;
            for (int j = (gen_->first_op_ + *aops_src)->num_outcomes() - 1;
                 j >= 0;
                 --j, ++succ_dest, ++succ_src) {
                *succ_dest =
                    (quotient_states.count(*succ_src) ? rid : *succ_src);
                self_loop = self_loop && (*succ_dest == rid);
            }
            if (self_loop) {
                succ_dest = k;
            } else {
                *aops_dest = *aops_src;
                ++aops_dest;
                ++new_size;
            }
        }
        entry.naops = new_size;
    }

    void update_cache(
        const std::vector<const ProbabilisticOperator*>& exclude,
        TransitionGenerator<Action>::CacheEntry& entry,
        const uint32_t rid,
        const std::unordered_set<uint32_t>& quotient_states)
    {
        unsigned new_size = 0;
        uint32_t* aops_src = entry.aops;
        uint32_t* aops_dest = entry.aops;
        uint32_t* succ_src = entry.succs;
        uint32_t* succ_dest = entry.succs;
        for (int i = entry.naops - 1; i >= 0; --i, ++aops_src) {
            bool self_loop = true;
            uint32_t* k = succ_dest;
            const ProbabilisticOperator* op = (gen_->first_op_ + *aops_src);
            if (std::count(exclude.begin(), exclude.end(), op)) {
                continue;
            }
            for (int j = op->num_outcomes() - 1; j >= 0;
                 --j, ++succ_dest, ++succ_src) {
                *succ_dest =
                    (quotient_states.count(*succ_src) ? rid : *succ_src);
                self_loop = self_loop && (*succ_dest == rid);
            }
            if (self_loop) {
                succ_dest = k;
            } else {
                *aops_dest = *aops_src;
                ++aops_dest;
                ++new_size;
            }
        }
        entry.naops = new_size;
    }

    void verify_cache_consistency();

    const QuotientInformation* get_infos(const StateID& sid) const;
    const TransitionGenerator<Action>::CacheEntry& lookup(const StateID& sid);
    const TransitionGenerator<Action>::CacheEntry&
    lookup(const StateID& sid) const;

    const bool cache_;

    segmented_vector::SegmentedVector<QuotientInformation> state_infos_;
    TransitionGenerator<Action>* gen_;

    std::unique_ptr<DefaultQuotientSystem<const ProbabilisticOperator*>>
        fallback_;
};

} // namespace quotient_system
} // namespace probabilistic
