
#ifndef PROBFD_COST_PARTITIONING_COLLECTION_SORTER_RANDOM_FACTORY_H
#define PROBFD_COST_PARTITIONING_COLLECTION_SORTER_RANDOM_FACTORY_H

#include "probfd/cost_partitioning/collection_sorter_factory.h"

#include "probfd/cost_partitioning/pattern_collection_sorter_random.h"

#include "downward/utils/rng.h"

#include <memory>

namespace probfd::cost_partitioning {

template <std::ranges::input_range T>
class CollectionSorterRandomFactory : public CollectionSorterFactory<T> {
    std::shared_ptr<downward::utils::RandomNumberGenerator> rng_;

public:
    explicit CollectionSorterRandomFactory(
        std::shared_ptr<downward::utils::RandomNumberGenerator> rng);

    std::unique_ptr<CollectionSorter<T>>
    create_object(const SharedProbabilisticTask& task);
};

template <std::ranges::input_range T>
CollectionSorterRandomFactory<T>::CollectionSorterRandomFactory(
    std::shared_ptr<downward::utils::RandomNumberGenerator> rng)
    : rng_(std::move(rng))
{
}

template <std::ranges::input_range T>
std::unique_ptr<CollectionSorter<T>>
CollectionSorterRandomFactory<T>::create_object(
    const SharedProbabilisticTask&)
{
    return std::make_unique<CollectionSorterRandom<T>>(rng_);
}

} // namespace probfd::cost_partitioning

#endif
