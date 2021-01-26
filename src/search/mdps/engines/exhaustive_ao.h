#pragma once

#include "../engine_interfaces/open_list.h"
#include "ao_search.h"

#include <iostream>
#include <sstream>

namespace probabilistic {
namespace exhaustive_ao {

namespace internal {

template<typename StateInfo>
struct PerStateInformation : public ao_search::PerStateInformation<StateInfo> {
    PerStateInformation()
        : ao_search::PerStateInformation<StateInfo>()
        , unsolved(0)
        , alive(0)
    {
    }
    unsigned unsolved : 31;
    unsigned alive : 1;
};

template<typename State, typename Action, typename DualBounds>
using AOBase = ao_search::AOBase<
    State,
    Action,
    DualBounds,
    std::false_type,
    PerStateInformation,
    false>;

} // namespace internal

template<typename StateT, typename ActionT, typename DualBoundsT>
class ExhaustiveAOSearch
    : public internal::AOBase<StateT, ActionT, DualBoundsT> {
public:
    using AOBase = internal::AOBase<StateT, ActionT, DualBoundsT>;
    using State = typename AOBase::State;
    using Action = typename AOBase::Action;

    template<typename... Args>
    ExhaustiveAOSearch(OpenList<Action>* open_list, Args... args)
        : AOBase(args...)
        , open_list_(open_list)
    {
    }

    virtual void solve(const State& state) override
    {
        this->initialize_report(state);
        StateID stateid = this->get_state_id(state);
        const auto& state_info = this->state_infos_(stateid);
        open_list_->push(stateid);
        do {
            step();
            this->report(stateid);
        } while (!state_info.is_solved());
        // this->dump_search_space_with_node_infos();
    }

private:
    using StateInfo = typename AOBase::StateInfo;

    friend class StateInfosToString;

    struct StateInfosToString {
        explicit StateInfosToString(ExhaustiveAOSearch* s)
            : s(s)
        {
        }

        std::string operator()(const State& state) const
        {
            const StateID sid = s->get_state_id(state);
            const auto& info = s->state_infos_(sid);
            std::ostringstream out;
            out << sid << ": v=" << info.get_value() << " | "
                << info.is_tip_state() << info.is_marked() << info.is_solved()
                << info.is_unflagged();
            return out.str();
        }

        ExhaustiveAOSearch* s;
    };

    void dump_search_space_with_node_infos()
    {
        StateInfosToString sstr(this);
        this->dump_search_space("eao_search_space.dot", &sstr);
    }

    void step()
    {
        assert(!this->open_list_->empty());
        StateID stateid = this->open_list_->pop();
        auto& info = this->state_infos_(stateid);
        if (!info.is_tip_state() || info.is_solved()) {
            return;
        }
        ++this->statistics_.iterations;
        bool solved = false;
        bool dead = false;
        bool terminal = false;
        bool value_changed = false;
        this->initialize_tip_state_value(
            stateid, info, terminal, solved, dead, value_changed);
        if (terminal) {
            assert(info.is_solved());
        } else {
            unsigned alive = 0;
            unsigned unsolved = 0;
            int min_succ_order = std::numeric_limits<int>::max();
            assert(this->aops_.empty() && this->transitions_.empty());
            this->generate_all_successors(
                stateid, this->aops_, this->transitions_);
            for (int i = this->aops_.size() - 1; i >= 0; i--) {
                const auto& t = this->transitions_[i];
                for (auto it = t.begin(); it != t.end(); ++it) {
                    const StateID succid = it->first;
                    auto& succ_info = this->state_infos_(succid);
                    if (!succ_info.is_solved()) {
                        if (!succ_info.is_marked()) {
                            succ_info.mark();
                            succ_info.add_parent(stateid);
                            min_succ_order = std::min(
                                min_succ_order, succ_info.update_order);
                            ++unsolved;
                        }
                        this->open_list_->push(
                            stateid, this->aops_[i], it->second, succid);
                    } else if (!succ_info.is_dead_end()) {
                        ++alive;
                    }
                }
            }
            info.alive = alive > 0;
            if (unsolved == 0) {
                this->aops_.clear();
                this->transitions_.clear();
                this->mark_solved_push_parents(stateid, info, info.alive == 0);
                this->backpropagate_tip_value();
            } else {
                assert(
                    min_succ_order >= 0
                    && min_succ_order < std::numeric_limits<int>::max());
                info.update_order = min_succ_order + 1;
                info.unsolved = unsolved;

                for (int i = this->transitions_.size() - 1; i >= 0; --i) {
                    const auto& t = this->transitions_[i];
                    for (auto it = t.begin(); it != t.end(); ++it) {
                        const StateID succ_id = it->first;
                        auto& succ_info = this->state_infos_(succ_id);
                        succ_info.unmark();
                    }
                }

                this->aops_.clear();
                this->transitions_.clear();

                this->backpropagate_update_order(stateid);
                if (value_changed) {
                    this->push_parents_to_queue(info);
                    this->backpropagate_tip_value();
                }
            }
        }
    }

    OpenList<Action>* open_list_;
    std::vector<Distribution<StateID>> transitions_;
};

} // namespace exhaustive_ao
} // namespace probabilistic
