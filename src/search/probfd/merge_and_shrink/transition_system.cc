#include "probfd/merge_and_shrink/transition_system.h"

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/labels.h"

#include "probfd/utils/json.h"

#include "downward/utils/collections.h"
#include "downward/utils/logging.h"
#include "downward/utils/system.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <numeric>
#include <ranges>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

using namespace std;
using utils::ExitCode;

namespace probfd::merge_and_shrink {

namespace {
template <typename T, char LPAREN = '[', char RPAREN = ']'>
std::istream& operator>>(std::istream& in, std::vector<T>& list)
{
    using R = std::conditional_t<std::is_same_v<T, bool>, int, T>;

    if (in.get() != LPAREN) {
        in.setstate(std::ios::failbit);
        return in;
    }

    if ((in >> std::ws, in.peek() != RPAREN)) {
        {
            R element;
            if (!(in >> element)) return in;

            if constexpr (std::is_same_v<T, bool>) {
                list.push_back(element != 0);
            } else {
                list.push_back(element);
            }
        }

        while ((in >> std::ws, in.peek() != RPAREN)) {
            if (in.get() != ',') {
                in.setstate(std::ios::failbit);
                return in;
            }

            R element;
            if (!(in >> std::ws >> element)) return in;

            if constexpr (std::is_same_v<T, bool>) {
                list.push_back(element != 0);
            } else {
                list.push_back(element);
            }
        }
    }

    in.get();
    return in;
}
} // namespace

LocalLabelInfo LocalLabelInfo::read_from_file(istream& is)
{
    LabelGroup label_group;
    std::vector<Transition> transitions;
    std::vector<value_t> probabilities;
    value_t cost;

    if (!(is >> label_group))
        throw std::invalid_argument("Could not read label group.");
    if (is.get() != ',') throw std::invalid_argument("Expected \",\".");
    if (!(is >> std::ws >> cost))
        throw std::invalid_argument("Could not read cost.");
    if (is.get() != ',') throw std::invalid_argument("Expected \",\".");
    if (!(is >> std::ws >> probabilities))
        throw std::invalid_argument("Could not read probabilities.");
    if (is.get() != ',') throw std::invalid_argument("Expected \",\".");
    if (!(is >> std::ws >> transitions))
        throw std::invalid_argument("Could not read transitions.");

    return LocalLabelInfo(
        std::move(label_group),
        std::move(transitions),
        std::move(probabilities),
        cost);
}

void LocalLabelInfo::add_label(int label, value_t label_cost)
{
    label_group.push_back(label);
    if (label_cost != -1) {
        cost = min(cost, label_cost);
    }
    assert(is_consistent());
}

void LocalLabelInfo::apply_same_cost_label_mapping(
    int new_label,
    const vector<int>& old_labels)
{
    assert(is_consistent());
    remove_labels(old_labels);
    label_group.push_back(new_label);
    assert(is_consistent());
}

void LocalLabelInfo::remove_labels(const vector<int>& old_labels)
{
    assert(is_consistent());
    assert(utils::is_sorted_unique(old_labels));
    auto it = set_difference(
        label_group.begin(),
        label_group.end(),
        old_labels.begin(),
        old_labels.end(),
        label_group.begin());
    label_group.erase(it, label_group.end());
    assert(is_consistent());
}

void LocalLabelInfo::recompute_cost(const Labels& labels)
{
    cost = INFINITE_VALUE;
    for (int label : label_group) {
        value_t label_cost = labels.get_label_cost(label);
        cost = min(cost, label_cost);
    }
}

void LocalLabelInfo::replace_transitions(vector<Transition>&& new_transitions)
{
    transitions = std::move(new_transitions);
    assert(is_consistent());
}

void LocalLabelInfo::merge_local_label_info(LocalLabelInfo& local_label_info)
{
    assert(is_consistent());
    assert(local_label_info.is_consistent());
    assert(transitions == local_label_info.transitions);
    label_group.insert(
        label_group.end(),
        make_move_iterator(local_label_info.label_group.begin()),
        make_move_iterator(local_label_info.label_group.end()));
    cost = min(cost, local_label_info.cost);
    local_label_info.deactivate();
    sort(label_group.begin(), label_group.end());
    assert(is_consistent());
}

void LocalLabelInfo::deactivate()
{
    utils::release_vector_memory(transitions);
    utils::release_vector_memory(label_group);
    cost = -1;
}

bool LocalLabelInfo::is_consistent() const
{
    return utils::is_sorted_unique(label_group) &&
           utils::is_sorted_unique(transitions);
}

std::ostream& operator<<(std::ostream& out, const LocalLabelInfo& label_info)
{
    return out << "Labels: " << label_info.get_label_group()
               << ", Cost: " << label_info.get_cost()
               << ", Probabilities: " << label_info.get_probabilities()
               << ", Transitions: " << label_info.get_transitions();
}

void dump_json(std::ostream& os, const LocalLabelInfo& info)
{
    json::write_object(
        os,
        std::forward_as_tuple("labels", info.label_group),
        std::forward_as_tuple("transitions", info.transitions),
        std::forward_as_tuple("probabilities", info.probabilities),
        std::forward_as_tuple("cost", info.cost));
}

LocalLabelInfo LocalLabelInfo::read_json(std::istream& is)
{
    return json::construct_from_object<
        LocalLabelInfo,
        LabelGroup,
        std::vector<Transition>,
        std::vector<value_t>,
        value_t>(is, "labels", "transitions", "probabilities", "cost");
}

std::partial_ordering
compare_transitions(const LocalLabelInfo& left, const LocalLabelInfo& right)
{
    return std::tie(left.probabilities, left.transitions) <=>
           std::tie(right.probabilities, right.transitions);
}

void LocalLabelInfo::merge(LocalLabelInfo& right)
{
    cost = std::min(cost, right.cost);
    label_group.push_back(right.label_group.back());
    right.label_group.clear();
}

/*
  Implementation note: Transitions are grouped by their label groups,
  not by source state or any such thing. Such a grouping is beneficial
  for fast generation of products because we can iterate label group
  by label group, and it also allows applying transition system
  mappings very efficiently.

  We rarely need to be able to efficiently query the successors of a
  given state; actually, only the distance computation requires that,
  and it simply generates such a graph representation of the
  transitions itself. Various experiments have shown that maintaining
  a graph representation permanently for the benefit of distance
  computation is not worth the overhead.
*/

TransitionSystem::TransitionSystem(
    vector<int> incorporated_variables,
    vector<int> label_to_local_label,
    vector<LocalLabelInfo> local_label_infos,
    int init_state,
    vector<bool> goal_states)
    : incorporated_variables(std::move(incorporated_variables))
    , label_to_local_label(std::move(label_to_local_label))
    , local_label_infos(std::move(local_label_infos))
    , init_state(init_state)
    , goal_states(std::move(goal_states))
{
}

unique_ptr<TransitionSystem> TransitionSystem::merge(
    const Labels& labels,
    const TransitionSystem& ts1,
    const TransitionSystem& ts2,
    utils::LogProxy& log)
{
    if (log.is_at_least_verbose()) {
        log << "Merging transition system with variables "
            << ts1.get_incorporated_variables()
            << " and transition system with variables "
            << ts2.get_incorporated_variables() << endl;
    }

    assert(ts1.init_state != PRUNED_STATE && ts2.init_state != PRUNED_STATE);
    assert(ts1.is_valid(labels) && ts2.is_valid(labels));

    vector<int> incorporated_variables;
    std::ranges::set_union(
        ts1.incorporated_variables,
        ts2.incorporated_variables,
        std::back_inserter(incorporated_variables));
    vector<int> label_to_local_label(labels.get_max_num_labels(), -1);
    vector<LocalLabelInfo> local_label_infos;

    const int ts1_size = ts1.get_size();
    const int ts2_size = ts2.get_size();

    // Compute merged initial state
    const int init_state = ts1.init_state + ts2.init_state * ts1_size;

    // Compute merged goal states
    vector<bool> goal_states(ts1_size * ts2_size, false);
    for (int s1 = 0; s1 < ts1_size; ++s1) {
        if (!ts1.goal_states[s1]) continue;
        for (int s2 = 0; s2 < ts2_size; ++s2) {
            if (ts2.goal_states[s2]) {
                const int state = s1 + s2 * ts1_size;
                goal_states[state] = true;
            }
        }
    }

    /*
      We can compute the local equivalence relation of a composite T
      from the local equivalence relations of the two components T1 and T2:
      l and l' are locally equivalent in T iff:
      (A) they are locally equivalent in T1 and in T2, or
      (B) they are both dead in T (e.g., this includes the case where
          l is dead in T1 only and l' is dead in T2 only, so they are not
          locally equivalent in either of the components).
    */
    vector<int> dead_labels;
    for (const LocalLabelInfo& local_label_info : ts1.label_infos()) {
        const LabelGroup& group1 = local_label_info.get_label_group();
        const vector<Transition>& transitions1 =
            local_label_info.get_transitions();
        const vector<value_t>& probabilities1 =
            local_label_info.get_probabilities();

        // Distribute the labels of this group among the "buckets"
        // corresponding to the groups of ts2.
        map<int, vector<int>> buckets;
        for (int label : group1) {
            int ts_local_label2 = ts2.label_to_local_label[label];
            buckets[ts_local_label2].push_back(label);
        }
        // Now buckets contains all equivalence classes that are
        // refinements of group1.

        // Now create the new groups together with their transitions.
        for (auto& [local_label2, new_labels] : buckets) {
            const auto& transitions2 =
                ts2.local_label_infos[local_label2].get_transitions();
            assert(
                probabilities1 ==
                ts2.local_label_infos[local_label2].get_probabilities());

            if (transitions1.empty() || transitions2.empty()) {
                dead_labels.insert(
                    dead_labels.end(),
                    new_labels.begin(),
                    new_labels.end());
                continue;
            }

            // Create the new transitions for this bucket
            vector<Transition> new_transitions;
            if (transitions1.size() >
                new_transitions.max_size() / transitions2.size())
                utils::exit_with(ExitCode::SEARCH_OUT_OF_MEMORY);

            new_transitions.reserve(transitions1.size() * transitions2.size());
            for (const auto& [src1, targets1] : transitions1) {
                for (const auto& [src2, targets2] : transitions2) {
                    assert(targets1.size() == targets2.size());

                    const int src = src1 + src2 * ts1_size;
                    std::vector<int> targets;
                    for (const auto [t1, t2] : views::zip(targets1, targets2)) {
                        targets.push_back(t1 + t2 * ts1_size);
                    }
                    new_transitions.emplace_back(src, std::move(targets));
                }
            }

            // Create a new group
            sort(new_transitions.begin(), new_transitions.end());
            sort(new_labels.begin(), new_labels.end());

            int new_local_label = local_label_infos.size();
            value_t cost = INFINITE_VALUE;
            for (int label : new_labels) {
                cost = min(labels.get_label_cost(label), cost);
                label_to_local_label[label] = new_local_label;
            }

            local_label_infos.emplace_back(
                std::move(new_labels),
                std::move(new_transitions),
                probabilities1,
                cost);
        }
    }

    /*
      We collect all dead labels separately, because the bucket refining
      does not work in cases where there are at least two dead labels l1
      and l2 in the composite, where l1 was only a dead label in the first
      component and l2 was only a dead label in the second component.
      All dead labels should form one single label group.
    */
    if (!dead_labels.empty()) {
        sort(dead_labels.begin(), dead_labels.end());

        int new_local_label = local_label_infos.size();
        value_t cost = INFINITE_VALUE;
        for (int label : dead_labels) {
            cost = min(cost, labels.get_label_cost(label));
            label_to_local_label[label] = new_local_label;
        }
        // Dead labels have empty transitions
        local_label_infos.emplace_back(
            std::move(dead_labels),
            vector<Transition>(),
            vector<value_t>(),
            cost);
    }

    return std::make_unique<TransitionSystem>(
        std::move(incorporated_variables),
        std::move(label_to_local_label),
        std::move(local_label_infos),
        init_state,
        std::move(goal_states));
}

void TransitionSystem::compute_equivalent_local_labels()
{
    /*
      Compare every group of labels and their transitions to all others and
      merge two groups whenever the transitions are the same.

      Note that there can be empty local label groups after applying label
      reduction when combining labels which are combinable for this transition
      system.
    */
    int num_local_labels = local_label_infos.size();
    for (int llabel1 = 0; llabel1 < num_local_labels; ++llabel1) {
        auto& local_label_info1 = local_label_infos[llabel1];
        if (!local_label_info1.is_active()) continue;

        const auto& transitions1 = local_label_info1.get_transitions();
        const auto& probabilities1 = local_label_info1.get_probabilities();

        for (int llabel2 = llabel1 + 1; llabel2 < num_local_labels; ++llabel2) {
            auto& local_label_info2 = local_label_infos[llabel2];
            if (!local_label_info2.is_active()) continue;

            const auto& transitions2 = local_label_info2.get_transitions();
            const auto& probabilities2 = local_label_info2.get_probabilities();

            // Comparing transitions directly works because they are
            // sorted and unique.
            if (probabilities1 == probabilities2 &&
                transitions1 == transitions2) {
                for (int label : local_label_info2.get_label_group()) {
                    label_to_local_label[label] = llabel1;
                }
                local_label_info1.merge_local_label_info(local_label_info2);
            }
        }
    }
}

void TransitionSystem::apply_abstraction(
    const StateEquivalenceRelation& state_equivalence_relation,
    const vector<int>& abstraction_mapping,
    utils::LogProxy& log)
{
    int new_num_states = state_equivalence_relation.size();
    assert(new_num_states <= get_size());
    if (log.is_at_least_verbose()) {
        log << tag() << "applying abstraction (" << get_size() << " to "
            << new_num_states << " states)" << endl;
    }

    // Compute abstract initial state
    init_state = abstraction_mapping[init_state];
    if (log.is_at_least_verbose() && init_state == PRUNED_STATE) {
        log << tag() << "initial state pruned; task unsolvable" << endl;
    }

    // Compute abstract goal states
    std::vector<bool> new_goal_states(new_num_states);

    for (int new_state = 0; new_state < new_num_states; ++new_state) {
        const auto& state_eqv_class = state_equivalence_relation[new_state];
        assert(!state_eqv_class.empty());

        auto is_goal = [&](int old_state) { return goal_states[old_state]; };
        new_goal_states[new_state] =
            std::ranges::any_of(state_eqv_class, is_goal);
    }

    goal_states = std::move(new_goal_states);

    // Update all transitions.
    for (LocalLabelInfo& local_label_info : local_label_infos) {
        auto& transitions = local_label_info.get_transitions();

        /*
          Modify the existing transitions inplace, removing those for which
          a state is pruned.
        */
        std::erase_if(transitions, [&abstraction_mapping](Transition& t) {
            auto& [src, targets] = t;

            // Modify source state
            src = abstraction_mapping[src];
            if (src == PRUNED_STATE) return true;

            // Modify target states
            for (int& target : targets) {
                target = abstraction_mapping[target];
                if (target == PRUNED_STATE) return true;
            }

            return false;
        });

        utils::sort_unique(transitions);
        assert(local_label_info.is_consistent());
    }

    compute_equivalent_local_labels();
}

void TransitionSystem::apply_label_reduction(
    const Labels& labels,
    const vector<pair<int, vector<int>>>& label_mapping,
    bool only_equivalent_labels)
{
    /*
      We iterate over the given label mapping, treating every new label and
      the reduced old labels separately. We further distinguish the case
      where we know that the reduced labels are all from the same equivalence
      class from the case where we may combine arbitrary labels.

      The case where only equivalent labels are combined is simple: remove all
      old labels from the label group and add the new one.

      The other case is more involved: again remove all old labels from their
      groups, and the groups themselves if they become empty. Also collect
      the transitions of all reduced labels. Add a new group for every new
      label and assign the collected transitions to this group. Recompute the
      cost of all groups and compute locally equivalent labels.

      NOTE: Previously, this latter case was computed in a more incremental
      fashion: Rather than recomputing cost of all groups, we only recomputed
      cost for groups from which we actually removed labels (hence temporarily
      storing these affected groups). Furthermore, rather than computing
      locally equivalent labels from scratch, we did not per default add a new
      group for every label, but checked for an existing equivalent label
      group. In issue539, it turned out that this incremental fashion of
      computation does not accelerate the computation.
    */

    if (only_equivalent_labels) {
        // Update both label mappings.
        for (const auto& [new_label, old_labels] : label_mapping) {
            assert(old_labels.size() >= 2);
            int local_label = label_to_local_label[old_labels.front()];
            local_label_infos[local_label].apply_same_cost_label_mapping(
                new_label,
                old_labels);

            label_to_local_label[new_label] = local_label;
            for (int old_label : old_labels) {
                assert(label_to_local_label[old_label] == local_label);
                // Reset (for consistency only, old labels are never accessed).
                label_to_local_label[old_label] = -1;
            }
        }
    } else {
        /*
          Iterate over the label mapping. For each new label, go over the
          reduced labels to combine their transitions into the transitions
          of the new label. Also store, for each local label, the labels
          removed from them. Add the new label together with its transitions
          as a new local label and update the label_to_local_label mapping.
        */
        unordered_map<int, vector<int>> local_label_to_old_labels;
        for (const auto& [new_label, old_labels] : label_mapping) {
            assert(old_labels.size() >= 2);

            unordered_set<int> seen_local_labels;
            std::vector<Transition> new_label_transitions;

            std::vector<value_t> probabilities =
                local_label_infos[label_to_local_label[old_labels.front()]]
                    .get_probabilities();

            for (int old_label : old_labels) {
                int old_local_label = label_to_local_label[old_label];

                if (seen_local_labels.insert(old_local_label).second) {
                    auto& local_info = local_label_infos[old_local_label];
                    const auto& transitions = local_info.get_transitions();
                    assert(probabilities == local_info.get_probabilities());

                    new_label_transitions.insert(
                        new_label_transitions.end(),
                        transitions.begin(),
                        transitions.end());
                }
                local_label_to_old_labels[old_local_label].push_back(old_label);
                // Reset (for consistency only, old labels are never accessed).
                label_to_local_label[old_label] = -1;
            }

            utils::sort_unique(new_label_transitions);

            int new_local_label = local_label_infos.size();
            label_to_local_label[new_label] = new_local_label;
            value_t new_cost = labels.get_label_cost(new_label);

            local_label_infos.emplace_back(
                std::vector<int>{new_label},
                std::move(new_label_transitions),
                std::move(probabilities),
                new_cost);
        }

        /*
          Remove all labels of all affected local labels and recompute the
          cost of these affected local labels.
        */
        for (auto& [local_label, old_labels] : local_label_to_old_labels) {
            ranges::sort(old_labels);
            local_label_infos[local_label].remove_labels(old_labels);
            local_label_infos[local_label].recompute_cost(labels);
        }

        compute_equivalent_local_labels();
    }

    assert(is_valid(labels));
}

bool TransitionSystem::are_local_labels_consistent() const
{
    return std::ranges::all_of(label_infos(), &LocalLabelInfo::is_consistent);
}

bool TransitionSystem::is_valid(const Labels& labels) const
{
    return are_local_labels_consistent() && is_label_mapping_consistent(labels);
}

bool TransitionSystem::is_label_mapping_consistent(const Labels& labels) const
{
    for (int label : labels.get_active_labels() | std::views::keys) {
        int local_label = label_to_local_label[label];
        const LabelGroup& label_group =
            local_label_infos[local_label].get_label_group();
        assert(!label_group.empty());

        if (!std::binary_search(
                label_group.begin(),
                label_group.end(),
                label)) {
            dump_label_mapping(labels, cerr);
            cerr << "label " << label
                 << " is not part of the "
                    "local label it is mapped to"
                 << endl;
            return false;
        }
    }

    for (size_t local_label = 0; local_label < local_label_infos.size();
         ++local_label) {
        const LocalLabelInfo& local_label_info = local_label_infos[local_label];
        for (int label : local_label_info.get_label_group()) {
            if (label_to_local_label[label] != static_cast<int>(local_label)) {
                dump_label_mapping(labels, cerr);
                cerr << "label " << label
                     << " is not mapped "
                        "to the local label it is part of"
                     << endl;
                return false;
            }
        }
    }
    return true;
}

void TransitionSystem::dump_label_mapping(
    const Labels& labels,
    std::ostream& out) const
{
    out << "to local label mapping: ";
    for (int label : labels.get_active_labels() | std::views::keys) {
        out << label << " -> " << label_to_local_label[label] << ", ";
    }
    out << endl;
    out << "local to label mapping: ";
    for (size_t local_label = 0; local_label < local_label_infos.size();
         ++local_label) {
        out << local_label << ": "
            << local_label_infos[local_label].get_label_group() << ", ";
    }
    out << endl;
}

bool TransitionSystem::is_solvable(const Distances& distances) const
{
    return init_state != PRUNED_STATE &&
           (!distances.are_goal_distances_computed() ||
            distances.get_goal_distance(init_state) != INFINITE_VALUE);
}

int TransitionSystem::compute_total_transitions() const
{
    int total = 0;
    for (const LocalLabelInfo& local_label_info : label_infos()) {
        total += local_label_info.get_num_transitions();
    }
    return total;
}

string TransitionSystem::tag() const
{
    std::stringstream ss;
    ss << "Transition system with variables " << incorporated_variables << ": ";
    return ss.str();
}

void TransitionSystem::dump_statistics(utils::LogProxy& log) const
{
    if (log.is_at_least_verbose()) {
        log << tag() << get_size() << " states, " << compute_total_transitions()
            << " arcs" << endl;
    }
}

void TransitionSystem::dump_dot_graph(utils::LogProxy& log) const
{
    if (log.is_at_least_debug()) {
        log << "digraph transition_system";
        for (int var : incorporated_variables) log << "_" << var;
        log << " {" << endl;
        log << "    node [shape = none] start;" << endl;
        for (int i = 0; i < get_size(); ++i) {
            log << "    node [shape = "
                << (goal_states[i] ? "doublecircle" : "circle") << "] node" << i
                << ";" << endl;
            if (i == init_state) log << "    start -> node" << i << ";" << endl;
        }

        // Introduce intermediate nodes for every transition
        size_t k = 0;
        for (const LocalLabelInfo& local_label_info : label_infos()) {
            const vector<Transition>& transitions =
                local_label_info.get_transitions();
            for (size_t i = 0; i != transitions.size(); ++i) {
                log << "    node [shape = "
                    << "diamond] interm" << k++ << ";" << endl;
            }
        }

        k = 0;
        for (const LocalLabelInfo& local_label_info : label_infos()) {
            const LabelGroup& label_group = local_label_info.get_label_group();
            const vector<Transition>& transitions =
                local_label_info.get_transitions();
            for (const auto& [src, targets] : transitions) {
                // Connect source to intermediate node
                log << "    node" << src << " -> interm" << k << " [label = ";
                for (auto label_it = label_group.begin();
                     label_it != label_group.end();
                     ++label_it) {
                    if (label_it != label_group.begin()) log << "_";
                    log << "x" << *label_it;
                }
                log << "];" << endl;

                // Connect intermediate node to each target
                for (int target : targets) {
                    log << "    interm" << k << " -> node" << target << ";"
                        << endl;
                }

                ++k;
            }
        }
        log << "}" << endl;
    }
}

void TransitionSystem::dump_labels_and_transitions(utils::LogProxy& log) const
{
    if (!log.is_at_least_debug()) return;

    log << tag() << "\nTransitions:";
    for (const LocalLabelInfo& label_info : label_infos()) {
        log << label_info;
    }
    log << std::endl;
}

TransitionSystem TransitionSystem::read_from_file(std::istream& is)
{
    std::vector<int> incorporated_variables;
    std::vector<int> label_to_local_label;
    std::vector<LocalLabelInfo> local_labels;
    int init_state;
    std::vector<bool> goal_states;

    if (!(is >> incorporated_variables))
        throw std::invalid_argument("Expected incorporated variables.");

    is >> std::ws;
    if (!(is >> label_to_local_label))
        throw std::invalid_argument("Expected label to local label mapping.");

    while ((is >> std::ws).peek() == '[') {
        local_labels.emplace_back(LocalLabelInfo::read_from_file(is));
    }

    if (!(is >> std::ws >> init_state))
        throw std::invalid_argument("Expected initial state.");

    if (!(is >> std::ws >> goal_states))
        throw std::invalid_argument("Expected goal states.");

    return TransitionSystem(
        std::move(incorporated_variables),
        std::move(label_to_local_label),
        std::move(local_labels),
        init_state,
        std::move(goal_states));
}

std::ostream& operator<<(std::ostream& os, const TransitionSystem& ts)
{
    os << "Incorporated Variables: " << ts.incorporated_variables << '\n'
       << "Local label mapping: " << ts.label_to_local_label << '\n';

    auto&& label_infos = ts.label_infos();
    os << "Local Label Transitions:\n";
    for (auto&& elem : label_infos) {
        os << "  " << elem << '\n';
    }

    return os << "Number of states: " << ts.get_size() << '\n'
              << "Initial State: " << ts.init_state << '\n'
              << "Goal states: " << ts.goal_states;
}

void dump_json(std::ostream& os, const TransitionSystem& ts)
{
    json::write_object(
        os,
        std::forward_as_tuple(
            "incorporated_variables",
            ts.incorporated_variables),
        std::forward_as_tuple("label_to_local_label", ts.label_to_local_label),
        std::forward_as_tuple("local_label_infos", ts.local_label_infos),
        std::forward_as_tuple("init_state", ts.init_state),
        std::forward_as_tuple("goal_states", ts.goal_states));
}

TransitionSystem TransitionSystem::read_json(std::istream& is)
{
    return json::construct_from_object<
        TransitionSystem,
        std::vector<int>,
        LabelGroup,
        std::vector<LocalLabelInfo>,
        int,
        std::vector<bool>>(
        is,
        "incorporated_variables",
        "label_to_local_label",
        "local_label_infos",
        "init_state",
        "goal_states");
}

} // namespace probfd::merge_and_shrink