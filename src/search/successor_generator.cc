#ifdef SUCCESSOR_GENERATOR_H

#include "successor_generator_factory.h"
#include "successor_generator_internals.h"

#include <algorithm>

namespace successor_generator {

template <typename Entry>
SuccessorGenerator<Entry>::SuccessorGenerator(
    std::unique_ptr<GeneratorBase<Entry>> root)
    : root(std::move(root))
{
}

template <typename Entry>
SuccessorGenerator<Entry>::~SuccessorGenerator() = default;

template <typename Entry>
void SuccessorGenerator<Entry>::generate_applicable_ops(
    const GlobalState& state,
    std::vector<Entry>& applicable_ops) const
{
    root->generate_applicable_ops(state, applicable_ops);
}

template <typename Entry>
void SuccessorGenerator<Entry>::generate_applicable_ops(
    const std::vector<int>& state,
    std::vector<Entry>& applicable_ops) const
{
    root->generate_applicable_ops(state, applicable_ops);
}

template <typename Entry>
void SuccessorGenerator<Entry>::generate_applicable_ops(
    const std::vector<std::pair<int, int>>& facts,
    std::vector<Entry>& applicable_ops) const
{
    root->generate_applicable_ops(0, facts, applicable_ops);
}

template <typename Entry>
template <typename RandomAccessIterator>
SuccessorGenerator<Entry>::SuccessorGenerator(
    RandomAccessIterator domains,
    const std::vector<std::vector<std::pair<int, int>>>& conditions,
    const std::vector<Entry>& values)
{
    assert(conditions.size() == values.size());
    std::vector<SuccessorGeneratorFactoryInfo<Entry>> operator_infos;
    for (unsigned i = 0; i < conditions.size(); i++) {
        assert(std::is_sorted(conditions[i].begin(), conditions[i].end()));
        operator_infos.emplace_back(values[i], conditions[i]);
    }
    root = create(domains, operator_infos);
}

} // namespace successor_generator

#else

#include "successor_generator.h"

#include "global_operator.h"
#include "global_state.h"
#include "globals.h"
#include "probfd/globals.h"
#include "probfd/probabilistic_operator.h"

using namespace std;

namespace successor_generator {

template <>
SuccessorGenerator<const GlobalOperator*>::SuccessorGenerator()
    : root(nullptr)
{
    std::vector<SuccessorGeneratorFactoryInfo<const GlobalOperator*>>
        operator_infos;
    operator_infos.reserve(g_operators.size());
    for (int i = g_operators.size() - 1; i >= 0; i--) {
        const GlobalOperator& op = g_operators[i];
        std::vector<SuccessorGeneratorFactoryKey> precond;
        precond.reserve(op.get_preconditions().size());
        for (unsigned j = 0; j < op.get_preconditions().size(); j++) {
            precond.emplace_back(
                op.get_preconditions()[j].var,
                op.get_preconditions()[j].val);
        }
        std::sort(precond.begin(), precond.end());
        operator_infos.emplace_back(&op, precond);
    }
    root = create(g_variable_domain, operator_infos);
}

template <>
SuccessorGenerator<const probfd::ProbabilisticOperator*>::SuccessorGenerator()
    : root(nullptr)
{
    std::vector<
        SuccessorGeneratorFactoryInfo<const probfd::ProbabilisticOperator*>>
        operator_infos;
    operator_infos.reserve(probfd::g_operators.size());
    for (int i = probfd::g_operators.size() - 1; i >= 0; i--) {
        const probfd::ProbabilisticOperator& op = probfd::g_operators[i];
        const GlobalOperator& out = *(op[0].op);
        std::vector<SuccessorGeneratorFactoryKey> precond;
        precond.reserve(out.get_preconditions().size());
        for (unsigned j = 0; j < out.get_preconditions().size(); j++) {
            precond.emplace_back(
                out.get_preconditions()[j].var,
                out.get_preconditions()[j].val);
        }
        std::sort(precond.begin(), precond.end());
        operator_infos.emplace_back(&op, precond);
    }
    root = create(g_variable_domain, operator_infos);
}

std::shared_ptr<SuccessorGenerator<const GlobalOperator*>>
    g_successor_generator = nullptr;
} // namespace successor_generator

#endif
