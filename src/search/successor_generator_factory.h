#ifndef SUCCESSOR_GENERATOR_FACTORY_H
#define SUCCESSOR_GENERATOR_FACTORY_H

#include <memory>
#include <vector>

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

template <typename Entry>
std::unique_ptr<GeneratorBase<Entry> > create(
    const std::vector<int>& domains,
    std::vector<SuccessorGeneratorFactoryInfo<Entry> >& infos);

} // namespace successor_generator

#include "successor_generator_factory.cc"

#endif
