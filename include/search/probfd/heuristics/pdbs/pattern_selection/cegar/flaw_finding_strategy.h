#ifndef MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_FLAW_FINDER_H
#define MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_FLAW_FINDER_H

#include "probfd/heuristics/pdbs/pattern_selection/cegar/types.h"

#include "probfd/task_proxy.h"

#include <string>

namespace probfd {
namespace heuristics {
namespace pdbs {

class AbstractPolicy;

namespace pattern_selection {

template <typename PDBType>
class PatternCollectionGeneratorCegar;

template <typename PDBType>
class FlawFindingStrategy {
protected:
    const ProbabilisticTask* task;
    ProbabilisticTaskProxy task_proxy;

    struct StateHash {
        std::vector<int> multipliers;

        StateHash(const ProbabilisticTaskProxy& task_proxy)
        {
            VariablesProxy variables = task_proxy.get_variables();
            multipliers.reserve(variables.size());
            int multiplier = 1;
            for (VariableProxy var : task_proxy.get_variables()) {
                multipliers.push_back(multiplier);
                multiplier *= var.get_domain_size();
            }
        }

        size_t operator()(const std::vector<int>& state) const
        {
            std::size_t res = 0;
            for (size_t i = 0; i != state.size(); ++i) {
                res += multipliers[i] * state[i];
            }
            return res;
        }
    };

public:
    FlawFindingStrategy(const ProbabilisticTask* task);
    virtual ~FlawFindingStrategy() = default;

    // Second value returns whether policy is executable (modulo blacklisting)
    // Note that the flaw list might be empty regardless since only remaining
    // goals are added to the list for goal violations.
    virtual std::pair<FlawList, bool> apply_policy(
        PatternCollectionGeneratorCegar<PDBType>& base,
        int solution_index,
        std::vector<int>& state) = 0;

protected:
    static std::vector<int> apply_op_to_state(
        std::vector<int> state,
        const ProbabilisticOutcomeProxy& op)
    {
        for (ProbabilisticEffectProxy effect : op.get_effects()) {
            FactPair effect_fact = effect.get_fact().get_pair();
            state[effect_fact.var] = effect_fact.value;
        }

        return state;
    }
};

} // namespace pattern_selection
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif