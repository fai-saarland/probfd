#include "downward/pdbs/pdb_heuristic.h"

#include "downward/pdbs/pattern_database.h"

#include "downward/plugins/plugin.h"

#include <limits>
#include <memory>

using namespace std;

namespace pdbs {
static shared_ptr<PatternDatabase> get_pdb_from_generator(
    const shared_ptr<AbstractTask>& task,
    const shared_ptr<PatternGenerator>& pattern_generator)
{
    PatternInformation pattern_info = pattern_generator->generate(task);
    return pattern_info.get_pdb();
}

PDBHeuristic::PDBHeuristic(
    const shared_ptr<PatternGenerator>& pattern,
    const shared_ptr<AbstractTask>& transform,
    bool cache_estimates,
    const string& description,
    utils::Verbosity verbosity)
    : Heuristic(transform, cache_estimates, description, verbosity)
    , pdb(get_pdb_from_generator(task, pattern))
{
}

int PDBHeuristic::compute_heuristic(const State& ancestor_state)
{
    State state = convert_ancestor_state(ancestor_state);
    int h = pdb->get_value(state.get_unpacked_values());
    if (h == numeric_limits<int>::max()) return DEAD_END;
    return h;
}

class PDBHeuristicFeature
    : public plugins::TypedFeature<Evaluator, PDBHeuristic> {
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
    create_component(const plugins::Options& opts, const utils::Context&)
        const override
    {
        return plugins::make_shared_from_arg_tuples<PDBHeuristic>(
            opts.get<shared_ptr<PatternGenerator>>("pattern"),
            get_heuristic_arguments_from_options(opts));
    }
};

static plugins::FeaturePlugin<PDBHeuristicFeature> _plugin;
} // namespace pdbs
