#ifndef PROBFD_HEURISTICS_ABSTRACTIONS_TYPES_H
#define PROBFD_HEURISTICS_ABSTRACTIONS_TYPES_H

#include <memory>
#include <vector>

namespace probfd {

template <typename, typename>
class SimpleMDP;

template <typename>
class Evaluator;

template <typename, typename>
class Policy;

template <typename, typename>
class MultiPolicy;

namespace heuristics {
namespace abstractions {

using AbstractStateIndex = int;

template <typename Action>
using AbstractMDP = SimpleMDP<AbstractStateIndex, Action>;

using AbstractionEvaluator = Evaluator<AbstractStateIndex>;

template <typename Action>
using AbstractionCollection = std::vector<std::unique_ptr<AbstractMDP<Action>>>;

template <typename Action>
using AbstractPolicy = Policy<AbstractStateIndex, Action>;

template <typename Action>
using AbstractMultiPolicy = MultiPolicy<AbstractStateIndex, Action>;

} // namespace abstractions
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_ABSTRACTIONS_TYPES_H
