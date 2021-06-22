#pragma once

#include "../../algorithms/segmented_vector.h"
#include "../engine_interfaces/action_id_map.h"
#include "../engine_interfaces/applicable_actions_generator.h"
#include "../engine_interfaces/transition_generator.h"

#include <deque>
#include <iterator>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#if !defined(NDEBUG)
#define QUOTIENT_DEBUG_MSG(x) // x
#else
#define QUOTIENT_DEBUG_MSG(x) // x
#endif

namespace probabilistic {
namespace quotient_system {

template<typename ActionT>
struct QuotientAction {
    using Action = ActionT;

    StateID state_id;
    ActionID action_id;

    explicit QuotientAction(const StateID& state_id, const ActionID& action_id)
        : state_id(state_id)
        , action_id(action_id)
    {
    }

    QuotientAction(const QuotientAction&) = default;
    QuotientAction(QuotientAction&&) = default;

    QuotientAction& operator=(const QuotientAction&) = default;
    QuotientAction& operator=(QuotientAction&&) = default;
};

struct QuotientInformation {
    explicit QuotientInformation()
        : size(0)
        , states(nullptr)
        , naops(nullptr)
        , aops(nullptr)
    {
    }

    QuotientInformation(QuotientInformation&& other)
        : size(std::move(other.size))
        , states(std::move(other.states))
        , naops(std::move(other.naops))
        , aops(std::move(other.aops))
    {
        other.size = 0;
    }

    QuotientInformation(const QuotientInformation&) = delete;

    ~QuotientInformation()
    {
        if (size) {
            delete[](states);
            delete[](naops);
            delete[](aops);
            size = 0;
        }
    }

    QuotientInformation& operator=(const QuotientInformation&) = delete;
    QuotientInformation& operator=(QuotientInformation&&) = delete;

    unsigned size;
    StateID* states;
    unsigned* naops;
    ActionID* aops;
};

using QuotientMap = std::unordered_map<StateID::size_type, QuotientInformation>;

template<typename BaseActionT>
class DefaultQuotientSystem {
    friend struct const_iterator;

public:
    using Action = BaseActionT;
    using QAction = QuotientAction<Action>;
    using QuotientStateIDIterator = const StateID*;

    static constexpr const StateID::size_type MASK =
        (StateID::size_type(-1) >> 1);
    static constexpr const StateID::size_type FLAG = ~MASK;

    struct const_iterator {
        using iterator_type = std::forward_iterator_tag;
        using value_type = StateID::size_type;
        using difference_type = int;
        using pointer = const StateID::size_type*;
        using reference = const StateID::size_type&;

        explicit const_iterator(
            const DefaultQuotientSystem* qs,
            StateID::size_type x)
            : i(x)
            , qs_(qs)
        {
        }

        const_iterator& operator++()
        {
            while (++i < qs_->quotient_ids_.size()) {
                const StateID::size_type& ref = qs_->quotient_ids_[i];
                if (i == (ref & DefaultQuotientSystem::MASK)) {
                    break;
                }
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

        reference operator*() const { return i; }

        StateID::size_type i;
    private:
        const DefaultQuotientSystem* qs_;
    };

    explicit DefaultQuotientSystem(
        ActionIDMap<Action>* action_id_map,
        ApplicableActionsGenerator<Action>* aops_gen,
        TransitionGenerator<Action>* transition_gen)
        : quotients_()
        , quotient_ids_()
        , action_id_map_(action_id_map)
        , aops_gen_(aops_gen)
        , transition_gen_(transition_gen)
    {
    }

    const_iterator begin() const { return const_iterator(this, 0); }

    const_iterator end() const
    {
        return const_iterator(this, quotient_ids_.size());
    }

    unsigned quotient_size(const StateID& state_id) const
    {
        const QuotientInformation* info = get_quotient_info(state_id);
        return info == nullptr ? 1 : info->size;
    }

    std::pair<QuotientStateIDIterator, QuotientStateIDIterator>
    quotient_iterator(const StateID& state_id) const
    {
        std::pair<QuotientStateIDIterator, QuotientStateIDIterator> result;
        const QuotientInformation* info = get_quotient_info(state_id);
        result.first = info == nullptr ? &state_id : info->states;
        result.second =
            info == nullptr ? (&state_id + 1) : (info->states + info->size);
        return result;
    }

    void generate_applicable_ops(
        const StateID& sid,
        std::vector<QAction>& result) const
    {
        const QuotientInformation* info = get_quotient_info(sid);
        if (info == nullptr) {
            std::vector<Action> orig;
            this->aops_gen_->operator()(sid, orig);
            result.reserve(orig.size());
            for (unsigned i = 0; i < orig.size(); ++i) {
                result.emplace_back(
                    sid, action_id_map_->get_action_id(sid, orig[i]));
            }
        } else {
            result.reserve(info->naops[info->size]);
            const StateID* qsid = info->states;
            const unsigned* naops = info->naops;
            const ActionID* aid = info->aops;
            for (int i = info->size - 1; i >= 0; --i, ++qsid, ++naops) {
                for (int j = *naops - 1; j >= 0; --j, ++aid) {
                    result.emplace_back(*qsid, *aid);
                }
            }
        }
    }

    void generate_successors(
        const StateID&,
        const QAction& a,
        Distribution<StateID>& result) const
    {
        Distribution<StateID> orig;
        this->transition_gen_->operator()(
            a.state_id,
            this->action_id_map_->get_action(a.state_id, a.action_id),
            orig);
        for (auto it = orig.begin(); it != orig.end(); ++it) {
            result.add(get_masked_state_id(it->first) & MASK, it->second);
        }
    }

    void generate_all_successors(
        const StateID& sid,
        std::vector<QAction>& aops,
        std::vector<Distribution<StateID>>& successors) const
    {
        const QuotientInformation* info = get_quotient_info(sid);
        if (info == nullptr) {
            std::vector<Action> orig_a;
            this->aops_gen_->operator()(sid, orig_a);
            aops.reserve(orig_a.size());
            successors.resize(orig_a.size());
            for (unsigned i = 0; i < orig_a.size(); ++i) {
                aops.emplace_back(
                    sid, this->action_id_map_->get_action_id(sid, orig_a[i]));
                this->generate_successors(sid, aops.back(), successors[i]);
            }
        } else {
            aops.reserve(info->naops[info->size]);
            successors.resize(info->naops[info->size]);
            const StateID* qsid = info->states;
            const unsigned* naops = info->naops;
            const ActionID* aid = info->aops;
            unsigned k = 0;
            for (int i = info->size - 1; i >= 0; --i, ++qsid, ++naops) {
                for (int j = *naops - 1; j >= 0; --j, ++aid, ++k) {
                    aops.emplace_back(*qsid, *aid);
                    this->generate_successors(sid, aops.back(), successors[k]);
                }
            }
        }
    }

    StateID translate_state_id(const StateID& sid) const
    {
        return StateID(this->get_masked_state_id(sid) & MASK);
    }

    ActionID get_original_action_id(const StateID& sid, const ActionID& a) const
    {
        const QuotientInformation* info = get_quotient_info(sid);
        if (info == nullptr) {
            return a;
        } else {
            return info->aops[a];
        }
    }

    ActionID get_action_id(const StateID& sid, const QAction& a) const
    {
        const QuotientInformation* info = get_quotient_info(sid);
        if (info == nullptr) {
            return a.action_id;
        } else {
            ActionID::size_type res = 0;
            const ActionID* aop = info->aops;
            const StateID* sid = info->states;
            unsigned i = 0;
            while (*sid != a.state_id) {
                res += info->naops[i];
                aop += info->naops[i];
                ++sid;
                ++i;
            }
            while (*aop != a.action_id) {
                ++res;
                ++aop;
            }
            return res;
        }
    }

    Action get_original_action(const StateID&, const QAction& a) const
    {
        return action_id_map_->get_action(a.state_id, a.action_id);
    }

    QAction get_action(const StateID& sid, const ActionID& aid) const
    {
        const QuotientInformation* info = get_quotient_info(sid);
        if (info == nullptr) {
            return QAction(sid, aid);
        } else {
            const StateID* qsid = info->states;
            const unsigned* naops = info->naops;
            unsigned sum = *naops;
            while (sum <= aid) {
                ++naops;
                ++qsid;
                sum += *naops;
            }
            return QAction(*qsid, info->aops[aid]);
        }
    }

    template<
        typename StateIDIterator,
        typename IgnoreActionsIterator = const void**>
    void build_quotient(
        StateIDIterator begin,
        StateIDIterator end,
        const StateID& rid,
        IgnoreActionsIterator ignore_actions = nullptr)
    {
        unsigned midx = 0;
        unsigned aops_start = 0;
        std::vector<StateID> states;
        std::vector<unsigned> naops;
        std::vector<ActionID> aops_merged;
        std::vector<Action> aops;

        // std::cout << "new quotient [";
        for (auto it = begin; it != end; ++it) {
            const typename StateIDIterator::reference state_id = *it;
            const StateID::size_type qsqid = get_masked_state_id(state_id);
            // std::cout << " " << state_id << std::flush;
            if (qsqid & FLAG) {
                auto qit = quotients_.find(qsqid);
                assert(qit != quotients_.end());
                const QuotientInformation& q = qit->second;
                // std::cout << ":" << qsqid << "{";
                // for (unsigned i = 0; i < q.size; ++i) {
                //     std::cout << (i > 0 ? "," : "") << (q.states[i]);
                // }
                // std::cout << "}" << std::flush;
                states.insert(states.end(), q.states, q.states + q.size);
                // if (states[midx].hash() > q.states->hash()) {
                if (*q.states == rid) {
                    midx = states.size() - q.size;
                    aops_start = aops_merged.size();
                }

                if constexpr (std::is_same_v<IgnoreActionsIterator, const void**>) {
                    filter_actions(
                        q.size,
                        q.states,
                        q.naops,
                        q.aops,
                        naops,
                        aops_merged);
                } else {
                    filter_actions(
                        q.size,
                        q.states,
                        q.naops,
                        q.aops,
                        *ignore_actions,
                        naops,
                        aops_merged);
                }

                quotients_.erase(qit);
            } else {
                states.push_back(state_id);
                // if (states[midx].hash() > state_id.hash()) {
                if (state_id == rid) {
                    midx = states.size() - 1;
                    aops_start = aops_merged.size();
                }
                aops_gen_->operator()(state_id, aops);

                if constexpr (std::is_same_v<IgnoreActionsIterator, const void**>) {
                    filter_actions(
                        state_id, aops, naops, aops_merged);
                } else {
                    filter_actions(
                        state_id, aops, *ignore_actions, naops, aops_merged);
                }


                aops.clear();
            }
        }
        // std::cout << " ]" << std::endl;

        const StateID::size_type qid = rid | FLAG;
        QuotientInformation& qinfo = quotients_[qid];
        assert(qinfo.size == 0);

        qinfo.size = states.size();
        qinfo.states = new StateID[qinfo.size];
        qinfo.naops = new unsigned[qinfo.size + 1];
        qinfo.aops = new ActionID[aops_merged.size()];
        qinfo.naops[qinfo.size] = aops_merged.size();

        assert(states[midx] == rid);

        if (midx == 0) {
            for (int i = aops_merged.size() - 1; i >= 0; --i) {
                qinfo.aops[i] = aops_merged[i];
            }
            for (int i = qinfo.size - 1; i >= 0; --i) {
                qinfo.states[i] = states[i];
                qinfo.naops[i] = naops[i];
                set_masked_state_id(states[i], qid);
            }
        } else {
            unsigned j = 0;
            const unsigned end = aops_start + naops[midx];
            for (unsigned i = aops_start; i < end; ++i, ++j) {
                qinfo.aops[j] = aops_merged[i];
            }
            for (unsigned i = 0; i < aops_start; ++i, ++j) {
                qinfo.aops[j] = aops_merged[i];
            }
            for (unsigned i = end; i < aops_merged.size(); ++i, ++j) {
                qinfo.aops[j] = aops_merged[i];
            }
            qinfo.states[0] = rid;
            qinfo.naops[0] = naops[midx];
            set_masked_state_id(rid, qid);
            for (unsigned i = 0; i < midx; ++i) {
                qinfo.states[i + 1] = states[i];
                qinfo.naops[i + 1] = naops[i];
                set_masked_state_id(states[i], qid);
            }
            for (unsigned i = midx + 1; i < qinfo.size; ++i) {
                qinfo.states[i] = states[i];
                qinfo.naops[i] = naops[i];
                set_masked_state_id(states[i], qid);
            }
        }

        QUOTIENT_DEBUG_MSG(
            std::cout << "created new quotient (size=" << qinfo.size
                      << ", states=[" << std::flush;
            for (unsigned i = 0; i < qinfo.size; ++i) {
                std::cout << (i > 0 ? ", " : "") << qinfo.states[i];
            } std::cout
            << "], naops=" << qinfo.naops[qinfo.size] << ") where midx=" << midx
            << std::endl;)

#if !defined(NDEBUG)
        if (std::is_same<IgnoreActionsIterator, const void**>::value) {
            unsigned j = 0;
            for (unsigned i = 0; i < qinfo.size; ++i) {
                const StateID state_id = qinfo.states[i];
                this->aops_gen_->operator()(state_id, aops);
                QUOTIENT_DEBUG_MSG(if (aops.size() != qinfo.naops[i]) {
                    std::cout << "naops mismatch @i=" << i << " ("
                              << qinfo.states[i] << ": " << qinfo.naops[i]
                              << " != " << aops.size() << ")" << std::endl;
                })
                assert(aops.size() == qinfo.naops[i]);
                for (unsigned k = 0; k < qinfo.naops[i]; ++k, ++j) {
                    QUOTIENT_DEBUG_MSG(
                        if (qinfo.aops[j]
                            != action_id_map_->get_action_id(
                                state_id, aops[k])) {
                            std::cout << "aops mismatch @j=" << j << ": i=" << i
                                      << " state=" << qinfo.states[i]
                                      << " k=" << k << "/"
                                      << (qinfo.naops[i] - 1) << std::endl;
                        })
                    assert(
                        qinfo.aops[j]
                        == action_id_map_->get_action_id(state_id, aops[k]));
                }
                aops.clear();
            }
        }
#endif

        if constexpr (!std::is_same_v<IgnoreActionsIterator, const void**>) {
            ++ignore_actions;
        }
    }

    ActionIDMap<Action>* get_action_id_map() const { return action_id_map_; }

private:
    inline void filter_actions(
        const StateID& state_id,
        const std::vector<Action>& aops,
        std::vector<unsigned>& naops,
        std::vector<ActionID>& result) const
    {
        naops.push_back(aops.size());
        result.reserve(result.size() + aops.size());
        for (unsigned i = 0; i < aops.size(); ++i) {
            result.push_back(action_id_map_->get_action_id(state_id, aops[i]));
        }
    }

    inline void filter_actions(
        const unsigned size,
        const StateID*,
        const unsigned* naopsx,
        const ActionID* aops,
        std::vector<unsigned>& naops,
        std::vector<ActionID>& result) const
    {
        naops.insert(naops.end(), naopsx, naopsx + size);
        result.insert(result.end(), aops, aops + naopsx[size]);
    }

    inline void filter_actions(
        const StateID& state_id,
        const std::vector<Action>& aops,
        const std::vector<Action>& filter,
        std::vector<unsigned>& naops,
        std::vector<ActionID>& result) const
    {
        unsigned added = 0;
        for (unsigned i = 0; i < aops.size(); ++i) {
            if (!std::count(filter.begin(), filter.end(), aops[i])) {
                result.push_back(
                    action_id_map_->get_action_id(state_id, aops[i]));
                ++added;
            }
        }
        naops.push_back(added);
    }

    inline void filter_actions(
        const unsigned size,
        const StateID* state_id,
        const unsigned* naopsx,
        const ActionID* aops,
        const std::vector<Action>& filter,
        std::vector<unsigned>& naops,
        std::vector<ActionID>& result) const
    {
        for (int i = size - 1; i >= 0; --i, ++state_id, ++naopsx) {
            unsigned added = 0;
            for (int j = *naopsx - 1; j >= 0; --j, ++aops) {
                if (!std::count(
                        filter.begin(),
                        filter.end(),
                        action_id_map_->get_action(*state_id, *aops))) {
                    result.push_back(*aops);
                    ++added;
                }
            }
            naops.push_back(added);
        }
    }

    QuotientInformation* get_quotient_info(const StateID& state_id)
    {
        const StateID::size_type qid = quotient_ids_[state_id];
        if (qid & FLAG) {
            return &quotients_.find(qid)->second;
        } else {
            return nullptr;
        }
    }

    const QuotientInformation* get_quotient_info(const StateID& state_id) const
    {
        const StateID::size_type qid = get_masked_state_id(state_id);
        if (qid & FLAG) {
            return &quotients_.find(qid)->second;
        } else {
            return nullptr;
        }
    }

    StateID::size_type get_masked_state_id(const StateID& sid) const
    {
        if (quotient_ids_.size() <= sid) {
            return sid;
        }
        return quotient_ids_[sid];
    }

    void set_masked_state_id(const StateID& sid, const StateID::size_type& qsid)
    {
        if (quotient_ids_.size() <= sid) {
            for (StateID::size_type idx = quotient_ids_.size(); idx <= sid;
                 ++idx) {
                quotient_ids_.push_back(idx);
            }
        }
        quotient_ids_[sid] = qsid | FLAG;
    }

    QuotientMap quotients_;
    segmented_vector::SegmentedVector<StateID::size_type> quotient_ids_;
    ActionIDMap<Action>* action_id_map_;
    ApplicableActionsGenerator<Action>* aops_gen_;
    TransitionGenerator<Action>* transition_gen_;
};

template<typename ActionT>
class QuotientSystem : public DefaultQuotientSystem<ActionT> {
public:
    using Action = ActionT;
    using QAction = QuotientAction<Action>;
    using QuotientStateIDIterator =
        typename DefaultQuotientSystem<Action>::QuotientStateIDIterator;
    using DefaultQuotientSystem<Action>::DefaultQuotientSystem;
};

} // namespace quotient_system
} // namespace probabilistic

#undef QUOTIENT_DEBUG_MSG
