#ifndef LANDMARKS_UTIL_H
#define LANDMARKS_UTIL_H

#include <unordered_map>
#include <vector>

namespace downward {
class AxiomOrOperatorProxy;
class OperatorProxy;
class TaskProxy;
}

namespace downward::utils {
class LogProxy;
}

namespace downward::landmarks {
class Landmark;
class LandmarkNode;
class LandmarkGraph;

extern std::unordered_map<int, int> _intersect(
    const std::unordered_map<int, int> &a,
    const std::unordered_map<int, int> &b);

extern bool possibly_reaches_lm(
    const AxiomOrOperatorProxy& op,
    const std::vector<std::vector<bool>>& reached,
    const Landmark& landmark);

extern AxiomOrOperatorProxy
get_operator_or_axiom(const TaskProxy& task_proxy, int op_or_axiom_id);
extern int get_operator_or_axiom_id(const AxiomOrOperatorProxy &op);

extern void dump_landmark_graph(
    const TaskProxy &task_proxy,
    const LandmarkGraph &graph,
    utils::LogProxy &log);
}

#endif
