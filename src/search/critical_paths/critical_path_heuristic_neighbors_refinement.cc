#include "critical_path_heuristic_neighbors_refinement.h"

#include "../global_operator.h"
#include "../global_state.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../strips.h"
#include "critical_path_heuristic.h"

#include <algorithm>
#include <cassert>

namespace critical_path_heuristic {

CriticalPathHeuristicNeighborsRefinement::RefinementNode::RefinementNode(
    const ConjunctionInfo* conj)
    : conjunction(conj)
    , achiever_index(conj->achievers.size() - 1)
{
}

CriticalPathHeuristicNeighborsRefinement::
    CriticalPathHeuristicNeighborsRefinement(const options::Options& opts)
    : CriticalPathHeuristicRefinement(opts)
{
}

void
CriticalPathHeuristicNeighborsRefinement::add_options_to_parser(
    options::OptionParser& parser)
{
    CriticalPathHeuristicRefinement::add_options_to_parser(parser);
}

bool
CriticalPathHeuristicNeighborsRefinement::
    requires_recognized_neighbors_explicitly() const
{
    return true;
}

std::string
CriticalPathHeuristicNeighborsRefinement::name() const
{
    return "Neighbors";
}

bool
CriticalPathHeuristicNeighborsRefinement::refine_conjunction_set(
    state_component::StateComponent& states,
    state_component::StateComponent& neighbors)
{
    if (initialize_current_state_reachable(states.current())) {
        return true;
    }
    last_num_conjunctions_ = hc_->num_conjunctions();
    store_current_states_fact_relation(states);
    store_neighbors_unreachable_conjunctions(neighbors);
    synchronize_reachability();
    push_for_refinement(task_->get_goal_facts());
    while (!refinement_queue_.empty()) {
        if (!process()) {
            return false;
        }
    }
#if !defined(NDEBUG)
    states.reset();
    hc_->evaluate(states.current());
    assert(hc_->is_dead_end());
#endif
    return true;
}

bool
CriticalPathHeuristicNeighborsRefinement::initialize_current_state_reachable(
    const GlobalState& state)
{
    bool term = hc_->set_exploration_early_termination(false);
    hc_->evaluate(state);
    hc_->set_exploration_early_termination(term);
    if (hc_->is_dead_end()) {
        return true;
    }
    current_states_reachability_.resize(hc_->num_conjunctions());
    std::fill(
        current_states_reachability_.begin(),
        current_states_reachability_.end(),
        true);
    for (int i = current_states_reachability_.size() - 1; i >= 0; i--) {
        current_states_reachability_[i] = hc_->is_conjunction_reachable(i);
    }
    return false;
}

void
CriticalPathHeuristicNeighborsRefinement::store_current_states_fact_relation(
    state_component::StateComponent& states)
{
    state_component_size_ = 0;
    inverse_fact_to_state_ =
        strips_utils::FactIndexedVector<std::vector<unsigned>>(task_->num_facts());
    states.reset();
    while (!states.end()) {
        strips_utils::FactSet facts = task_->get_fact_set(states.current());
        // std::cout << "component[" << state_component_size_ << " (" <<
        // states.current().get_id() << ")"
        //           << "] = " << task_->to_string(facts) << std::endl;
        auto it = std::begin(facts);
        for (auto p = task_->fact_begin(); p != task_->fact_end(); ++p) {
            if (it == std::end(facts) || (*it) != (*p)) {
                inverse_fact_to_state_[*p].push_back(state_component_size_);
            } else {
                it++;
            }
        }
        states.next();
        state_component_size_++;
    }
}

void
CriticalPathHeuristicNeighborsRefinement::
    store_neighbors_unreachable_conjunctions(
        state_component::StateComponent& neighbors)
{
    conjunction_to_neighbor_ =
        std::vector<std::vector<unsigned>>(hc_->num_conjunctions());
    num_neighbors_ = 0;
    neighbors.reset();
    while (!neighbors.end()) {
        hc_->evaluate(neighbors.current());
        assert(hc_->is_dead_end());
        // std::cout << "neighbor = " <<
        // task_->to_string(task_->get_fact_set(neighbors.current())) <<
        // std::endl;
        for (unsigned conj = 0; conj < hc_->num_conjunctions(); conj++) {
            if (!hc_->is_conjunction_reachable(conj)) {
                // std::cout << "   unreachable: " <<
                // task_->to_string(hc_->get_conjunction_fact_set(conj)) <<
                // std::endl;
                conjunction_to_neighbor_[conj].push_back(num_neighbors_);
            }
        }
        neighbors.next();
        num_neighbors_++;
    }
}

void
CriticalPathHeuristicNeighborsRefinement::synchronize_reachability()
{
    for (int counter_id = hc_->num_counters() - 1; counter_id >= 0;
         counter_id--) {
        CounterInfo* counter = hc_->get_counter_info(counter_id);
        counter->unsat = 0;
    }
    for (int conj_id = hc_->num_conjunctions() - 1; conj_id >= 0; conj_id--) {
        ConjunctionInfo* conj = hc_->get_conjunction_info(conj_id);
        conj->cost = 0;
        if (!current_states_reachability_[conj_id]) {
            conj->cost = ConjunctionInfo::UNACHIEVED;
            for (int i = conj->in_regression_of.size() - 1; i >= 0; i--) {
                conj->in_regression_of[i]->unsat++;
            }
        }
    }
}

bool
CriticalPathHeuristicNeighborsRefinement::push_for_refinement(
    const strips_utils::FactSet& subgoal)
{
#ifndef NDEBUG
    {
        std::vector<unsigned> conjs =
            hc_->get_conjunction_fact_sets().get_subsets(subgoal);
        for (int i = conjs.size() - 1; i >= 0; i--) {
            assert(current_states_reachability_[conjs[i]]);
        }
    }
#endif

    auto merge_into = [](const auto& facts, strips_utils::FactSet& conflict) {
        unsigned size = conflict.size();
        conflict.reserve(conflict.size() + facts.size());
        std::set_difference(
            facts.begin(),
            facts.end(),
            conflict.begin(),
            conflict.end(),
            std::back_inserter(conflict));
        std::inplace_merge(
            conflict.begin(), conflict.begin() + size, conflict.end());
    };

    strips_utils::FactSet conflict;

    {
        std::vector<unsigned> neighbor_cover = get_neighbor_covering(subgoal);
        for (unsigned i = 0; i < neighbor_cover.size(); i++) {
            const strips_utils::FactSet& conjunction =
                hc_->get_conjunction_fact_set(neighbor_cover[i]);
            merge_into(conjunction, conflict);
        }
    }

    {
        std::vector<strips_utils::Fact> facts =
            get_inverse_state_covering(subgoal, conflict);
        std::sort(facts.begin(), facts.end());
        merge_into(facts, conflict);
    }

#if !defined(NDEBUG)
    assert(std::is_sorted(conflict.begin(), conflict.end()));
    assert(std::unique(conflict.begin(), conflict.end()) == conflict.end());
    assert(std::includes(
        subgoal.begin(), subgoal.end(), conflict.begin(), conflict.end()));
    std::vector<bool> a(num_neighbors_, false);
    std::vector<bool> b(state_component_size_, false);
    unsigned na = num_neighbors_;
    unsigned nb = state_component_size_;
    hc_->get_conjunction_fact_sets().apply_to_subsets(
        conflict, [&](unsigned id) {
            for (int i = conjunction_to_neighbor_[id].size() - 1; i >= 0; i--) {
                if (!a[conjunction_to_neighbor_[id][i]]) {
                    a[conjunction_to_neighbor_[id][i]] = true;
                    na--;
                }
            }
            const strips_utils::FactSet& facts = hc_->get_conjunction_fact_set(id);
            for (const auto& p : facts) {
                for (const auto& s : inverse_fact_to_state_[p]) {
                    if (!b[s]) {
                        b[s] = true;
                        nb--;
                    }
                }
            }
        });
    assert(na == 0);
    assert(nb == 0);

#endif

    int last_num_counters = hc_->num_counters();
    std::pair<unsigned, bool> conj_id = hc_->add_conjunction(conflict);

    refinement_queue_.emplace_back(hc_->get_conjunction_info(conj_id.first));

    // std::cout << "conflict(" << task_->to_string(subgoal)
    //           << ") = " << task_->to_string(conflict)
    //           << " [is_new=" << conj_id.second << "]"
    //           << std::endl;

    if (conj_id.second) {
        conjunction_to_neighbor_.emplace_back();
        current_states_reachability_.push_back(true);
        for (int i = hc_->num_counters() - 1; i >= last_num_counters; i--) {
            CounterInfo* counter = hc_->get_counter_info(i);
            counter->unsat = 0;
            for (int j = counter->regression_conjunction_ids.size() - 1; j >= 0;
                 --j) {
                if (!current_states_reachability_
                        [counter->regression_conjunction_ids[j]]) {
                    counter->unsat++;
                }
            }
        }
#if !defined(NDEBUG)
        // std::cout << "added new conjunction " << task_->to_string(conflict)
        // << std::endl;
#endif
    }

#if 1
    hc_->get_conjunction_fact_sets().apply_to_supersets(
        conflict, [this](unsigned id) {
            if (current_states_reachability_[id]) {
                current_states_reachability_[id] = false;
                ConjunctionInfo* conjunction = hc_->get_conjunction_info(id);
                assert(
                    id == hc_->num_conjunctions() - 1
                    || conjunction->cost != ConjunctionInfo::UNACHIEVED);
                conjunction->cost = ConjunctionInfo::UNACHIEVED;
                for (int i = conjunction->in_regression_of.size() - 1; i >= 0;
                     i--) {
                    conjunction->in_regression_of[i]->unsat++;
                }
            }
        });
#else
    current_states_reachability_[conj_id.first] = false;
#endif

    assert(!current_states_reachability_[conj_id.first]);

    return !is_size_limit_reached();
}

bool
CriticalPathHeuristicNeighborsRefinement::process()
{
    RefinementNode& node = refinement_queue_.back();
    // std::cout << "processing " <<
    // task_->to_string(hc_->get_conjunction_fact_set(node.conjunction->id)) <<
    // " @index=" << node.achiever_index << std::endl;
    while (node.achiever_index >= 0) {
        const CounterInfo* achiever =
            node.conjunction->achievers[node.achiever_index].first;
        node.achiever_index--;
#if 1
        if (achiever->unsat) {
            continue;
        }
#else
        bool is_sat = true;
        hc_->get_conjunction_fact_sets().apply_to_subsets(
            achiever->regression, [this, &is_sat](unsigned id) {
                if ( // id >= last_num_conjunctions_
                     //|| (id < last_num_conjunctions_
                     //&&
                    (!current_states_reachability_[id])) {
                    is_sat = false;
                }
            });
        // for (int i = hc_->num_conjunctions() - 1; i >=
        // static_cast<int>(last_num_conjunctions_); i--) {
        //     const FactSet& fact_set = hc_->get_conjunction_fact_set(i);
        //     if (std::includes(achiever->regression.begin(),
        //     achiever->regression.end(), fact_set.begin(),
        //     fact_set.end())) {
        //         is_sat = false;
        //         break;
        //     }
        // }
        // for (int i = achiever->regression_conjunction_ids.size() - 1; i
        // >= 0; i--) {
        //     if
        //     (!current_states_reachability_[achiever->regression_conjunction_ids[i]])
        //     {
        //         is_sat = false;
        //         break;
        //     }
        // }
        // int op = task_
        //              ->get_actions()[std::find_if(
        //                                  achiever->achieving.begin(),
        //                                  achiever->achieving.end(),
        //                                  [node](const auto& x) {
        //                                      return x.first ==
        //                                      node.conjunction;
        //                                  })
        //                                  ->second]
        //              .original_operator;
        // std::cout << "processing "
        //           << task_->to_string(
        //                  hc_->get_conjunction_fact_set(node.conjunction->id))
        //           << ", op=" << g_operators[op].get_name()
        //           << " ====> " << is_sat << std::endl;
        if (!is_sat) { // achiever->unsat > 0) {
            continue;
        }
#endif
        return push_for_refinement(achiever->regression);
    }
    // hc_->get_conjunction_fact_sets().apply_to_supersets(
    //     hc_->get_conjunction_fact_set(node.conjunction->id),
    //     [this](unsigned conj_id) {
    //         if (current_states_reachability_[conj_id]) {
    //             current_states_reachability_[conj_id] = false;
    //             ConjunctionInfo* conj =
    //             hc_->get_conjunction_info(conj_id); for (int i =
    //             conj->in_regression_of.size() - 1; i >= 0;
    //                  --i) {
    //                 conj->in_regression_of[i]->unsat++;
    //             }
    //         }
    //     });
    refinement_queue_.pop_back();
    return true;
}

template<typename Value, typename Key, typename ValueToIndexVector>
static std::vector<Value>
select_covering_greedy(
    std::vector<Key>& ordered_choices,
    std::vector<bool>& covered,
    int left,
    const ValueToIndexVector& choice_to_index)
{
    assert(left > 0);
    std::vector<Value> cover;
    while (true) {
        assert(!ordered_choices.empty());
        Key* min_key = &ordered_choices.back();
        for (int i = ordered_choices.size() - 1; i >= 1; i--) {
            assert(ordered_choices[i].covers > 0);
            if (ordered_choices[i] < *min_key) {
                min_key = &ordered_choices[i];
            }
        }
        cover.push_back(min_key->value);
#ifndef NDEBUG
        int old_left = left;
#endif
        {
            const auto& covers = choice_to_index[min_key->value];
            for (int i = covers.size() - 1; i >= 0; --i) {
                if (!covered[covers[i]]) {
                    --left;
                    covered[covers[i]] = true;
                }
            }
        }
#ifndef NDEBUG
        assert(left < old_left);
#endif
        if (left == 0) {
            break;
        }
        {
            unsigned new_i = 0;
            for (unsigned i = 0; i < ordered_choices.size(); i++) {
                Key& key = ordered_choices[i];
                key.covers = 0;
                {
                    const auto& covers = choice_to_index[key.value];
                    for (int j = covers.size() - 1; j >= 0; j--) {
                        key.covers += !covered[covers[j]];
                    }
                }
                if (key.covers > 0) {
                    ordered_choices[new_i] = std::move(ordered_choices[i]);
                    new_i++;
                }
            }
            while (ordered_choices.size() > new_i) {
                ordered_choices.pop_back();
            }
        }
    }
    return cover;
}

std::vector<unsigned>
CriticalPathHeuristicNeighborsRefinement::get_neighbor_covering(
    const strips_utils::FactSet& subgoal)
{
    int neighbors = num_neighbors_;
    if (neighbors == 0) {
        return {};
    }
    struct Key {
        unsigned covers;
        unsigned size;
        unsigned value;
        Key(unsigned id, unsigned covers, unsigned size)
            : covers(covers)
            , size(size)
            , value(id)
        {
        }
        Key(Key&&) = default;
        Key& operator=(Key&&) = default;
        bool operator<(const Key& other) const
        {
            return covers > other.covers
                || (covers == other.covers && size < other.size);
        }
    };
    std::vector<unsigned> choices =
        hc_->get_conjunction_fact_sets().get_subsets(subgoal);
    std::vector<Key> sorted_choices;
    sorted_choices.reserve(choices.size());
    for (int i = choices.size() - 1; i >= 0; i--) {
        const unsigned conj = choices[i];
        const unsigned covers = conjunction_to_neighbor_[conj].size();
        if (covers > 0) {
            sorted_choices.emplace_back(
                conj, covers, hc_->get_conjunction_fact_set(conj).size());
        }
    }
    std::vector<bool> covered(neighbors, false);
    return select_covering_greedy<unsigned>(
        sorted_choices, covered, neighbors, conjunction_to_neighbor_);
}

std::vector<strips_utils::Fact>
CriticalPathHeuristicNeighborsRefinement::get_inverse_state_covering(
    const strips_utils::FactSet& subgoal,
    const strips_utils::FactSet& conflict)
{
    int left = state_component_size_;
    std::vector<bool> covered(state_component_size_, false);
    {
        for (auto conflict_it = std::begin(conflict);
             conflict_it != std::end(conflict);
             conflict_it++) {
            const std::vector<unsigned>& states =
                inverse_fact_to_state_[*conflict_it];
            for (int i = states.size() - 1; i >= 0; --i) {
                if (!covered[states[i]]) {
                    covered[states[i]] = true;
                    --left;
                }
            }
        }
        if (left == 0) {
            return {};
        }
    }
    struct Key {
        strips_utils::Fact value;
        unsigned covers;
        Key(const strips_utils::Fact& p)
            : value(p)
            , covers(0)
        {
        }
        Key(Key&&) = default;
        Key& operator=(Key&&) = default;
        bool operator<(const Key& other) const
        {
            return covers > other.covers
                || (covers == other.covers && value < other.value);
        }
    };
    std::vector<Key> ordered_facts;
    for (auto sg_it = std::begin(subgoal); sg_it != std::end(subgoal);
         ++sg_it) {
        Key key(*sg_it);
        const std::vector<unsigned>& states = inverse_fact_to_state_[*sg_it];
        for (int i = states.size() - 1; i >= 0; --i) {
            if (!covered[states[i]]) {
                key.covers++;
            }
        }
        if (key.covers > 0) {
            ordered_facts.push_back(std::move(key));
        }
    }
    return select_covering_greedy<strips_utils::Fact>(
        ordered_facts, covered, left, inverse_fact_to_state_);
}

static Plugin<state_component::StateComponentListener> _plugin(
    "hc_neighbors_refinement",
    options::parse<
        state_component::StateComponentListener,
        CriticalPathHeuristicNeighborsRefinement>);

} // namespace critical_path_heuristic
