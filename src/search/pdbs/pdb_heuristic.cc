#include "pdbs/pdb_heuristic.h"

#include "pdbs/pattern_database.h"
#include "pdbs/pattern_generator.h"

#include "option_parser.h"
#include "plugin.h"
#include "operator_cost.h"

#include <limits>
#include <memory>

using namespace std;

namespace pdbs {
shared_ptr<PatternDatabase> get_pdb_from_options(const Options &opts) {
    shared_ptr<PatternGenerator> pattern_generator =
        opts.get<shared_ptr<PatternGenerator>>("pattern");
    PatternInformation pattern_info = pattern_generator->generate(OperatorCost(opts.get_enum("cost_type")));
    return pattern_info.get_pdb();
}

PDBHeuristic::PDBHeuristic(const Options &opts)
    : Heuristic(opts),
      pdb(get_pdb_from_options(opts)) {
}

int PDBHeuristic::compute_heuristic(const GlobalState &state) {
    int h = pdb->get_value(state);
    if (h == numeric_limits<int>::max())
        return DEAD_END;
    return h;
}

static shared_ptr<Heuristic> _parse(OptionParser &parser) {
    parser.document_synopsis("Pattern database heuristic", "TODO");
    parser.document_language_support("action costs", "supported");
    parser.document_language_support("conditional effects", "not supported");
    parser.document_language_support("axioms", "not supported");
    parser.document_property("admissible", "yes");
    parser.document_property("consistent", "yes");
    parser.document_property("safe", "yes");
    parser.document_property("preferred operators", "no");

    parser.add_option<shared_ptr<PatternGenerator>>(
        "pattern",
        "pattern generation method",
        "greedy()");
    Heuristic::add_options_to_parser(parser);

    Options opts = parser.parse();
    if (parser.dry_run())
        return nullptr;

    return make_shared<PDBHeuristic>(opts);
}

static Plugin<Heuristic> _plugin("pdb", _parse);
}
