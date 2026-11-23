#ifndef PROBFD_COST_PARTITIONING_PATTERN_COLLECTION_SORTER_FACTORY_H
#define PROBFD_COST_PARTITIONING_PATTERN_COLLECTION_SORTER_FACTORY_H

#include "probfd/cost_partitioning/collection_sorter_factory.h"

#include "probfd/pdbs/types.h"

namespace probfd::cost_partitioning {

using PatternCollectionSorterFactory =
    CollectionSorterFactory<pdbs::PatternCollection>;

} // namespace probfd::cost_partitioning

#endif
