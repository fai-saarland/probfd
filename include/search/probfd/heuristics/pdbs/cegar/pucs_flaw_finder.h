#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_PUCS_FLAW_FINDER_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_PUCS_FLAW_FINDER_H

#include "probfd/heuristics/pdbs/cegar/flaw_finding_strategy.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/value_type.h"

#include "downward/algorithms/priority_queues.h"

#include "downward/task_proxy.h"

class StateID;
class State;
class StateRegistry;

namespace plugins {
class Options;
} // namespace plugins

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

class PUCSFlawFinder : public FlawFindingStrategy {
    struct ExpansionInfo {
        bool expanded = false;
        value_t path_probability = 0_vt;
    };

    priority_queues::HeapQueue<value_t, State> pq;
    storage::PerStateStorage<ExpansionInfo> probabilities;

    const int max_search_states;

public:
    explicit PUCSFlawFinder(const plugins::Options& opts);
    explicit PUCSFlawFinder(int max_search_states);

    ~PUCSFlawFinder() override = default;

    bool apply_policy(
        const ProbabilisticTaskProxy& task_proxy,
        const PDBInfo& pdb_info,
        const std::unordered_set<int>& blacklisted_variables,
        std::vector<Flaw>& flaw_list,
        utils::CountdownTimer& timer) override;

    std::string get_name() override;
};

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif