#include "probfd/heuristics/pdbs/utils.h"

#include "probfd/heuristics/pdbs/expcost_projection.h"
#include "probfd/heuristics/pdbs/maxprob_projection.h"

#include <sstream>

namespace probfd {
namespace heuristics {
namespace pdbs {

void dump_pattern_vars(std::ostream& out, const Pattern& p)
{
    out << "[";
    for (unsigned j = 0; j < p.size(); j++) {
        out << (j > 0 ? ", " : "") << p[j];
    }
    out << "]";
}

void dump_pattern_short(std::ostream& out, PatternID i, const Pattern& p)
{
    out << "pattern[" << i << "]: vars = ";
    dump_pattern_vars(out, p);
}

template value_t evaluate_subcollection<ExpCostProjection>(
    const std::vector<value_t>& pdb_estimates,
    const std::vector<int>& subcollection);

template value_t evaluate_subcollection<MaxProbProjection>(
    const std::vector<value_t>& pdb_estimates,
    const std::vector<int>& subcollection);

template value_t combine<ExpCostProjection>(value_t left, value_t right);

template value_t combine<MaxProbProjection>(value_t left, value_t right);

template EvaluationResult evaluate<ExpCostProjection>(
    const PPDBCollection<ExpCostProjection>& database,
    const std::vector<PatternSubCollection>& subcollections,
    const State& state);

template EvaluationResult evaluate<MaxProbProjection>(
    const PPDBCollection<MaxProbProjection>& database,
    const std::vector<PatternSubCollection>& subcollections,
    const State& state);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd