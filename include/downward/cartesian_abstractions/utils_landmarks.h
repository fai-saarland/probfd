#ifndef CEGAR_UTILS_LANDMARKS_H
#define CEGAR_UTILS_LANDMARKS_H

#include "downward/task_dependent_factory_fwd.h"

#include "downward/utils/hash.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace downward {
class AbstractTask;
struct FactPair;
class MutexInformation;
}

namespace downward::landmarks {
class LandmarkGraph;
class LandmarkNode;
} // namespace landmarks

namespace downward::cartesian_abstractions {
using VarToValues = std::unordered_map<int, std::vector<int>>;

extern std::shared_ptr<landmarks::LandmarkGraph> get_landmark_graph(
    const std::shared_ptr<AbstractTask>& task,
    std::shared_ptr<TaskDependentFactory<MutexInformation>> mutex_factory);
extern std::vector<FactPair>
get_fact_landmarks(const landmarks::LandmarkGraph& graph);

extern utils::HashMap<FactPair, landmarks::LandmarkNode*>
get_fact_to_landmark_map(
    const std::shared_ptr<landmarks::LandmarkGraph>& graph);

/*
  Do a breadth-first search through the landmark graph ignoring
  duplicates. Start at the given node and collect for each variable the
  facts that have to be made true before the given node can be true for
  the first time.
*/
extern VarToValues get_prev_landmarks(const landmarks::LandmarkNode* node);
} // namespace cartesian_abstractions

#endif
