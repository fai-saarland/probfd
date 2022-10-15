#ifndef SUCCESSOR_GENERATOR_H
#define SUCCESSOR_GENERATOR_H

#include <memory>
#include <vector>

class GlobalState;
class GlobalOperator;

namespace successor_generator {

template <typename Entry>
class GeneratorBase;

template <typename Entry>
class SuccessorGenerator {
    std::unique_ptr<GeneratorBase<Entry>> root;

    explicit SuccessorGenerator(std::unique_ptr<GeneratorBase<Entry>> root);

public:
    explicit SuccessorGenerator();

    template <typename RandomAccessIterator>
    explicit SuccessorGenerator(
        RandomAccessIterator domains,
        const std::vector<std::vector<std::pair<int, int>>>& conditions,
        const std::vector<Entry>& values);
    /*
      We cannot use the default destructor (implicitly or explicitly)
      here because GeneratorBase is a forward declaration and the
      incomplete type cannot be destroyed.
    */
    ~SuccessorGenerator();

    // Transitional method, used until the search is switched to the new task
    // interface.
    void generate_applicable_ops(
        const GlobalState& state,
        std::vector<Entry>& applicable_ops) const;

    void generate_applicable_ops(
        const std::vector<int>& state,
        std::vector<Entry>& applicable_ops) const;

    void generate_applicable_ops(
        const std::vector<std::pair<int, int>>& facts,
        std::vector<Entry>& applicable_ops) const;
};

extern std::shared_ptr<SuccessorGenerator<const GlobalOperator*>>
    g_successor_generator;

} // namespace successor_generator

#include "successor_generator-impl.h"

#endif
