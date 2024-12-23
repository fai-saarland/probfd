#include "downward_plugins/plugins/plugin.h"

#include "downward_plugins/heuristic_options.h"

#include "downward/pdbs/pdb_heuristic.h"

using namespace std;
using namespace utils;
using namespace pdbs;

using namespace downward_plugins::plugins;

using downward_plugins::add_heuristic_options_to_feature;
using downward_plugins::get_heuristic_arguments_from_options;

namespace {

class PDBHeuristicFeature : public TypedFeature<Evaluator, PDBHeuristic> {
public:
    PDBHeuristicFeature()
        : TypedFeature("pdb")
    {
        document_subcategory("heuristics_pdb");
        document_title("Pattern database heuristic");
        document_synopsis("TODO");

        add_option<shared_ptr<PatternGenerator>>(
            "pattern",
            "pattern generation method",
            "greedy()");
        add_heuristic_options_to_feature(*this, "pdb");

        document_language_support("action costs", "supported");
        document_language_support("conditional effects", "not supported");
        document_language_support("axioms", "not supported");

        document_property("admissible", "yes");
        document_property("consistent", "yes");
        document_property("safe", "yes");
        document_property("preferred operators", "no");
    }

    virtual shared_ptr<PDBHeuristic>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<PDBHeuristic>(
            opts.get<shared_ptr<PatternGenerator>>("pattern"),
            get_heuristic_arguments_from_options(opts));
    }
};

FeaturePlugin<PDBHeuristicFeature> _plugin;

} // namespace
