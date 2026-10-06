#include "probfd/merge_and_shrink/factored_transition_system.h"

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/factored_mapping.h"
#include "probfd/merge_and_shrink/labels.h"
#include "probfd/merge_and_shrink/transition_system.h"
#include "probfd/merge_and_shrink/utils.h"

#include "probfd/utils/bind.h"

#include "downward/utils/collections.h"
#include "downward/utils/exceptions.h"
#include "downward/utils/logging.h"
#include "downward/utils/system.h"

#ifndef NDEBUG
#include "downward/utils/collections.h"
#endif

#include <algorithm>
#include <cassert>
#include <utility>

using namespace std;
using namespace downward;

namespace probfd::merge_and_shrink {

FTSConstIterator::FTSConstIterator(
    const FactoredTransitionSystem& fts,
    bool end)
    : fts(fts)
    , current_index(end ? fts.get_size() : 0)
{
    next_valid_index();
}

void FTSConstIterator::next_valid_index()
{
    while (std::cmp_less(current_index, fts.get_size()) &&
           !fts.is_active(current_index)) {
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

bool Factor::apply_abstraction(
    const Labels& labels,
    const StateEquivalenceRelation& state_equivalence_relation,
    bool do_compute_goal_distances,
    bool do_compute_liveness,
    utils::LogProxy& log)
{
    if (const auto new_num_states = state_equivalence_relation.size();
        new_num_states == transition_system->get_size()) {
        return false;
    }

    const vector<int> abstraction_mapping = compute_abstraction_mapping(
        transition_system->get_size(),
        state_equivalence_relation);

    transition_system->apply_abstraction(
        labels,
        state_equivalence_relation,
        abstraction_mapping,
        log);

    factored_mapping->apply_abstraction(abstraction_mapping);

    if (do_compute_goal_distances) {
        distances->apply_abstraction(
            labels,
            *transition_system,
            state_equivalence_relation,
            do_compute_liveness,
            log);
    }

    return true;
}

bool Factor::is_valid() const
{
    return (transition_system && factored_mapping && distances) ||
           (!transition_system && !factored_mapping && !distances);
}

bool Factor::is_trivial() const
{
    if (!factored_mapping->is_total()) {
        return false;
    }

    for (int state = 0;
         std::cmp_not_equal(state, transition_system->get_size());
         ++state) {
        if (!transition_system->is_goal_state(state)) {
            return false;
        }
    }

    return true;
}

bool Factor::is_solvable() const
{
    return transition_system->is_solvable(*distances);
}

void Factor::dump(utils::LogProxy& log) const
{
    if (log.is_at_least_debug()) {
        transition_system->dump_labels_and_transitions(log);
        factored_mapping->dump(log);
    }
}

void Factor::statistics(utils::LogProxy& log) const
{
    if (log.is_at_least_verbose()) {
        transition_system->dump_statistics(log);
        distances->statistics(*transition_system, log);
    }
}

FactoredTransitionSystem::FactoredTransitionSystem(
    Labels labels,
    vector<Factor> factors)
    : labels(std::move(labels))
    , factors(std::move(factors))
    , num_active_entries(this->factors.size())
{
    assert(
        std::ranges::all_of(
            std::as_const(this->factors),
            probfd::bind_front<&FactoredTransitionSystem::is_factor_valid>(
                std::ref(*this))));
}

void FactoredTransitionSystem::assert_index_valid(int index) const
{
    assert(utils::in_bounds(index, factors));
    if (!factors[index].is_valid()) {
        throw utils::CriticalError(
            "Factor at index {} is in an inconsistent state!",
            index);
    }
}

bool FactoredTransitionSystem::is_factor_valid(const Factor& factor) const
{
    return factor.transition_system->is_valid(labels);
}

void FactoredTransitionSystem::assert_all_components_valid() const
{
    for (const auto& factor : factors) {
        if (factor.transition_system) {
            assert(is_factor_valid(factor));
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

    for (size_t i = 0; i < combinable_index; ++i) {
        if (const auto& ts = factors[i].transition_system) {
            ts->apply_equivalent_label_reduction(labels, label_mapping);
        }
    }

    factors[combinable_index]
        .transition_system->apply_non_equivalent_label_reduction(
            labels,
            label_mapping);

    for (size_t i = combinable_index + 1; i < factors.size(); ++i) {
        if (const auto& ts = factors[i].transition_system) {
            ts->apply_equivalent_label_reduction(labels, label_mapping);
        }
    }

    assert_all_components_valid();
}

bool FactoredTransitionSystem::apply_abstraction(
    int index,
    const StateEquivalenceRelation& state_equivalence_relation,
    bool do_compute_goal_distances,
    bool do_compute_liveness,
    utils::LogProxy& log)
{
    Factor& factor = factors[index];

    assert(is_factor_valid(factor));

    const bool b = factor.apply_abstraction(
        labels,
        state_equivalence_relation,
        do_compute_goal_distances,
        do_compute_liveness,
        log);

    /* If distances need to be recomputed, this already happened in the
       Distances object. */
    assert(is_factor_valid(factor));

    return b;
}

auto FactoredTransitionSystem::merge(
    int index1,
    int index2,
    utils::LogProxy& log) -> MergeResult
{
    Factor& factor1 = factors[index1];
    Factor& factor2 = factors[index2];

    assert(is_factor_valid(factor1));
    assert(is_factor_valid(factor2));

    auto&& [ts1, fm1, distances1] = factor1;
    auto&& [ts2, fm2, distances2] = factor2;

    auto&& merged_factor = factors.emplace_back();
    auto&& [ts, fm, distances] = merged_factor;

    ts = TransitionSystem::merge(labels, *ts1, *ts2, log);

    fm = std::make_unique<FactoredMappingMerge>(std::move(fm1), std::move(fm2));

    distances = std::make_unique<Distances>();

    --num_active_entries;

    const int new_index = factors.size() - 1;

    assert(is_factor_valid(merged_factor));

    return {.left_factor = std::move(factors[index1]),
            .right_factor = std::move(factors[index2]),
            .merged_factor = merged_factor,
            .merge_index = new_index};
}

Factor FactoredTransitionSystem::extract_factor(int index)
{
    Factor& factor = factors[index];
    assert(is_factor_valid(factor));
    return std::move(factor);
}

void FactoredTransitionSystem::statistics(int index, utils::LogProxy& log) const
{
    const Factor& factor = factors[index];
    assert(is_factor_valid(factor));
    factor.statistics(log);
}

void FactoredTransitionSystem::dump(utils::LogProxy& log) const
{
    if (log.is_at_least_debug()) {
        for (const int index : *this) {
            assert_index_valid(index);
            factors[index].dump(log);
        }
    }
}

bool FactoredTransitionSystem::is_factor_solvable(int index) const
{
    const Factor& factor = factors[index];
    assert(is_factor_valid(factor));
    return factor.is_solvable();
}

bool FactoredTransitionSystem::is_factor_trivial(int index) const
{
    const Factor& factor = factors[index];
    assert(is_factor_valid(factor));
    return factor.is_trivial();
}

bool FactoredTransitionSystem::is_active(int index) const
{
    assert_index_valid(index);
    return factors[index].transition_system != nullptr;
}

} // namespace probfd::merge_and_shrink