
#ifndef PROBFD_COST_PARTITIONING_COLLECTION_SORTER_RANDOM_H
#define PROBFD_COST_PARTITIONING_COLLECTION_SORTER_RANDOM_H

#include "probfd/cost_partitioning/collection_sorter.h"

#include "downward/utils/rng.h"

#include <algorithm>
#include <memory>

namespace probfd::cost_partitioning {

template <std::ranges::input_range T>
class CollectionSorterRandom : public CollectionSorter<T> {
    std::shared_ptr<downward::utils::RandomNumberGenerator> rng_;

public:
    explicit CollectionSorterRandom(
        std::shared_ptr<downward::utils::RandomNumberGenerator> rng);

    void sort(T& range) override;
};

template <std::ranges::input_range T>
CollectionSorterRandom<T>::CollectionSorterRandom(
    std::shared_ptr<downward::utils::RandomNumberGenerator> rng)
    : rng_(std::move(rng))
{
}

template <std::ranges::input_range T>
void CollectionSorterRandom<T>::sort(T& range)
{
    rng_->shuffle(range);
}

} // namespace probfd::heuristics

#endif
