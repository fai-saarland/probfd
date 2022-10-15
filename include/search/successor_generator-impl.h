//#ifdef SUCCESSOR_GENERATOR_H

#include "successor_generator_factory.h"
#include "successor_generator_internals.h"

#include <algorithm>

namespace successor_generator {

template <typename Entry>
SuccessorGenerator<Entry>::SuccessorGenerator(
    std::unique_ptr<GeneratorBase<Entry>> root)
    : root(std::move(root))
{
}

template <typename Entry>
SuccessorGenerator<Entry>::~SuccessorGenerator() = default;

template <typename Entry>
void SuccessorGenerator<Entry>::generate_applicable_ops(
    const GlobalState& state,
    std::vector<Entry>& applicable_ops) const
{
    root->generate_applicable_ops(state, applicable_ops);
}

template <typename Entry>
void SuccessorGenerator<Entry>::generate_applicable_ops(
    const std::vector<int>& state,
    std::vector<Entry>& applicable_ops) const
{
    root->generate_applicable_ops(state, applicable_ops);
}

template <typename Entry>
void SuccessorGenerator<Entry>::generate_applicable_ops(
    const std::vector<std::pair<int, int>>& facts,
    std::vector<Entry>& applicable_ops) const
{
    root->generate_applicable_ops(0, facts, applicable_ops);
}

template <typename Entry>
template <typename RandomAccessIterator>
SuccessorGenerator<Entry>::SuccessorGenerator(
    RandomAccessIterator domains,
    const std::vector<std::vector<std::pair<int, int>>>& conditions,
    const std::vector<Entry>& values)
{
    assert(conditions.size() == values.size());
    std::vector<SuccessorGeneratorFactoryInfo<Entry>> operator_infos;
    for (unsigned i = 0; i < conditions.size(); i++) {
        assert(std::is_sorted(conditions[i].begin(), conditions[i].end()));
        operator_infos.emplace_back(values[i], conditions[i]);
    }
    root = create(domains, operator_infos);
}

} // namespace successor_generator
