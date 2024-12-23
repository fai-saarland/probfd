#include "downward_plugins/plugins/plugin.h"

#include "downward_plugins/heuristic_options.h"

#include "downward/pdbs/zero_one_pdbs_heuristic.h"

using namespace std;
using namespace utils;
using namespace pdbs;

using namespace downward_plugins::plugins;

using downward_plugins::add_heuristic_options_to_feature;
using downward_plugins::get_heuristic_arguments_from_options;

namespace {

class ZeroOnePDBsHeuristicFeature
    : public TypedFeature<Evaluator, ZeroOnePDBsHeuristic> {
public:
    ZeroOnePDBsHeuristicFeature()
        : TypedFeature("zopdbs")
    {
        document_subcategory("heuristics_pdb");
        document_title("Zero-One PDB");
        document_synopsis(
            "The zero/one pattern database heuristic is simply the sum of the "
            "heuristic values of all patterns in the pattern collection. In "
            "contrast "
            "to the canonical pattern database heuristic, there is no need to "
            "check "
            "for additive subsets, because the additivity of the patterns is "
            "guaranteed by action cost partitioning. This heuristic uses the "
            "most "
            "simple form of action cost partitioning, i.e. if an operator "
            "affects "
            "more than one pattern in the collection, its costs are entirely "
            "taken "
            "into account for one pattern (the first one which it affects) and "
            "set "
            "to zero for all other affected patterns.");

        add_option<shared_ptr<PatternCollectionGenerator>>(
            "patterns",
            "pattern generation method",
            "systematic(1)");
        add_heuristic_options_to_feature(*this, "zopdbs");

        document_language_support("action costs", "supported");
        document_language_support("conditional effects", "not supported");
        document_language_support("axioms", "not supported");

        document_property("admissible", "yes");
        document_property("consistent", "yes");
        document_property("safe", "yes");
        document_property("preferred operators", "no");
    }

    virtual shared_ptr<ZeroOnePDBsHeuristic>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<ZeroOnePDBsHeuristic>(
            opts.get<shared_ptr<PatternCollectionGenerator>>("patterns"),
            get_heuristic_arguments_from_options(opts));
    }
};

FeaturePlugin<ZeroOnePDBsHeuristicFeature> _plugin;

} // namespace
