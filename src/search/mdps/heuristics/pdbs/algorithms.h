#pragma once

#include "../../algorithms/graph_visualization.h"
#include "../../algorithms/interval_iteration.h"
#include "../../algorithms/topological_value_iteration.h"
#include "abstract_state.h"
#include "types.h"

namespace probabilistic {
namespace pdbs {

using ValueIteration = algorithms::topological_vi::
    TopologicalValueIteration<AbstractState, const AbstractOperator*, true>;

using IntervalIteration = algorithms::interval_iteration::
    IntervalIteration<AbstractState, const AbstractOperator*, true>;

template<typename StateToString = AbstractStateToString>
using ProjectionGraphVis = algorithms::graphviz::GraphVisualization<
    AbstractState,
    const AbstractOperator*,
    StateToString,
    AbstractOperatorToString>;

} // namespace pdbs
} // namespace probabilistic

