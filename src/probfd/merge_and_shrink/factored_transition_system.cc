#include "probfd/merge_and_shrink/factored_transition_system.h"

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/factored_mapping.h"
#include "probfd/merge_and_shrink/labels.h"
#include "probfd/merge_and_shrink/transition_system.h"
#include "probfd/merge_and_shrink/utils.h"

#include "probfd/utils/bind.h"

#include "downward/utils/collections.h"
#include "downward/utils/logging.h"
#include "downward/utils/system.h"

#ifndef NDEBUG
#include "downward/utils/collections.h"
#endif

#include <algorithm>
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
    vector<Factor>&& factors)
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
        cerr << "Factor at index is in an inconsistent state!" << endl;
        utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    }
}

bool FactoredTransitionSystem::is_component_valid(int index) const
{
    assert(is_active(index));
    return is_factor_valid(factors[index]);
}

bool FactoredTransitionSystem::is_factor_valid(const Factor& factor) const
{
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
    fm->apply_abstraction(abstraction_mapping);

    /* If distances need to be recomputed, this already happened in the
       Distances object. */
    assert(is_component_valid(index));
    return true;
}

auto FactoredTransitionSystem::merge(
    int index1,
    int index2,
    utils::LogProxy& log) -> MergeResult
{
    assert(is_component_valid(index1));
    assert(is_component_valid(index2));

    auto&& [ts1, fm1, distances1] = factors[index1];
    auto&& [ts2, fm2, distances2] = factors[index2];

    auto&& f = factors.emplace_back();
    auto&& [ts, fm, distances] = f;

    ts = TransitionSystem::merge(labels, *ts1, *ts2, log);

    fm = std::make_unique<FactoredMappingMerge>(std::move(fm1), std::move(fm2));

    distances = std::make_unique<Distances>();

    --num_active_entries;

    const int new_index = factors.size() - 1;

    assert(is_component_valid(new_index));

    return {
        std::move(factors[index1]),
        std::move(factors[index2]),
        f,
        new_index};
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