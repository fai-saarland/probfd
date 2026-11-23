#ifndef PROBFD_COST_PARTITIONING_PATTERN_COLLECTION_SORTER_RANDOM_FACTORY_H
#define PROBFD_COST_PARTITIONING_PATTERN_COLLECTION_SORTER_RANDOM_FACTORY_H

#include "probfd/cost_partitioning/collection_sorter_random_factory.h"

#include "probfd/pdbs/types.h"

namespace probfd::cost_partitioning {

using PatternCollectionSorterRandomFactory =
    CollectionSorterRandomFactory<pdbs::PatternCollection>;

} // namespace probfd::cost_partitioning

#endif
