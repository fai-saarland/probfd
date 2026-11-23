#ifndef PROBFD_COST_PARTITIONING_PATTERN_COLLECTION_SORTER_RANDOM_H
#define PROBFD_COST_PARTITIONING_PATTERN_COLLECTION_SORTER_RANDOM_H

#include "probfd/cost_partitioning/collection_sorter_random.h"

#include "probfd/pdbs/types.h"

namespace probfd::cost_partitioning {

using PatternCollectionSorterRandom =
    CollectionSorterRandom<pdbs::PatternCollection>;

} // namespace probfd::cost_partitioning

#endif
