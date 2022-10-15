#ifndef TASK_UTILS_SUCCESSOR_GENERATOR_INTERNALS_H
#define TASK_UTILS_SUCCESSOR_GENERATOR_INTERNALS_H

#include <memory>
#include <unordered_map>
#include <vector>

class GlobalState;

namespace successor_generator {

template <typename Entry>
class GeneratorBase {
public:
    virtual ~GeneratorBase() {}

    virtual void generate_applicable_ops(
        const std::vector<int>& state,
        std::vector<Entry>& applicable_ops) const = 0;

    virtual void generate_applicable_ops(
        const GlobalState& state,
        std::vector<Entry>& applicable_ops) const = 0;

    virtual void generate_applicable_ops(
        unsigned i,
        const std::vector<std::pair<int, int>>& facts,
        std::vector<Entry>& result) const = 0;
};

template <typename Entry>
class GeneratorForkBinary : public GeneratorBase<Entry> {
    std::unique_ptr<GeneratorBase<Entry>> generator1;
    std::unique_ptr<GeneratorBase<Entry>> generator2;

public:
    GeneratorForkBinary(
        std::unique_ptr<GeneratorBase<Entry>> generator1,
        std::unique_ptr<GeneratorBase<Entry>> generator2);

    virtual void generate_applicable_ops(
        const std::vector<int>& state,
        std::vector<Entry>& applicable_ops) const override;

    virtual void generate_applicable_ops(
        const GlobalState& state,
        std::vector<Entry>& applicable_ops) const override;

    virtual void generate_applicable_ops(
        unsigned i,
        const std::vector<std::pair<int, int>>& facts,
        std::vector<Entry>& result) const override;
};

template <typename Entry>
class GeneratorForkMulti : public GeneratorBase<Entry> {
    std::vector<std::unique_ptr<GeneratorBase<Entry>>> children;

public:
    GeneratorForkMulti(
        std::vector<std::unique_ptr<GeneratorBase<Entry>>> children);

    virtual void generate_applicable_ops(
        const std::vector<int>& state,
        std::vector<Entry>& applicable_ops) const override;

    virtual void generate_applicable_ops(
        const GlobalState& state,
        std::vector<Entry>& applicable_ops) const override;

    virtual void generate_applicable_ops(
        unsigned i,
        const std::vector<std::pair<int, int>>& facts,
        std::vector<Entry>& result) const override;
};

template <typename Entry>
class GeneratorSwitchVector : public GeneratorBase<Entry> {
    int switch_var_id;
    std::vector<std::unique_ptr<GeneratorBase<Entry>>> generator_for_value;

public:
    GeneratorSwitchVector(
        int switch_var_id,
        std::vector<std::unique_ptr<GeneratorBase<Entry>>>&&
            generator_for_value);

    virtual void generate_applicable_ops(
        const std::vector<int>& state,
        std::vector<Entry>& applicable_ops) const override;

    virtual void generate_applicable_ops(
        const GlobalState& state,
        std::vector<Entry>& applicable_ops) const override;

    virtual void generate_applicable_ops(
        unsigned i,
        const std::vector<std::pair<int, int>>& facts,
        std::vector<Entry>& result) const override;
};

template <typename Entry>
class GeneratorSwitchHash : public GeneratorBase<Entry> {
    int switch_var_id;
    std::unordered_map<int, std::unique_ptr<GeneratorBase<Entry>>>
        generator_for_value;

public:
    GeneratorSwitchHash(
        int switch_var_id,
        std::unordered_map<int, std::unique_ptr<GeneratorBase<Entry>>>&&
            generator_for_value);

    virtual void generate_applicable_ops(
        const std::vector<int>& state,
        std::vector<Entry>& applicable_ops) const override;

    virtual void generate_applicable_ops(
        const GlobalState& state,
        std::vector<Entry>& applicable_ops) const override;

    virtual void generate_applicable_ops(
        unsigned i,
        const std::vector<std::pair<int, int>>& facts,
        std::vector<Entry>& result) const override;
};

template <typename Entry>
class GeneratorSwitchSingle : public GeneratorBase<Entry> {
    int switch_var_id;
    int value;
    std::unique_ptr<GeneratorBase<Entry>> generator_for_value;

public:
    GeneratorSwitchSingle(
        int switch_var_id,
        int value,
        std::unique_ptr<GeneratorBase<Entry>> generator_for_value);

    virtual void generate_applicable_ops(
        const std::vector<int>& state,
        std::vector<Entry>& applicable_ops) const override;

    virtual void generate_applicable_ops(
        const GlobalState& state,
        std::vector<Entry>& applicable_ops) const override;

    virtual void generate_applicable_ops(
        unsigned i,
        const std::vector<std::pair<int, int>>& facts,
        std::vector<Entry>& result) const override;
};

template <typename Entry>
class GeneratorLeafVector : public GeneratorBase<Entry> {
    std::vector<Entry> applicable_operators;

public:
    GeneratorLeafVector(std::vector<Entry>&& applicable_operators);

    virtual void generate_applicable_ops(
        const std::vector<int>& state,
        std::vector<Entry>& applicable_ops) const override;

    virtual void generate_applicable_ops(
        const GlobalState& state,
        std::vector<Entry>& applicable_ops) const override;

    virtual void generate_applicable_ops(
        unsigned i,
        const std::vector<std::pair<int, int>>& facts,
        std::vector<Entry>& result) const override;
};

template <typename Entry>
class GeneratorLeafSingle : public GeneratorBase<Entry> {
    Entry applicable_operator;

public:
    GeneratorLeafSingle(Entry applicable_operator);

    virtual void generate_applicable_ops(
        const std::vector<int>& state,
        std::vector<Entry>& applicable_ops) const override;

    virtual void generate_applicable_ops(
        const GlobalState& state,
        std::vector<Entry>& applicable_ops) const override;

    virtual void generate_applicable_ops(
        unsigned i,
        const std::vector<std::pair<int, int>>& facts,
        std::vector<Entry>& result) const override;
};

} // namespace successor_generator

#include "successor_generator_internals-impl.h"

#endif
