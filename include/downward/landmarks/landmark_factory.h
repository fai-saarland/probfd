#ifndef LANDMARKS_LANDMARK_FACTORY_H
#define LANDMARKS_LANDMARK_FACTORY_H

#include "downward/landmarks/landmark_graph.h"

#include "downward/utils/logging.h"

#include "downward/fact_pair.h"

#include <list>
#include <map>
#include <memory>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace downward {
class AbstractTask;
class OperatorProxy;
}

namespace downward::landmarks {
/*
  TODO: Change order to private -> protected -> public
   (omitted so far to minimize diff)
*/
class LandmarkFactory {
public:
    virtual ~LandmarkFactory() = default;
    LandmarkFactory(const LandmarkFactory&) = delete;

    std::shared_ptr<LandmarkGraph>
    compute_lm_graph(const std::shared_ptr<AbstractTask>& task);

    virtual bool supports_conditional_effects() const = 0;

    bool achievers_are_calculated() const { return achievers_calculated; }

protected:
    explicit LandmarkFactory(utils::Verbosity verbosity);
    mutable utils::LogProxy log;
    std::shared_ptr<LandmarkGraph> lm_graph;
    bool achievers_calculated = false;

    void edge_add(LandmarkNode& from, LandmarkNode& to, EdgeType type);

    void discard_all_orderings();

    bool
    is_landmark_precondition(const OperatorProxy& op, const Landmark& landmark)
        const;

    const std::vector<int>&
    get_operators_including_eff(const FactPair& eff) const
    {
        return operators_eff_lookup[eff.var][eff.value];
    }

private:
    AbstractTask* lm_graph_task;
    std::vector<std::vector<std::vector<int>>> operators_eff_lookup;

    virtual void
    generate_landmarks(const std::shared_ptr<AbstractTask>& task) = 0;
    void generate_operators_lookups(const AbstractTask& task);
};

} // namespace landmarks

#endif
