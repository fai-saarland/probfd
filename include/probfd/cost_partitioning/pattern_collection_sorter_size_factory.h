
#ifndef PROBFD_COST_PARTITIONING_COLLECTION_SORTER_SIZE_FACTORY_H
#define PROBFD_COST_PARTITIONING_COLLECTION_SORTER_SIZE_FACTORY_H

#include "probfd/cost_partitioning/pattern_collection_sorter_factory.h"

#include "probfd/cost_partitioning/pattern_collection_sorter_size.h"

namespace probfd::cost_partitioning {

template <typename Proj>
class PatternCollectionSorterProjectionFactory
    : public PatternCollectionSorterFactory {
    Proj proj_;

public:
    explicit PatternCollectionSorterProjectionFactory(Proj proj);

    std::unique_ptr<PatternCollectionSorter>
    create_object(const SharedProbabilisticTask& task) override;
};

template <typename Proj>
PatternCollectionSorterProjectionFactory<
    Proj>::PatternCollectionSorterProjectionFactory(Proj proj)
    : proj_(std::move(proj))
{
}

template <typename Proj>
std::unique_ptr<PatternCollectionSorter>
PatternCollectionSorterProjectionFactory<Proj>::create_object(
    const SharedProbabilisticTask&)
{
    return std::make_unique<PatternCollectionSorterProjection<decltype(proj_)>>(
        proj_);
}

inline std::unique_ptr<PatternCollectionSorterFactory>
create_pattern_collection_sorter_size_ascending_factory()
{
    constexpr auto f = [](const pdbs::Pattern& pattern) {
        return pattern.size();
    };
    return std::make_unique<
        PatternCollectionSorterProjectionFactory<decltype(f)>>(f);
}

inline std::unique_ptr<PatternCollectionSorterFactory>
create_pattern_collection_sorter_size_descending_factory()
{
    constexpr auto f = [](const pdbs::Pattern& pattern) {
        return -static_cast<long long>(pattern.size());
    };
    return std::make_unique<
        PatternCollectionSorterProjectionFactory<decltype(f)>>(f);
}

} // namespace probfd::cost_partitioning

#endif
