#include "utils.h"
#include "../../../globals.h"
#include "../../globals.h"
#include "maxprob/maxprob_projection.h"
#include "expected_cost/expcost_projection.h"

#include <sstream>

namespace probabilistic::pdbs {

void dump_pattern_vars(std::ostream& out, const Pattern& p) {
    out << "[";
    for (unsigned j = 0; j < p.size(); j++) {
        out << (j > 0 ? ", " : "") << p[j];
    }
    out << "]";
}

void dump_pattern_short(std::ostream& out, PatternID i, const Pattern& p) {
    out << "pattern[" << i << "]: vars = ";
    dump_pattern_vars(out, p);
}

void dump_pattern(std::ostream& out, PatternID i, const Pattern& p) {
    dump_pattern_short(out, i, p);

    out << " ({";
    for (unsigned j = 0; j < p.size(); j++) {
        out << (j > 0 ? ", " : "") << ::g_fact_names[p[j]][0];
    }
    out << "})" << std::flush;
}

void dump_projection(
    MaxProbProjection& projection,
    const std::string& path,
    bool print_transition_labels,
    bool print_values)
{
    dump_graphviz(
        projection,
        path,
        print_transition_labels,
        print_values);
}

}