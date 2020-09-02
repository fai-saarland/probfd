#pragma once

#include "types_common.h"
#include "types_heuristic_search.h"
#include "types_storage.h"

#include <deque>
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
namespace algorithms {
namespace quotient_system {

template<typename State, typename Action>
class QuotientAction : public std::pair<State, Action> {
public:
    using std::pair<State, Action>::pair;
};

template<typename S, typename A>
class QuotientState : public S {
public:
    using State = S;
    using Action = A;
    QuotientState(const QuotientState& x) = default;
    QuotientState(QuotientState&&) = default;
    QuotientState(const S& x)
        : S(x)
    {
    }
    QuotientState(S&& x)
        : S(std::move(x))
    {
    }
    bool operator==(const QuotientState& other) const
    {
        return static_cast<const S&>(*this) == static_cast<const S&>(other);
    }
    QuotientState& operator=(const QuotientState& x) = default;
    QuotientState& operator=(QuotientState&& x)
    {
        S::operator=(std::move(x));
        return *this;
    }
    QuotientState& operator=(const S& x)
    {
        S::operator=(x);
        return *this;
    }
    QuotientState& operator=(S&& x)
    {
        S::operator=(std::move(x));
        return *this;
    }
};

template<typename Action>
struct QuotientInformation {
    QuotientInformation()
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
    Action* aops;
};

template<typename A>
using QuotientMap = std::unordered_map<unsigned, QuotientInformation<A>>;

using QIDMap = storage::StateHashMap<unsigned>;

template<typename BaseStateT, typename BaseActionT>
class QuotientSystem {
public:
    using State = BaseStateT;
    using Action = BaseActionT;
    using QState = QuotientState<State, Action>;
    using QAction = QuotientAction<State, Action>;
    using QInfo = QuotientInformation<Action>;
    using QMap = QuotientMap<Action>;
    using QuotientStateIDIterator = const StateID*;
    using iterator = typename QMap::const_iterator;

    static const unsigned SELF = -1;

    explicit QuotientSystem(
        bool remove_self_loops,
        StateIDMap<State>* id_map,
        ApplicableActionsGenerator<State, Action>* aops_gen,
        TransitionGenerator<State, Action>* transition_gen)
        : remove_self_loops_(remove_self_loops)
        , next_id_(0)
        , quotients_()
        , quotient_ids_(SELF)
        , state_id_map_(id_map)
        , aops_gen_(aops_gen)
        , transition_gen_(transition_gen)
    {
    }

    iterator begin() const { return quotients_.begin(); }
    iterator end() const { return quotients_.end(); }

    unsigned quotient_size(const StateID& state_id) const
    {
        const QInfo* info = get_quotient_info(state_id);
        return info == nullptr ? 1 : info->size;
    }

    QuotientStateIDIterator quotient_begin(const iterator& it) const
    {
        return it->second.states;
    }

    QuotientStateIDIterator quotient_end(const iterator& it) const
    {
        return it->second.states + it->second.size;
    }

    std::pair<QuotientStateIDIterator, QuotientStateIDIterator>
    quotient_iterator(const StateID& state_id) const
    {
        std::pair<QuotientStateIDIterator, QuotientStateIDIterator> result;
        const QInfo* info = get_quotient_info(state_id);
        result.first = info == nullptr ? &state_id : info->states;
        result.second =
            info == nullptr ? (&state_id + 1) : (info->states + info->size);
        return result;
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
        unsigned qid = -1;

        unsigned midx = 0;
        unsigned aops_start = 0;
        std::vector<StateID> states;
        std::vector<unsigned> naops;
        std::vector<Action> aops_merged;
        std::vector<Action> aops;

        // std::cout << "new quotient [";
        for (auto it = begin; it != end; ++it, ++ignore_actions) {
            const typename StateIDIterator::reference state_id = *it;
            const unsigned qsqid = quotient_ids_[state_id];
            // std::cout << " " << state_id << std::flush;
            if (qsqid == SELF) {
                states.push_back(state_id);
                // if (states[midx].hash() > state_id.hash()) {
                if (state_id == rid) {
                    midx = states.size() - 1;
                    aops_start = aops_merged.size();
                }
                State qs = this->state_id_map_->operator[](state_id);
                aops_gen_->operator()(qs, aops);
                if (std::is_same<IgnoreActionsIterator, const void**>::value) {
                    aops_merged.insert(
                        aops_merged.end(), aops.begin(), aops.end());
                    naops.push_back(aops.size());
                } else {
                    unsigned n = 0;
                    for (unsigned i = 0; i < aops.size(); ++i) {
                        if (!is_ignored(*ignore_actions, aops[i])) {
                            aops_merged.push_back(aops[i]);
                            ++n;
                        }
                    }
                    naops.push_back(n);
                }
                aops.clear();
            } else {
                qid = std::min(qsqid, qid);
                auto qit = quotients_.find(qsqid);
                assert(qit != quotients_.end());
                const QInfo& q = qit->second;
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
                if (std::is_same<IgnoreActionsIterator, const void**>::value) {
                    naops.insert(naops.end(), q.naops, q.naops + q.size);
                    aops_merged.insert(
                        aops_merged.end(), q.aops, q.aops + q.naops[q.size]);
                } else {
                    const StateID* sid = q.states;
                    const unsigned* nops = q.naops;
                    const Action* aops = q.aops;
                    for (int i = q.size; i > 0; --i, ++sid, ++nops) {
                        unsigned n = 0;
                        for (int j = *nops; j > 0; --j, ++aops) {
                            if (!is_ignored(*ignore_actions, *sid, *aops)) {
                                aops_merged.push_back(*aops);
                                ++n;
                            }
                        }
                        naops.push_back(n);
                    }
                }
                quotients_.erase(qit);
            }
        }
        // std::cout << " ]" << std::endl;

        if (qid == (unsigned)-1) {
            qid = next_id_++;
        }

        QInfo& qinfo = quotients_[qid];
        assert(qinfo.size == 0);

        qinfo.size = states.size();
        qinfo.states = new StateID[qinfo.size];
        qinfo.naops = new unsigned[qinfo.size + 1];
        qinfo.aops = new Action[aops_merged.size()];
        qinfo.naops[qinfo.size] = aops_merged.size();

        assert(states[midx] == rid);

        if (midx == 0) {
            for (int i = aops_merged.size() - 1; i >= 0; --i) {
                qinfo.aops[i] = aops_merged[i];
            }
            for (int i = qinfo.size - 1; i >= 0; --i) {
                qinfo.states[i] = states[i];
                qinfo.naops[i] = naops[i];
                quotient_ids_[states[i]] = qid;
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
            quotient_ids_[rid] = qid;
            for (unsigned i = 0; i < midx; ++i) {
                qinfo.states[i + 1] = states[i];
                qinfo.naops[i + 1] = naops[i];
                quotient_ids_[states[i]] = qid;
            }
            for (unsigned i = midx + 1; i < qinfo.size; ++i) {
                qinfo.states[i] = states[i];
                qinfo.naops[i] = naops[i];
                quotient_ids_[states[i]] = qid;
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
                State state = this->state_id_map_->operator[](qinfo.states[i]);
                this->aops_gen_->operator()(state, aops);
                QUOTIENT_DEBUG_MSG(if (aops.size() != qinfo.naops[i]) {
                    std::cout << "naops mismatch @i=" << i << " ("
                              << qinfo.states[i] << ": " << qinfo.naops[i]
                              << " != " << aops.size() << ")" << std::endl;
                })
                assert(aops.size() == qinfo.naops[i]);
                for (unsigned k = 0; k < qinfo.naops[i]; ++k, ++j) {
                    QUOTIENT_DEBUG_MSG(if (qinfo.aops[j] != aops[k]) {
                        std::cout << "aops mismatch @j=" << j << ": i=" << i
                                  << " state=" << qinfo.states[i] << " k=" << k
                                  << "/" << (qinfo.naops[i] - 1) << std::endl;
                    })
                    assert(qinfo.aops[j] == aops[k]);
                }
                aops.clear();
            }
        }
#endif
    }

    const QInfo* get_quotient_info(const StateID& state_id) const
    {
        const unsigned qid = quotient_ids_[state_id];
        if (qid == SELF) {
            return nullptr;
        } else {
            return &quotients_.find(qid)->second;
        }
    }

    const QInfo* get_quotient_info(const State& qstate) const
    {
        const StateID state_id = state_id_map_->operator[](qstate);
        return get_quotient_info(state_id);
    }

    StateID get_id(const QState& state) const
    {
        return this->state_id_map_->operator[](state);
    }

    StateID get_representative_id(const State& state) const
    {
        const StateID id = state_id_map_->operator[](state);
        return get_representative_id(id);
    }

    StateID get_representative_id(const StateID& state_id) const
    {
        const QInfo* qinfo = this->get_quotient_info(state_id);
        if (qinfo == nullptr) {
            return state_id;
        } else {
            return qinfo->states[0];
        }
    }

    QState get_quotient_state(const StateID& state_id) const
    {
        const StateID rid = get_representative_id(state_id);
        return QState(this->state_id_map_->operator[](rid));
    }

    QState get_quotient_state(const State& state) const
    {
        const StateID rid = get_representative_id(state);
        return QState(this->state_id_map_->operator[](rid));
    }

    StateIDMap<QState>* create_state_id_map()
    {
        return new StateIDMap<QState>(this);
    }

    ApplicableActionsGenerator<QState, QAction>* create_aops_generator()
    {
        return new ApplicableActionsGenerator<QState, QAction>(
            this, state_id_map_, aops_gen_);
    }

    TransitionGenerator<QState, QAction>* create_transition_generator()
    {
        return new TransitionGenerator<QState, QAction>(this, transition_gen_);
    }

    StateEvaluationFunction<QState>*
    create_state_evaluator(StateEvaluationFunction<State>* orig)
    {
        return new StateEvaluationFunction<QState>(orig);
    }

    TransitionRewardFunction<QState, QAction>*
    create_transition_reward_function(
        TransitionRewardFunction<State, Action>* orig)
    {
        return new TransitionRewardFunction<QState, QAction>(orig);
    }

    // PolicyChooser<QState, QAction>*
    // create_policy_chooser(PolicyChooser<State, Action>* orig);
    TransitionSampler<QState, QAction>*
    create_transition_sampler(TransitionSampler<State, Action>* orig)
    {
        return new TransitionSampler<QState, QAction>(this, orig);
    }

    StateListener<QState, QAction>*
    create_state_listener(StateListener<State, Action>* orig)
    {
        return new StateListener<QState, QAction>(this, state_id_map_, orig);
    }

    NewStateHandler<QState, QAction>*
    create_new_state_handler(NewStateHandler<State, Action>* orig)
    {
        return new NewStateHandler<QState, QAction>(orig);
    }

    StateIDMap<State>* get_original_state_id_map() const
    {
        return state_id_map_;
    }

private:
    inline bool is_ignored(const void*, const Action&) const { return false; }
    inline bool is_ignored(const void*, const StateID&, const Action&) const
    {
        return false;
    }

    inline bool
    is_ignored(const std::vector<QAction>& ign, const Action& a) const
    {
        return std::count(ign.begin(), ign.end(), [&a](const QAction& qa) {
            return qa.second == a;
        });
    }

    inline bool
    is_ignored(const std::vector<Action>& ign, const Action& a) const
    {
        return std::count(ign.begin(), ign.end(), a);
    }

    inline bool
    is_ignored(const std::vector<Action>&, const StateID&, const Action&) const
    {
        return false;
    }

    inline bool is_ignored(
        const std::vector<QAction>& ign,
        const StateID& state_id,
        const Action& a) const
    {
        return std::count(
            ign.begin(), ign.end(), [this, &state_id, &a](const QAction& qa) {
                return qa.second == a
                    && state_id_map_->operator[](qa.first) == state_id;
            });
    }

    const bool remove_self_loops_;
    unsigned next_id_;
    QMap quotients_;
    mutable storage::PerStateStorage<unsigned> quotient_ids_;
    StateIDMap<State>* state_id_map_;
    ApplicableActionsGenerator<State, Action>* aops_gen_;
    TransitionGenerator<State, Action>* transition_gen_;
};

template<typename StateT, typename ActionT>
class QuotientSystemFunctionRegistry {
public:
    using State = StateT;
    using Action = ActionT;
    using QSystem = QuotientSystem<StateT, ActionT>;
    using QState = typename QSystem::QState;
    using QAction = typename QSystem::QAction;

    explicit QuotientSystemFunctionRegistry(QSystem* q)
        : q_(q)
        , id_map_(q->create_state_id_map())
        , aops_(q->create_aops_generator())
        , trans_(q->create_transition_generator())
        , state_evals_()
        , transition_evals_()
        , transition_sampler_()
        , state_listener_()
        , new_state_handlers_()
    {
    }

    ~QuotientSystemFunctionRegistry()
    {
        delete (id_map_);
        delete (aops_);
        delete (trans_);
        delete_ptrs(state_evals_);
        delete_ptrs(transition_evals_);
        delete_ptrs(transition_sampler_);
        delete_ptrs(state_listener_);
        delete_ptrs(new_state_handlers_);
        delete_ptrs(open_lists_);
    }

    algorithms::StateIDMap<QState>* get_id_map() { return id_map_; }

    algorithms::ApplicableActionsGenerator<QState, QAction>* get_aops_gen()
    {
        return aops_;
    }

    algorithms::TransitionGenerator<QState, QAction>* get_transition_gen()
    {
        return trans_;
    }

    algorithms::StateEvaluationFunction<QState>*
    create(algorithms::StateEvaluationFunction<State>* x)
    {
        if (x == nullptr) {
            return nullptr;
        }
        state_evals_.push_back(q_->create_state_evaluator(x));
        return state_evals_.back();
    }

    algorithms::TransitionRewardFunction<QState, QAction>*
    create(algorithms::TransitionRewardFunction<State, Action>* x)
    {
        if (x == nullptr)
            return nullptr;
        transition_evals_.push_back(q_->create_transition_reward_function(x));
        return transition_evals_.back();
    }

    algorithms::TransitionSampler<QState, QAction>*
    create(algorithms::TransitionSampler<State, Action>* x)
    {
        if (x == nullptr)
            return nullptr;
        transition_sampler_.push_back(q_->create_transition_sampler(x));
        return transition_sampler_.back();
    }

    algorithms::StateListener<QState, QAction>*
    create(algorithms::StateListener<State, Action>* x)
    {
        if (x == nullptr)
            return nullptr;
        state_listener_.push_back(q_->create_state_listener(x));
        return state_listener_.back();
    }

    algorithms::NewStateHandler<QState, QAction>*
    create(algorithms::NewStateHandler<State, Action>* x)
    {
        if (x == nullptr)
            return nullptr;
        new_state_handlers_.push_back(q_->create_new_state_handler(x));
        return new_state_handlers_.back();
    }

    algorithms::OpenList<QState, QAction>*
    create(algorithms::OpenList<State, Action>* x)
    {
        if (x == nullptr)
            return nullptr;
        open_lists_.push_back(new algorithms::OpenList<QState, QAction>(q_, x));
        return open_lists_.back();
    }

    QSystem* quotient() const { return q_; }

private:
    template<typename T>
    static void delete_ptrs(std::vector<T*>& v)
    {
        for (int i = v.size() - 1; i >= 0; --i) {
            delete (v[i]);
        }
        v.clear();
    }

    QSystem* q_;

    algorithms::StateIDMap<QState>* id_map_;
    algorithms::ApplicableActionsGenerator<QState, QAction>* aops_;
    algorithms::TransitionGenerator<QState, QAction>* trans_;

    std::vector<algorithms::StateEvaluationFunction<QState>*> state_evals_;
    std::vector<algorithms::TransitionRewardFunction<QState, QAction>*>
        transition_evals_;
    std::vector<algorithms::TransitionSampler<QState, QAction>*>
        transition_sampler_;
    std::vector<algorithms::StateListener<QState, QAction>*> state_listener_;
    std::vector<algorithms::NewStateHandler<QState, QAction>*>
        new_state_handlers_;
    std::vector<algorithms::OpenList<QState, QAction>*> open_lists_;
};

} // namespace quotient_system

template<typename State, typename Action>
class StateIDMap<quotient_system::QuotientState<State, Action>> {
public:
    using Q = quotient_system::QuotientSystem<State, Action>;
    using QState = typename Q::QState;

    StateIDMap(const Q* system)
        : system_(system)
    {
    }

    StateID operator[](const QState& s)
    {
        return system_->get_representative_id(s);
    }

    QState operator[](const StateID& sid)
    {
        return system_->get_quotient_state(sid);
    }

    const Q* quotient() const { return system_; }

private:
    const Q* system_;
};

template<typename State, typename Action>
class ApplicableActionsGenerator<
    quotient_system::QuotientState<State, Action>,
    quotient_system::QuotientAction<State, Action>> {
public:
    using Q = quotient_system::QuotientSystem<State, Action>;
    using QState = typename Q::QState;
    using QAction = typename Q::QAction;
    using QInfo = typename Q::QInfo;

    ApplicableActionsGenerator(
        const Q* sys,
        StateIDMap<State>* id_map,
        ApplicableActionsGenerator<State, Action>* aops_gen)
        : sys_(sys)
        , id_map_(id_map)
        , aops_gen_(aops_gen)
    {
    }

    void operator()(const QState& qs, std::vector<QAction>& result)
    {
        const QInfo* qinfo = sys_->get_quotient_info(qs);
        if (qinfo == nullptr) {
            aops_gen_->operator()(qs, aops_);
            result.reserve(aops_.size());
            for (unsigned i = 0; i < aops_.size(); ++i) {
                result.emplace_back(qs, aops_[i]);
            }
            aops_.clear();
        } else {
            result.reserve(qinfo->naops[qinfo->size]);
            const StateID* stateid = qinfo->states;
            const unsigned* n = qinfo->naops;
            const Action* a = qinfo->aops;
            for (int i = qinfo->size; i > 0; --i, ++n, ++stateid) {
                State s = id_map_->operator[](*stateid);
                for (int j = *n; j > 0; --j, ++a) {
                    result.emplace_back(s, *a);
                }
            }
            assert(result.size() == *n);
        }
    }

    ApplicableActionsGenerator<State, Action>* base() const
    {
        return aops_gen_;
    }

private:
    const Q* sys_;
    StateIDMap<State>* id_map_;
    ApplicableActionsGenerator<State, Action>* aops_gen_;
    std::vector<Action> aops_;
};

template<typename State, typename Action>
class TransitionGenerator<
    quotient_system::QuotientState<State, Action>,
    quotient_system::QuotientAction<State, Action>> {
public:
    using Q = quotient_system::QuotientSystem<State, Action>;
    using QState = typename Q::QState;
    using QAction = typename Q::QAction;

    TransitionGenerator(const Q* sys, TransitionGenerator<State, Action>* tgen)
        : sys_(sys)
        , tgen_(tgen)
        , transition_()
    {
    }

    Distribution<QState> operator()(const QState&, const QAction& qa)
    {
        transition_ = tgen_->operator()(qa.first, qa.second);
        Distribution<QState> result;
        for (auto it = transition_.begin(); it != transition_.end(); ++it) {
            result.add(sys_->get_quotient_state(it->first), it->second);
        }
        transition_.clear();
        return result;
    }

    TransitionGenerator<State, Action>* base() const { return tgen_; }

private:
    const Q* sys_;
    TransitionGenerator<State, Action>* tgen_;
    Distribution<State> transition_;
};

template<typename State, typename Action>
class StateEvaluationFunction<quotient_system::QuotientState<State, Action>> {
public:
    using Q = quotient_system::QuotientSystem<State, Action>;
    using QState = typename Q::QState;

    explicit StateEvaluationFunction(StateEvaluationFunction<State>* eval)
        : eval_(eval)
    {
    }

    EvaluationResult operator()(const QState& qstate)
    {
        return eval_->operator()(qstate);
    }

    StateEvaluationFunction<State>* base() const { return eval_; }

private:
    StateEvaluationFunction<State>* eval_;
};

template<typename State, typename Action>
class TransitionRewardFunction<
    quotient_system::QuotientState<State, Action>,
    quotient_system::QuotientAction<State, Action>> {
public:
    using Q = quotient_system::QuotientSystem<State, Action>;
    using QState = typename Q::QState;
    using QAction = typename Q::QAction;

    TransitionRewardFunction(TransitionRewardFunction<State, Action>* treward)
        : reward_(treward)
    {
    }

    value_type::value_t operator()(const QState&, const QAction& a)
    {
        return this->reward_->operator()(a.first, a.second);
    }

    TransitionRewardFunction<State, Action>* base() const { return reward_; }

private:
    TransitionRewardFunction<State, Action>* reward_;
};

template<typename State, typename Action>
class TransitionSampler<
    quotient_system::QuotientState<State, Action>,
    quotient_system::QuotientAction<State, Action>> {
public:
    using Q = quotient_system::QuotientSystem<State, Action>;
    using QState = typename Q::QState;
    using QAction = typename Q::QAction;

    TransitionSampler(const Q* sys, TransitionSampler<State, Action>* sampler)
        : sys_(sys)
        , sampler_(sampler)
    {
    }

    QState operator()(
        const QState&,
        const QAction& qaction,
        const Distribution<QState>& transition)
    {
        d_.clear();
        for (auto it = transition.begin(); it != transition.end(); ++it) {
            d_.add(QState(it->first), it->second);
        }
        return QState(sampler_->operator()(qaction.first, qaction.second, d_));
    }

    TransitionSampler<State, Action>* base() const { return sampler_; }

    void connect(HeuristicSearchStatusInterface<QState, QAction>* hsi)
    {
        sampler_->connect(
            reinterpret_cast<HeuristicSearchStatusInterface<State, Action>*>(
                hsi));
    }

private:
    const Q* sys_;
    TransitionSampler<State, Action>* sampler_;
    Distribution<State> d_;
};

template<typename State, typename Action>
class OpenList<
    quotient_system::QuotientState<State, Action>,
    quotient_system::QuotientAction<State, Action>> {
public:
    using Q = quotient_system::QuotientSystem<State, Action>;
    using QState = typename Q::QState;
    using QAction = typename Q::QAction;

    OpenList(const Q* sys, OpenList<State, Action>* orig)
        : sys_(sys)
        , list_(orig)
    {
    }

    bool empty() { return list_->empty(); }

    unsigned size() { return list_->size(); }

    void clear() { return list_->clear(); }

    StateID pop() { return list_->pop(); }

    void push(const StateID& state_id, const QState& state)
    {
        list_->push(state_id, state);
    }

    void push(
        const StateID& state_id,
        const QState&,
        const QAction& action,
        const value_type::value_t& prob,
        const QState& state)
    {
        list_->push(state_id, action.first, action.second, prob, state);
    }

    OpenList<State, Action>* base() const { return list_; }

private:
    const Q* sys_;
    OpenList<State, Action>* list_;
};

template<typename State, typename Action>
class StateListener<
    quotient_system::QuotientState<State, Action>,
    quotient_system::QuotientAction<State, Action>> {
public:
    using Q = quotient_system::QuotientSystem<State, Action>;
    using QState = typename Q::QState;
    using QAction = typename Q::QAction;
    using QInfo = typename Q::QInfo;

    explicit StateListener(
        const Q* sys,
        StateIDMap<State>* id_map,
        StateListener<State, Action>* listener)
        : sys_(sys)
        , id_map_(id_map)
        , listener_(listener)
    {
    }

    bool operator()(const QState& qs)
    {
        const QInfo* info = sys_->get_quotient_info(qs);
        if (info == nullptr) {
            return listener_->operator()(qs);
        } else {
            const StateID* sid = info->states;
            for (int i = info->size; i > 0; --i, ++sid) {
                col_.push_back(id_map_->operator[](*sid));
            }
            const bool r = listener_->operator()(col_.begin(), col_.end());
            col_.clear();
            return r;
        }
    }

    bool operator()(
        const typename std::deque<QState>::const_iterator begin,
        const typename std::deque<QState>::const_iterator end)
    {
        for (auto it = begin; it != end; ++it) {
            const QInfo* info = sys_->get_quotient_info(*it);
            if (info == nullptr) {
                col_.push_back(*it);
            } else if (ids_.insert(info->states[0]).second) {
                const StateID* sid = info->states;
                for (int i = info->size; i > 0; --i, ++sid) {
                    col_.push_back(id_map_->operator[](*sid));
                }
            }
        }
        const bool r = listener_->operator()(col_.begin(), col_.end());
        col_.clear();
        ids_.clear();
        return r;
    }

    StateListener<State, Action>* base() const { return listener_; }

private:
    const Q* sys_;
    StateIDMap<State>* id_map_;
    StateListener<State, Action>* listener_;
    std::deque<State> col_;
    std::unordered_set<StateID> ids_;
};

template<typename State, typename Action>
class NewStateHandler<
    quotient_system::QuotientState<State, Action>,
    quotient_system::QuotientAction<State, Action>> {
public:
    using Q = quotient_system::QuotientSystem<State, Action>;
    using QState = typename Q::QState;
    using QAction = typename Q::QAction;

    NewStateHandler(NewStateHandler<State, Action>* orig)
        : orig_(orig)
    {
    }

    void touch(const QState& qstate) const { orig_->touch(qstate); }
    void touch_dead_end(const QState& qstate) const
    {
        orig_->touch_dead_end(qstate);
    }
    void touch_goal(const QState& qstate) const { orig_->touch_goal(qstate); }

    NewStateHandler<State, Action>* base() const { return orig_; }

private:
    NewStateHandler<State, Action>* orig_;
};

namespace heuristic_search_base {
namespace internal {

template<typename RealStateInfo, typename State, typename Action>
class HeuristicSearchStatusInterface;

template<typename RealStateInfo, typename State, typename Action>
class HeuristicSearchStatusInterface<
    RealStateInfo,
    typename quotient_system::QuotientState<State, Action>,
    typename quotient_system::QuotientAction<State, Action>>
    : public algorithms::HeuristicSearchStatusInterface<
          typename quotient_system::QuotientState<State, Action>,
          typename quotient_system::QuotientAction<State, Action>>,
      public algorithms::HeuristicSearchStatusInterface<State, Action> {
public:
    using Q = quotient_system::QuotientSystem<State, Action>;
    using QState = typename Q::QState;
    using QAction = typename Q::QAction;

    HeuristicSearchStatusInterface(
        StateIDMap<QState>* state_id_map,
        storage::PerStateStorage<RealStateInfo>* infos)
        : state_id_map_(state_id_map->quotient()->get_original_state_id_map())
        , infos_(infos)
    {
    }

    virtual const HeuristicSearchStateBaseInformation*
    lookup_state_info(const QState& state) override
    {
        return &infos_->operator[](state_id_map_->operator[](state));
    }

    virtual const HeuristicSearchStateBaseInformation*
    lookup_state_info(const State& state) override
    {
        return &infos_->operator[](state_id_map_->operator[](state));
    }

private:
    StateIDMap<State>* state_id_map_;
    storage::PerStateStorage<RealStateInfo>* infos_;
};

} // namespace internal
} // namespace heuristic_search_base

} // namespace algorithms
} // namespace probabilistic

namespace std {
template<typename State, typename Action>
struct hash<
    probabilistic::algorithms::quotient_system::QuotientState<State, Action>> {
    size_t operator()(const probabilistic::algorithms::quotient_system::
                          QuotientState<State, Action>& s) const
    {
        return hash<State>()(s);
    }
};

} // namespace std

#undef QUOTIENT_DEBUG_MSG
