#include "utils.h"
#include "../../../globals.h"
#include "../../globals.h"

#include "expected_cost/expcost_projection.h"
#include "maxprob/maxprob_projection.h"

#include <sstream>

namespace probabilistic {
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

void dump_pattern(std::ostream& out, PatternID i, const Pattern& p)
{
    dump_pattern_short(out, i, p);

    out << " ({";
    for (unsigned j = 0; j < p.size(); j++) {
        out << (j > 0 ? ", " : "") << ::g_fact_names[p[j]][0];
    }
    out << "})" << std::flush;
}

template value_type::value_t
evaluate_subcollection<expected_cost::ExpCostProjection>(
    const std::vector<value_type::value_t>& pdb_estimates,
    const std::vector<int>& subcollection);

template value_type::value_t evaluate_subcollection<maxprob::MaxProbProjection>(
    const std::vector<value_type::value_t>& pdb_estimates,
    const std::vector<int>& subcollection);

template value_type::value_t combine<expected_cost::ExpCostProjection>(
    value_type::value_t left,
    value_type::value_t right);

template value_type::value_t combine<maxprob::MaxProbProjection>(
    value_type::value_t left,
    value_type::value_t right);

template EvaluationResult evaluate<expected_cost::ExpCostProjection>(
    const PPDBCollection<expected_cost::ExpCostProjection>& database,
    const std::vector<PatternSubCollection>& subcollections,
    const GlobalState& state);

template EvaluationResult evaluate<maxprob::MaxProbProjection>(
    const PPDBCollection<maxprob::MaxProbProjection>& database,
    const std::vector<PatternSubCollection>& subcollections,
    const GlobalState& state);

} // namespace pdbs
} // namespace probabilistic