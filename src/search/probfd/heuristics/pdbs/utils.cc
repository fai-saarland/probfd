#include "probfd/heuristics/pdbs/utils.h"

#include "probfd/heuristics/pdbs/maxprob_pattern_database.h"
#include "probfd/heuristics/pdbs/ssp_pattern_database.h"


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

template value_t evaluate_subcollection<SSPPatternDatabase>(
    const std::vector<value_t>& pdb_estimates,
    const std::vector<int>& subcollection);

template value_t evaluate_subcollection<MaxProbPatternDatabase>(
    const std::vector<value_t>& pdb_estimates,
    const std::vector<int>& subcollection);

template value_t combine<SSPPatternDatabase>(value_t left, value_t right);

template value_t combine<MaxProbPatternDatabase>(value_t left, value_t right);

template EvaluationResult evaluate<SSPPatternDatabase>(
    const PPDBCollection<SSPPatternDatabase>& database,
    const std::vector<PatternSubCollection>& subcollections,
    const State& state);

template EvaluationResult evaluate<MaxProbPatternDatabase>(
    const PPDBCollection<MaxProbPatternDatabase>& database,
    const std::vector<PatternSubCollection>& subcollections,
    const State& state);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd