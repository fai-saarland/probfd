
#ifndef PROBFD_COST_PARTITIONING_COLLECTION_SORTER_SIZE_H
#define PROBFD_COST_PARTITIONING_COLLECTION_SORTER_SIZE_H

#include "probfd/cost_partitioning/pattern_collection_sorter.h"

#include <algorithm>

namespace probfd::cost_partitioning {

template <typename Proj>
class PatternCollectionSorterProjection : public PatternCollectionSorter {
    Proj proj_;

public:
    explicit PatternCollectionSorterProjection(Proj proj);

    void sort(pdbs::PatternCollection& range) override;
};

template <typename Proj>
PatternCollectionSorterProjection<Proj>::PatternCollectionSorterProjection(
    Proj proj)
    : proj_(std::move(proj))
{
}

template <typename Proj>
void PatternCollectionSorterProjection<Proj>::sort(
    pdbs::PatternCollection& range)
{
    std::ranges::sort(range, std::less{}, proj_);
}

inline std::unique_ptr<PatternCollectionSorter>
create_pattern_collection_sorter_size_ascending()
{
    constexpr auto f = [](const pdbs::Pattern& pattern) {
        return pattern.size();
    };
    return std::make_unique<PatternCollectionSorterProjection<decltype(f)>>(f);
}

inline std::unique_ptr<PatternCollectionSorter>
create_pattern_collection_sorter_size_descending()
{
    constexpr auto f = [](const pdbs::Pattern& pattern) {
        return -static_cast<long long>(pattern.size());
    };
    return std::make_unique<PatternCollectionSorterProjection<decltype(f)>>(f);
}

} // namespace probfd::cost_partitioning

#endif
