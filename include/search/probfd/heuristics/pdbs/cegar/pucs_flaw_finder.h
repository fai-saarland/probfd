#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_PUCS_FLAW_FINDER_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_PUCS_FLAW_FINDER_H

#include "probfd/heuristics/pdbs/cegar/flaw_finding_strategy.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/value_type.h"

#include "algorithms/priority_queues.h"

#include "task_proxy.h"

#include <stack>
#include <unordered_map>

class StateID;
class State;
class StateRegistry;

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

template <typename PDBType>
class PUCSFlawFinder : public FlawFindingStrategy<PDBType> {
    priority_queues::HeapQueue<value_t, State> pq;
    storage::PerStateStorage<value_t> probabilities;

    unsigned int violation_threshold;

public:
    PUCSFlawFinder(options::Options& opts);
    PUCSFlawFinder(unsigned int violation_threshold);
    ~PUCSFlawFinder() override = default;

    virtual bool apply_policy(
        PatternCollectionGeneratorCegar<PDBType>& base,
        const ProbabilisticTaskProxy& task_proxy,
        int solution_index,
        std::vector<Flaw>& flaw_list) override;

    virtual std::string get_name() override;

private:
    bool expand(
        PatternCollectionGeneratorCegar<PDBType>& base,
        const ProbabilisticTaskProxy& task_proxy,
        int solution_index,
        State state,
        value_t priority,
        std::vector<Flaw>& flaw_list,
        StateRegistry& registry);
};

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif