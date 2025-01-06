#include "downward/cli/plugins/plugin.h"

#include "downward/cli/heuristic_options.h"

#include "downward/cli/pdbs/canonical_pdbs_heuristic_options.h"

#include "downward/pdbs/canonical_pdbs_heuristic.h"

using namespace std;
using namespace utils;
using namespace pdbs;

using namespace downward::cli::plugins;

using downward::cli::add_heuristic_options_to_feature;
using downward::cli::get_heuristic_arguments_from_options;

using downward::cli::pdbs::add_canonical_pdbs_options_to_feature;
using downward::cli::pdbs::get_canonical_pdbs_arguments_from_options;

namespace {

class CanonicalPDBsHeuristicFeature
    : public TypedFeature<Evaluator, CanonicalPDBsHeuristic> {
public:
    CanonicalPDBsHeuristicFeature()
        : TypedFeature("cpdbs")
    {
        document_subcategory("heuristics_pdb");
        document_title("Canonical PDB");
        document_synopsis(
            "The canonical pattern database heuristic is calculated as "
            "follows. "
            "For a given pattern collection C, the value of the "
            "canonical heuristic function is the maximum over all "
            "maximal additive subsets A in C, where the value for one subset "
            "S in A is the sum of the heuristic values for all patterns in S "
            "for a given state.");

        add_option<shared_ptr<PatternCollectionGenerator>>(
            "patterns",
            "pattern generation method",
            "systematic(1)");
        add_canonical_pdbs_options_to_feature(*this);
        add_heuristic_options_to_feature(*this, "cpdbs");

        document_language_support("action costs", "supported");
        document_language_support("conditional effects", "not supported");
        document_language_support("axioms", "not supported");

        document_property("admissible", "yes");
        document_property("consistent", "yes");
        document_property("safe", "yes");
        document_property("preferred operators", "no");
    }

    virtual shared_ptr<CanonicalPDBsHeuristic>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<CanonicalPDBsHeuristic>(
            opts.get<shared_ptr<PatternCollectionGenerator>>("patterns"),
            get_canonical_pdbs_arguments_from_options(opts),
            get_heuristic_arguments_from_options(opts));
    }
};

FeaturePlugin<CanonicalPDBsHeuristicFeature> _plugin;

} // namespace
