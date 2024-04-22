#include "probfd/merge_and_shrink/factored_transition_system.h"

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/factored_mapping.h"
#include "probfd/merge_and_shrink/labels.h"
#include "probfd/merge_and_shrink/transition_system.h"
#include "probfd/merge_and_shrink/utils.h"

#include "downward/utils/collections.h"
#include "downward/utils/logging.h"
#include "downward/utils/system.h"

#ifndef NDEBUG
#include "downward/utils/collections.h"
#endif

#include <cassert>

using namespace std;

namespace probfd::merge_and_shrink {

FTSConstIterator::FTSConstIterator(
    const FactoredTransitionSystem& fts,
    bool end)
    : fts(fts)
    , current_index((end ? fts.get_size() : 0))
{
    next_valid_index();
}

void FTSConstIterator::next_valid_index()
{
    while (current_index < fts.get_size() && !fts.is_active(current_index)) {
        ++current_index;
    }
}

void FTSConstIterator::operator++()
{
    ++current_index;
    next_valid_index();
}

Factor::Factor(Factor&&) noexcept = default;
Factor& Factor::operator=(Factor&&) noexcept = default;
Factor::~Factor() = default;

bool Factor::is_valid() const
{
    return (transition_system && factored_mapping && distances) ||
           (!transition_system && !factored_mapping && !distances);
}

FactoredTransitionSystem::FactoredTransitionSystem(
    Labels labels,
    vector<Factor>&& factors,
    const bool compute_liveness,
    const bool compute_goal_distances,
    utils::LogProxy& log)
    : labels(std::move(labels))
    , factors(std::move(factors))
    , compute_liveness(compute_liveness)
    , compute_goal_distances(compute_goal_distances)
    , num_active_entries(this->factors.size())
{
    assert(!compute_liveness || compute_goal_distances);
    for (size_t index = 0; index < this->factors.size(); ++index) {
        if (compute_goal_distances) {
            Factor& factor = this->factors[index];
            factor.distances->compute_distances(
                *factor.transition_system,
                compute_liveness,
                log);
        }
        assert(is_component_valid(index));
    }
}

void FactoredTransitionSystem::assert_index_valid(int index) const
{
    assert(utils::in_bounds(index, factors));
    if (!factors[index].is_valid()) {
        cerr << "Factor at index is in an inconsistent state!" << endl;
        utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    }
}

bool FactoredTransitionSystem::is_component_valid(int index) const
{
    assert(is_active(index));
    const Factor& factor = factors[index];

    if (compute_liveness && !factor.distances->is_liveness_computed()) {
        return false;
    }

    if (compute_goal_distances &&
        !factor.distances->are_goal_distances_computed()) {
        return false;
    }

    return factor.transition_system->is_valid(labels);
}

void FactoredTransitionSystem::assert_all_components_valid() const
{
    for (size_t index = 0; index < factors.size(); ++index) {
        if (factors[index].transition_system) {
            assert(is_component_valid(index));
        }
    }
}

void FactoredTransitionSystem::apply_label_mapping(
    const vector<pair<int, vector<int>>>& label_mapping,
    int combinable_index)
{
    assert_all_components_valid();
    for (const auto& [fst, old_labels] : label_mapping) {
        assert(fst == labels.get_num_total_labels());
        labels.reduce_labels(old_labels);
    }
    for (size_t i = 0; i < factors.size(); ++i) {
        if (factors[i].transition_system) {
            factors[i].transition_system->apply_label_reduction(
                labels,
                label_mapping,
                static_cast<int>(i) != combinable_index);
        }
    }

    assert_all_components_valid();
}

bool FactoredTransitionSystem::apply_abstraction(
    int index,
    const StateEquivalenceRelation& state_equivalence_relation,
    utils::LogProxy& log)
{
    assert(is_component_valid(index));

    auto&& [ts, fm, distances] = factors[index];

    if (const int new_num_states = state_equivalence_relation.size();
        new_num_states == ts->get_size()) {
        return false;
    }

    const vector<int> abstraction_mapping =
        compute_abstraction_mapping(ts->get_size(), state_equivalence_relation);

    ts->apply_abstraction(state_equivalence_relation, abstraction_mapping, log);
    if (compute_goal_distances) {
        distances->apply_abstraction(
            *ts,
            state_equivalence_relation,
            compute_liveness,
            log);
    }
    fm->apply_abstraction(abstraction_mapping);

    /* If distances need to be recomputed, this already happened in the
       Distances object. */
    assert(is_component_valid(index));
    return true;
}

int FactoredTransitionSystem::merge(
    int index1,
    int index2,
    utils::LogProxy& log)
{
    assert(is_component_valid(index1));
    assert(is_component_valid(index2));

    auto&& [ts1, fm1, distances1] = factors[index1];
    auto&& [ts2, fm2, distances2] = factors[index2];

    auto&& [ts, fm, distances] = factors.emplace_back();

    ts = TransitionSystem::merge(labels, *ts1, *ts2, log);
    ts1 = nullptr;
    ts2 = nullptr;

    if (!compute_goal_distances) {
        distances1 = nullptr;
        distances2 = nullptr;
    }

    fm = std::make_unique<FactoredMappingMerge>(
        std::move(fm1),
        std::move(fm2));
    fm1 = nullptr;
    fm2 = nullptr;

    distances = std::make_unique<Distances>();

    class MergeHeuristic : public Evaluator<int> {
        const FactoredMappingMerge& merge_fm;
        std::vector<value_t> distance_table1;
        std::vector<value_t> distance_table2;

    public:
        MergeHeuristic(
            const FactoredMappingMerge& merge_fm,
            Distances& distances1,
            Distances& distances2)
            : merge_fm(merge_fm)
            , distance_table1(distances1.extract_goal_distances())
            , distance_table2(distances2.extract_goal_distances())
        {
        }

        value_t evaluate(int state) const override
        {
            const auto [left, right] = merge_fm.get_children_states(state);
            return std::max(distance_table1[left], distance_table2[right]);
        }
    };

    // Restore the invariant that distances are computed.
    if (compute_goal_distances) {
        MergeHeuristic heuristic(
            static_cast<const FactoredMappingMerge&>(*fm),
            *distances1,
            *distances2);

        distances1 = nullptr;
        distances2 = nullptr;

        distances->compute_distances(*ts, compute_liveness, log, heuristic);
    }
    --num_active_entries;

    int new_index = factors.size() - 1;
    assert(is_component_valid(new_index));
    return new_index;
}

Factor FactoredTransitionSystem::extract_factor(int index)
{
    assert(is_component_valid(index));
    return std::move(factors[index]);
}

void FactoredTransitionSystem::statistics(int index, utils::LogProxy& log) const
{
    if (log.is_at_least_verbose()) {
        assert(is_component_valid(index));
        const Factor& factor = factors[index];
        factor.transition_system->dump_statistics(log);
        factor.distances->statistics(*factor.transition_system, log);
    }
}

void FactoredTransitionSystem::dump(int index, utils::LogProxy& log) const
{
    if (log.is_at_least_debug()) {
        assert_index_valid(index);
        const Factor& factor = factors[index];
        factor.transition_system->dump_labels_and_transitions(log);
        factor.factored_mapping->dump(log);
    }
}

void FactoredTransitionSystem::dump(utils::LogProxy& log) const
{
    if (log.is_at_least_debug()) {
        for (const int index : *this) {
            dump(index, log);
        }
    }
}

bool FactoredTransitionSystem::is_factor_solvable(int index) const
{
    assert(is_component_valid(index));
    const Factor& factor = factors[index];
    return factor.transition_system->is_solvable(*factor.distances);
}

bool FactoredTransitionSystem::is_factor_trivial(int index) const
{
    assert(is_component_valid(index));
    const Factor& factor = factors[index];

    if (!factor.factored_mapping->is_total()) {
        return false;
    }
    const TransitionSystem& ts = *factor.transition_system;
    for (int state = 0; state < ts.get_size(); ++state) {
        if (!ts.is_goal_state(state)) {
            return false;
        }
    }
    return true;
}

bool FactoredTransitionSystem::is_active(int index) const
{
    assert_index_valid(index);
    return factors[index].transition_system != nullptr;
}

} // namespace probfd::merge_and_shrink