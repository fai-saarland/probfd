#ifndef LANDMARKS_LANDMARK_FACTORY_H
#define LANDMARKS_LANDMARK_FACTORY_H

#include "downward/landmarks/landmark_graph.h"

#include "downward/utils/logging.h"

#include "downward/abstract_task.h"
#include "downward/fact_pair.h"

#include <list>
#include <map>
#include <memory>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace downward {
class OperatorProxy;
}

namespace downward::landmarks {

class LandmarkFactory {
    AbstractTaskPtrTuple lm_graph_task;
    std::vector<std::vector<std::vector<int>>> operators_eff_lookup;

protected:
    mutable utils::LogProxy log;
    std::shared_ptr<LandmarkGraph> lm_graph;
    bool achievers_calculated = false;

public:
    LandmarkFactory(const LandmarkFactory&) = delete;

    virtual ~LandmarkFactory() = default;

    std::shared_ptr<LandmarkGraph>
    compute_lm_graph(const SharedAbstractTask& task);

    virtual bool supports_conditional_effects() const = 0;

    bool achievers_are_calculated() const { return achievers_calculated; }

protected:
    explicit LandmarkFactory(utils::Verbosity verbosity);

    void edge_add(LandmarkNode& from, LandmarkNode& to, EdgeType type) const;

    void discard_all_orderings() const;

    bool
    is_landmark_precondition(const OperatorProxy& op, const Landmark& landmark)
        const;

    const std::vector<int>&
    get_operators_including_eff(const FactPair& eff) const
    {
        return operators_eff_lookup[eff.var][eff.value];
    }

private:
    virtual void generate_landmarks(const SharedAbstractTask& task) = 0;

    void generate_operators_lookups(
        const VariableSpace& variables,
        const AxiomSpace& axioms,
        const ClassicalOperatorSpace& operators);
};

} // namespace downward::landmarks

#endif
