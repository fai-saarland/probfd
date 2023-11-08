#include "../../../include/probfd/merge_and_shrink/transition_system.h"

#include "../../../include/probfd/merge_and_shrink/distances.h"
#include "../../../include/probfd/merge_and_shrink/labels.h"

#include "../../../include/downward/utils/collections.h"
#include "../../../include/downward/utils/logging.h"
#include "../../../include/downward/utils/system.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <ranges>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

using namespace std;
using utils::ExitCode;

namespace probfd::merge_and_shrink {

ostream& operator<<(ostream& os, const Transition& trans)
{
    os << trans.src << "-> (" << trans.targets[0];

    for (size_t i = 1; i < trans.targets.size(); ++i) {
        os << "," << trans.targets[i];
    }
    os << ")";
    return os;
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

TransitionSystemConstIterator::TransitionSystemConstIterator(
    vector<LocalLabelInfo>::const_iterator it,
    vector<LocalLabelInfo>::const_iterator end_it)
    : it(it)
    , end_it(end_it)
{
    advance_to_next_valid_index();
}

void TransitionSystemConstIterator::advance_to_next_valid_index()
{
    while (it != end_it && !it->is_active()) {
        ++it;
    }
}

TransitionSystemConstIterator& TransitionSystemConstIterator::operator++()
{
    ++it;
    advance_to_next_valid_index();
    return *this;
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
    int num_variables,
    vector<int>&& incorporated_variables,
    const Labels& labels,
    vector<int>&& label_to_local_label,
    vector<LocalLabelInfo>&& local_label_infos,
    int num_states,
    vector<bool>&& goal_states,
    int init_state)
    : num_variables(num_variables)
    , incorporated_variables(std::move(incorporated_variables))
    , labels(std::move(labels))
    , label_to_local_label(std::move(label_to_local_label))
    , local_label_infos(std::move(local_label_infos))
    , num_states(num_states)
    , goal_states(std::move(goal_states))
    , init_state(init_state)
{
    assert(is_valid());
}

TransitionSystem::TransitionSystem(const TransitionSystem& other)
    : num_variables(other.num_variables)
    , incorporated_variables(other.incorporated_variables)
    , labels(other.labels)
    , label_to_local_label(other.label_to_local_label)
    , local_label_infos(other.local_label_infos)
    , num_states(other.num_states)
    , goal_states(other.goal_states)
    , init_state(other.init_state)
{
}

TransitionSystem::~TransitionSystem() = default;

unique_ptr<TransitionSystem> TransitionSystem::merge(
    const Labels& labels,
    const TransitionSystem& ts1,
    const TransitionSystem& ts2,
    utils::LogProxy& log)
{
    if (log.is_at_least_verbose()) {
        log << "Merging " << ts1.get_description() << " and "
            << ts2.get_description() << endl;
    }

    assert(ts1.init_state != PRUNED_STATE && ts2.init_state != PRUNED_STATE);
    assert(ts1.is_valid() && ts2.is_valid());

    int num_variables = ts1.num_variables;
    vector<int> incorporated_variables;
    std::ranges::set_union(
        ts1.incorporated_variables,
        ts2.incorporated_variables,
        std::back_inserter(incorporated_variables));
    vector<int> label_to_local_label(labels.get_max_num_labels(), -1);
    vector<LocalLabelInfo> local_label_infos;

    int ts1_size = ts1.get_size();
    int ts2_size = ts2.get_size();
    int num_states = ts1_size * ts2_size;

    // Compute merged initial state
    int init_state = ts1.init_state * ts2_size + ts2.init_state;

    // Compute merged goal states
    vector<bool> goal_states(num_states, false);
    for (int s1 = 0; s1 < ts1_size; ++s1) {
        if (!ts1.goal_states[s1]) continue;
        for (int s2 = 0; s2 < ts2_size; ++s2) {
            if (ts1.goal_states[s2]) {
                const int state = s1 * ts2_size + s2;
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
    int multiplier = ts2_size;
    vector<int> dead_labels;
    for (const LocalLabelInfo& local_label_info : ts1) {
        const LabelGroup& group1 = local_label_info.get_label_group();
        const vector<Transition>& transitions1 =
            local_label_info.get_transitions();

        // Distribute the labels of this group among the "buckets"
        // corresponding to the groups of ts2.
        unordered_map<int, vector<int>> buckets;
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

                    const int src = src1 * multiplier + src2;
                    std::vector<int> targets;
                    for (const auto [t1, t2] : views::zip(targets1, targets2)) {
                        targets.push_back(t1 * multiplier + t2);
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
                cost = min(ts1.labels.get_label_cost(label), cost);
                label_to_local_label[label] = new_local_label;
            }

            local_label_infos.emplace_back(
                std::move(new_labels),
                std::move(new_transitions),
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
            cost = min(cost, ts1.labels.get_label_cost(label));
            label_to_local_label[label] = new_local_label;
        }
        // Dead labels have empty transitions
        local_label_infos.emplace_back(
            std::move(dead_labels),
            vector<Transition>(),
            cost);
    }

    return std::make_unique<TransitionSystem>(
        num_variables,
        std::move(incorporated_variables),
        ts1.labels,
        std::move(label_to_local_label),
        std::move(local_label_infos),
        num_states,
        std::move(goal_states),
        init_state);
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

        for (int llabel2 = llabel1 + 1; llabel2 < num_local_labels; ++llabel2) {
            auto& local_label_info2 = local_label_infos[llabel2];
            if (!local_label_info2.is_active()) continue;

            const auto& transitions2 = local_label_info2.get_transitions();

            // Comparing transitions directly works because they are
            // sorted and unique.
            if (transitions1 == transitions2) {
                for (int label : local_label_info2.get_label_group()) {
                    label_to_local_label[label] = llabel1;
                }
                local_label_info1.merge_local_label_info(local_label_info2);
            }
        }
    }

    assert(is_valid());
}

void TransitionSystem::apply_abstraction(
    const StateEquivalenceRelation& state_equivalence_relation,
    const vector<int>& abstraction_mapping,
    utils::LogProxy& log)
{
    assert(is_valid());

    int new_num_states = state_equivalence_relation.size();
    assert(new_num_states < num_states);
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
    goal_states.erase(goal_states.begin() + new_num_states, goal_states.end());

    for (int new_state = 0; new_state < new_num_states; ++new_state) {
        const auto& state_eqv_class = state_equivalence_relation[new_state];
        assert(!state_eqv_class.empty());

        auto is_goal = [&](int old_state) { return goal_states[old_state]; };
        goal_states[new_state] = std::ranges::any_of(state_eqv_class, is_goal);
    }

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

    num_states = new_num_states;

    assert(is_valid());
}

void TransitionSystem::apply_label_reduction(
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
        for (const pair<int, vector<int>>& mapping : label_mapping) {
            int new_label = mapping.first;
            const vector<int>& old_labels = mapping.second;
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
        for (const pair<int, vector<int>>& mapping : label_mapping) {
            const vector<int>& old_labels = mapping.second;
            assert(old_labels.size() >= 2);
            unordered_set<int> seen_local_labels;
            vector<Transition> new_label_transitions;
            for (int old_label : old_labels) {
                int old_local_label = label_to_local_label[old_label];
                if (seen_local_labels.insert(old_local_label).second) {
                    const vector<Transition>& transitions =
                        local_label_infos[old_local_label].get_transitions();
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

            int new_label = mapping.first;
            int new_local_label = local_label_infos.size();
            label_to_local_label[new_label] = new_local_label;
            value_t new_cost = labels.get_label_cost(new_label);

            LabelGroup new_label_group = {new_label};
            local_label_infos.emplace_back(
                std::move(new_label_group),
                std::move(new_label_transitions),
                new_cost);
        }

        /*
          Remove all labels of all affected local labels and recompute the
          cost of these affected local labels.
        */
        for (auto& entry : local_label_to_old_labels) {
            sort(entry.second.begin(), entry.second.end());
            local_label_infos[entry.first].remove_labels(entry.second);
            local_label_infos[entry.first].recompute_cost(labels);
        }

        compute_equivalent_local_labels();
    }

    assert(is_valid());
}

string TransitionSystem::tag() const
{
    string desc(get_description());
    desc[0] = toupper(desc[0]);
    return desc + ": ";
}

bool TransitionSystem::are_local_labels_consistent() const
{
    for (const LocalLabelInfo& local_label_info : *this) {
        if (!local_label_info.is_consistent()) return false;
    }
    return true;
}

bool TransitionSystem::is_valid() const
{
    return are_local_labels_consistent() && is_label_mapping_consistent();
}

bool TransitionSystem::is_label_mapping_consistent() const
{
    for (int label : labels) {
        int local_label = label_to_local_label[label];
        const LabelGroup& label_group =
            local_label_infos[local_label].get_label_group();
        assert(!label_group.empty());

        if (find(label_group.begin(), label_group.end(), label) ==
            label_group.end()) {
            dump_label_mapping();
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
                dump_label_mapping();
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

void TransitionSystem::dump_label_mapping() const
{
    utils::g_log << "to local label mapping: ";
    for (int label : labels) {
        utils::g_log << label << " -> " << label_to_local_label[label] << ", ";
    }
    utils::g_log << endl;
    utils::g_log << "local to label mapping: ";
    for (size_t local_label = 0; local_label < local_label_infos.size();
         ++local_label) {
        utils::g_log << local_label << ": "
                     << local_label_infos[local_label].get_label_group()
                     << ", ";
    }
    utils::g_log << endl;
}

bool TransitionSystem::is_solvable(const Distances& distances) const
{
    if (init_state == PRUNED_STATE) {
        return false;
    }
    if (distances.are_goal_distances_computed() &&
        distances.get_goal_distance(init_state) == INFINITE_VALUE) {
        return false;
    }
    return true;
}

int TransitionSystem::compute_total_transitions() const
{
    int total = 0;
    for (const LocalLabelInfo& local_label_info : *this) {
        total += local_label_info.get_transitions().size();
    }
    return total;
}

string TransitionSystem::get_description() const
{
    ostringstream s;
    if (incorporated_variables.size() == 1) {
        s << "atomic transition system #" << *incorporated_variables.begin();
    } else {
        s << "composite transition system with "
          << incorporated_variables.size() << "/" << num_variables << " vars";
    }
    return s.str();
}

void TransitionSystem::dump_dot_graph(utils::LogProxy& log) const
{
    assert(is_valid());

    if (log.is_at_least_debug()) {
        log << "digraph transition_system";
        for (size_t i = 0; i < incorporated_variables.size(); ++i)
            log << "_" << incorporated_variables[i];
        log << " {" << endl;
        log << "    node [shape = none] start;" << endl;
        for (int i = 0; i < num_states; ++i) {
            bool is_init = (i == init_state);
            bool is_goal = goal_states[i];
            log << "    node [shape = " << (is_goal ? "doublecircle" : "circle")
                << "] node" << i << ";" << endl;
            if (is_init) log << "    start -> node" << i << ";" << endl;
        }

        // Introduce intermediate nodes for every transition
        size_t k = 0;
        for (const LocalLabelInfo& local_label_info : *this) {
            const vector<Transition>& transitions =
                local_label_info.get_transitions();
            for (size_t i = 0; i != transitions.size(); ++i) {
                log << "    node [shape = "
                    << "diamond] interm" << k++ << ";" << endl;
            }
        }

        k = 0;
        for (const LocalLabelInfo& local_label_info : *this) {
            const LabelGroup& label_group = local_label_info.get_label_group();
            const vector<Transition>& transitions =
                local_label_info.get_transitions();
            for (const Transition& transition : transitions) {
                // Connect source to intermediate node
                int src = transition.src;
                log << "    node" << src << " -> interm" << k << " [label = ";
                for (auto label_it = label_group.begin();
                     label_it != label_group.end();
                     ++label_it) {
                    if (label_it != label_group.begin()) log << "_";
                    log << "x" << *label_it;
                }
                log << "];" << endl;

                // Connect intermediate node to each target
                for (int target : transition.targets) {
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
    if (log.is_at_least_debug()) {
        log << tag() << "transitions" << endl;
        for (const LocalLabelInfo& local_label_info : *this) {
            const LabelGroup& label_group = local_label_info.get_label_group();
            log << "labels: " << label_group << endl;
            log << "transitions: ";
            const vector<Transition>& transitions =
                local_label_info.get_transitions();
            for (size_t i = 0; i < transitions.size(); ++i) {
                int src = transitions[i].src;
                auto& targets = transitions[i].targets;
                if (i != 0) log << ",";
                log << src << " -> (" << targets[0];
                for (int t : targets | std::views::drop(1)) {
                    log << ", " << t;
                }
                log << ")";
            }
            utils::g_log << "cost: " << local_label_info.get_cost() << endl;
        }
    }
}

void TransitionSystem::statistics(utils::LogProxy& log) const
{
    if (log.is_at_least_verbose()) {
        log << tag() << get_size() << " states, " << compute_total_transitions()
            << " arcs " << endl;
    }
}

} // namespace probfd::merge_and_shrink