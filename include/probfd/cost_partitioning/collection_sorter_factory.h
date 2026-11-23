#ifndef PROBFD_COST_PARTITIONING_COLLECTION_SORTER_FACTORY_H
#define PROBFD_COST_PARTITIONING_COLLECTION_SORTER_FACTORY_H

#include "probfd/task_dependent_factory.h"

#include <concepts>
#include <ranges>

namespace probfd::cost_partitioning {
template <std::ranges::input_range T>
class CollectionSorter;
}

namespace probfd::cost_partitioning {

template <std::ranges::input_range T>
using CollectionSorterFactory = TaskDependentFactory<CollectionSorter<T>>;

} // namespace probfd::cost_partitioning

#endif
