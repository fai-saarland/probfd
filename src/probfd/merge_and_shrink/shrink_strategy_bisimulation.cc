#include "probfd/merge_and_shrink/shrink_strategy_bisimulation.h"

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "downward/utils/collections.h"
#include "downward/utils/logging.h"
#include "downward/utils/markup.h"
#include "downward/utils/system.h"

#include "downward/cli/plugins/plugin.h"

#include <algorithm>
#include <cassert>
#include <compare>
#include <deque>
#include <iostream>
#include <limits>
#include <memory>

using namespace std;
using namespace downward::cli::plugins;

namespace probfd::merge_and_shrink {

/* A successor signature characterizes the behaviour of an abstract
   state in so far as bisimulation cares about it. States with
   identical successor signature are not distinguished by
   bisimulation.

   Each entry in the vector is a pair of (label group ID, equivalence class of
   successor). The bisimulation algorithm requires that the vector is
   sorted and uniquified. */

using LabelTargetClassesPair = pair<int, std::vector<int>>;
using SuccessorSignature = set<LabelTargetClassesPair>;

/*
  The following class encodes all we need to know about a state for
  bisimulation: its h value, which equivalence class ("group") it currently
  belongs to, its successor signature (see above), and what the original
  state is.
*/

struct Signature {
    int group;
    SuccessorSignature succ_signature;
    int state;

    Signature(int group_, int state_)
        : group(group_)
        , state(state_)
    {
    }

    /*
     * Two successor signatures of two states compare equal according to
     * SuccessorSignature::operator== iff both states are bisimilar in the
     * current iteration.
     */
    friend auto operator<=>(const Signature&, const Signature&) = default;

    void dump(utils::LogProxy& log) const
    {
        if (log.is_at_least_debug()) {
            log << "Signature(group = " << group << ", state = " << state
                << ", succ_sig = [";

            auto it = succ_signature.begin();

            if (const auto end = succ_signature.end(); it != end) {
                log << "(" << it->first << "," << it->second << ")";

                for (++it; it != end; ++it) {
                    log << ", (" << it->first << "," << it->second << ")";
                }
            }

            log << "])" << endl;
        }
    }
};

ShrinkStrategyBisimulation::ShrinkStrategyBisimulation(
    AtLimit at_limit,
    const bool require_goal_distances)
    : at_limit(at_limit)
    , require_goal_distances(require_goal_distances)
{
}

static int initialize_groups(
    const TransitionSystem& ts,
    const Distances& distances,
    vector<int>& state_to_group)
{
    /* Group 0 holds all goal states.

       Each other group holds all states with one particular h value.

       Note that some goal state *must* exist because irrelevant and
       unreachable states are pruned before we shrink and we never
       perform the shrinking if that pruning shows that the problem is
       unsolvable.
    */

    map<value_t, int> h_to_group;
    int num_groups = 1; // Group 0 is for goal states.

    for (int state = 0; state < ts.get_size(); ++state) {
        if (ts.is_goal_state(state)) {
            assert(distances.get_goal_distance(state) == 0);
            state_to_group[state] = 0;
            continue;
        }

        value_t h = distances.get_goal_distance(state);

        auto [first, end] = std::ranges::equal_range(
            h_to_group,
            h,
            [](value_t x, value_t y) {
                return probfd::is_approx_less(x, y, 0.001);
            },
            [](const auto& p) { return p.first; });

        if (first == end) {
            auto [it, inserted] = h_to_group.emplace(h, num_groups);
            assert(inserted);
            state_to_group[state] = it->second;
            ++num_groups;
        } else {
            state_to_group[state] = first->second;
        }
    }

    return num_groups;
}

static void compute_signatures(
    const TransitionSystem& ts,
    vector<Signature>& signatures,
    const vector<int>& state_to_group)
{
    assert(signatures.empty());

    // Step 1: Compute bare state signatures (without transition information).
    for (int state = 0; state < ts.get_size(); ++state) {
        signatures.emplace_back(state_to_group[state], state);
    }

    // Step 2: Add transition information.
    // Note that the final result of the bisimulation may depend on the
    // order in which transitions are considered below.

    int label_group_counter = 0;

    for (const LocalLabelInfo& local_label_info : ts.label_infos()) {
        for (const auto& [src, targets] : local_label_info.get_transitions()) {
            assert(signatures[src].state == src);

            std::vector<int> target_groups(targets.size());
            for (int target : targets) {
                target_groups.emplace_back(target);
            }
            signatures[src].succ_signature.emplace(
                label_group_counter,
                target_groups);
        }
        ++label_group_counter;
    }

    // Step 3: Canonicalize the representation. Signature::operator< defines a
    // total order such that states that currently fall into the same group
    // form contiguous subsequences.

    ranges::sort(signatures);
}

StateEquivalenceRelation ShrinkStrategyBisimulation::compute_equivalence_relation(
    const TransitionSystem& ts,
    const Distances& distances,
    int target_size,
    utils::LogProxy&) const
{
    assert(distances.are_goal_distances_computed());
    const int num_states = ts.get_size();

    vector<int> state_to_group(num_states);

    int num_groups = initialize_groups(ts, distances, state_to_group);

    // TODO: We currently violate this; see issue250
    // assert(num_groups <= target_size);

    vector<Signature> signatures;
    signatures.reserve(num_states);

    for (bool stable = false; !stable && num_groups < target_size;
         signatures.clear()) {
        stable = true;

        compute_signatures(ts, signatures, state_to_group);

        // Verify size of signatures.
        assert(static_cast<int>(signatures.size()) == num_states);

        int sig_end;

        for (int sig_start = 0; sig_start != num_states; sig_start = sig_end) {
            // Split the current group.
            int num_new_groups = 0;
            for (sig_end = sig_start + 1; sig_end < num_states; ++sig_end) {
                const Signature& prev_sig = signatures[sig_end - 1];
                const Signature& curr_sig = signatures[sig_end];

                if (prev_sig.group != curr_sig.group) break; // end of group

                // If successor sigs differ, split the group
                if (prev_sig.succ_signature != curr_sig.succ_signature) {
                    ++num_new_groups;
                }
            }

            if (num_new_groups == 0) {
                continue;
            }

            if (at_limit == AtLimit::RETURN &&
                num_groups + num_new_groups > target_size) {
                /* Can't split the group (or the set of groups for
                   this h value) -- would exceed bound on abstract
                   state number.
                */
                goto break_outer_loop;
            }

            // Split into new groups.
            stable = false;

            for (int i = sig_start + 1; i < sig_end; ++i) {
                const Signature& prev_sig = signatures[i - 1];
                const Signature& curr_sig = signatures[i];

                assert(prev_sig.group == curr_sig.group);

                if (prev_sig.succ_signature != curr_sig.succ_signature) {
                    state_to_group[curr_sig.state] = num_groups;
                    assert(num_groups <= target_size);
                    if (++num_groups == target_size) break;
                }
            }
        }
    }

break_outer_loop:;

    /* Reduce memory pressure before generating the equivalence
       relation since this is one of the code parts relevant to peak
       memory. */
    utils::release_vector_memory(signatures);

    // Generate final result.
    StateEquivalenceRelation equivalence_relation;
    equivalence_relation.resize(num_groups);
    for (int state = 0; state < num_states; ++state) {
        if (const int group = state_to_group[state]; group != -1) {
            assert(group >= 0 && group < num_groups);
            equivalence_relation[group].push_front(state);
        }
    }

    return equivalence_relation;
}

string ShrinkStrategyBisimulation::name() const
{
    return "bisimulation";
}

void ShrinkStrategyBisimulation::dump_strategy_specific_options(
    utils::LogProxy& log) const
{
    if (log.is_at_least_normal()) {
        log << "At limit: ";
        if (at_limit == AtLimit::RETURN) {
            log << "return";
        } else if (at_limit == AtLimit::USE_UP) {
            log << "use up limit";
        } else {
            ABORT("Unknown setting for at_limit.");
        }
        log << endl;
    }
}

namespace {
class ShrinkBisimulationFeature
    : public TypedFeature<ShrinkStrategy, ShrinkStrategyBisimulation> {
public:
    ShrinkBisimulationFeature()
        : TypedFeature("pshrink_bisimulation")
    {
        document_title("Bismulation based shrink strategy");
        document_synopsis(
            "This shrink strategy implements the algorithm described in"
            " the paper:" +
            utils::format_conference_reference(
                {"Raz Nissim", "Joerg Hoffmann", "Malte Helmert"},
                "Computing Perfect Heuristics in Polynomial Time: On "
                "Bisimulation"
                " and Merge-and-Shrink Abstractions in Optimal Planning.",
                "https://ai.dmi.unibas.ch/papers/nissim-et-al-ijcai2011.pdf",
                "Proceedings of the Twenty-Second International Joint "
                "Conference"
                " on Artificial Intelligence (IJCAI 2011)",
                "1983-1990",
                "AAAI Press",
                "2011"));

        add_option<ShrinkStrategyBisimulation::AtLimit>(
            "at_limit",
            "what to do when the size limit is hit",
            "return");

        add_option<bool>(
            "require_goal_distances",
            "whether goal distances are required",
            "true");
    }

protected:
    shared_ptr<ShrinkStrategyBisimulation>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return make_shared_from_arg_tuples<ShrinkStrategyBisimulation>(
            options.get<ShrinkStrategyBisimulation::AtLimit>("at_limit"),
            options.get<bool>("require_goal_distances"));
    }
};

FeaturePlugin<ShrinkBisimulationFeature> _plugin;

TypedEnumPlugin<ShrinkStrategyBisimulation::AtLimit> _enum_plugin(
    {{"return", "stop without refining the equivalence class further"},
     {"use_up",
      "continue refining the equivalence class until "
      "the size limit is hit"}});
} // namespace

} // namespace probfd::merge_and_shrink