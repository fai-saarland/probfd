#include "downward/cartesian_abstractions/utils_landmarks.h"

#include "downward/landmarks/landmark.h"
#include "downward/landmarks/landmark_factory_h_m.h"
#include "downward/landmarks/landmark_graph.h"

#include "downward/utils/logging.h"
#include "downward/utils/memory.h"

#include <algorithm>

using namespace std;
using namespace downward::landmarks;

namespace downward::cartesian_abstractions {
static FactPair get_fact(const Landmark& landmark)
{
    // We assume that the given Landmarks are from an h^m landmark graph with
    // m=1.
    assert(landmark.facts.size() == 1);
    return landmark.facts[0];
}

shared_ptr<LandmarkGraph> get_landmark_graph(
    const shared_ptr<AbstractTask>& task,
    std::shared_ptr<MutexFactory> mutex_factory)
{
    LandmarkFactoryHM lm_graph_factory(
        std::move(mutex_factory),
        1,
        false,
        true,
        utils::Verbosity::SILENT);

    return lm_graph_factory.compute_lm_graph(task);
}

vector<FactPair> get_fact_landmarks(const LandmarkGraph& graph)
{
    vector<FactPair> facts;
    const LandmarkGraph::Nodes& nodes = graph.get_nodes();
    facts.reserve(nodes.size());
    for (auto& node : nodes) {
        facts.push_back(get_fact(node->get_landmark()));
    }
    sort(facts.begin(), facts.end());
    return facts;
}

utils::HashMap<FactPair, LandmarkNode*>
get_fact_to_landmark_map(const shared_ptr<LandmarkGraph>& graph)
{
    const LandmarkGraph::Nodes& nodes = graph->get_nodes();
    // All landmarks are simple, i.e., each has exactly one fact.
    assert(all_of(nodes.begin(), nodes.end(), [](auto& node) {
        return node->get_landmark().facts.size() == 1;
    }));
    utils::HashMap<FactPair, landmarks::LandmarkNode*> fact_to_landmark_map;
    for (auto& node : nodes) {
        const FactPair& fact = node->get_landmark().facts[0];
        fact_to_landmark_map[fact] = node.get();
    }
    return fact_to_landmark_map;
}

VarToValues get_prev_landmarks(const LandmarkNode* node)
{
    VarToValues groups;
    vector<const LandmarkNode*> open;
    unordered_set<const LandmarkNode*> closed;
    for (const auto& parent_and_edge : node->parents) {
        const LandmarkNode* parent = parent_and_edge.first;
        open.push_back(parent);
    }
    while (!open.empty()) {
        const LandmarkNode* ancestor = open.back();
        open.pop_back();
        if (closed.find(ancestor) != closed.end()) continue;
        closed.insert(ancestor);
        FactPair ancestor_fact = get_fact(ancestor->get_landmark());
        groups[ancestor_fact.var].push_back(ancestor_fact.value);
        for (const auto& parent_and_edge : ancestor->parents) {
            const LandmarkNode* parent = parent_and_edge.first;
            open.push_back(parent);
        }
    }
    return groups;
}
} // namespace cartesian_abstractions
