#ifndef LEGACY_SUCCESSOR_GENERATOR_FACTORY_H
#define LEGACY_SUCCESSOR_GENERATOR_FACTORY_H

#include <memory>
#include <vector>

namespace legacy {
namespace successor_generator {

template <typename Entry>
class GeneratorBase;

using SuccessorGeneratorFactoryKey = std::pair<int, int>;

template <typename Entry>
class SuccessorGeneratorFactoryInfo {
public:
    explicit SuccessorGeneratorFactoryInfo(
        Entry op,
        std::vector<SuccessorGeneratorFactoryKey> precondition);
    bool operator<(const SuccessorGeneratorFactoryInfo<Entry>& other) const;
    Entry get() const;
    int first(int depth) const;
    int second(int depth) const;

private:
    Entry op;
    std::vector<SuccessorGeneratorFactoryKey> precondition;
};

template <typename Entry, typename RandomAccessIterator>
std::unique_ptr<GeneratorBase<Entry>> create(
    RandomAccessIterator domains,
    std::vector<SuccessorGeneratorFactoryInfo<Entry>>& infos);

} // namespace successor_generator
} // namespace legacy

#include "legacy/successor_generator_factory-impl.h"

#endif
