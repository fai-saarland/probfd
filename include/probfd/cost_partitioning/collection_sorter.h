#ifndef PROBFD_COST_PARTITIONING_COLLECTION_SORTER_H
#define PROBFD_COST_PARTITIONING_COLLECTION_SORTER_H

#include <concepts>
#include <ranges>

namespace probfd::cost_partitioning {

template <std::ranges::input_range T>
class CollectionSorter {
public:
    virtual ~CollectionSorter() = default;

    virtual void sort(T& range) = 0;
};

} // namespace probfd::cost_partitioning

#endif
