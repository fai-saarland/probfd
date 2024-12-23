#include "downward_plugins/plugins/plugin.h"

#include "downward_plugins/heuristic_options.h"

#include "downward/heuristics/max_heuristic.h"

using namespace std;
using namespace max_heuristic;
using namespace utils;

using namespace downward_plugins::plugins;

using downward_plugins::add_heuristic_options_to_feature;
using downward_plugins::get_heuristic_arguments_from_options;

namespace {

class HSPMaxHeuristicFeature : public TypedFeature<Evaluator, HSPMaxHeuristic> {
public:
    HSPMaxHeuristicFeature()
        : TypedFeature("hmax")
    {
        document_title("Max heuristic");

        add_heuristic_options_to_feature(*this, "hmax");

        document_language_support("action costs", "supported");
        document_language_support("conditional effects", "supported");
        document_language_support(
            "axioms",
            "supported (in the sense that the planner won't complain -- "
            "handling of axioms might be very stupid "
            "and even render the heuristic unsafe)");

        document_property("admissible", "yes for tasks without axioms");
        document_property("consistent", "yes for tasks without axioms");
        document_property("safe", "yes for tasks without axioms");
        document_property("preferred operators", "no");
    }

    virtual shared_ptr<HSPMaxHeuristic>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<HSPMaxHeuristic>(
            get_heuristic_arguments_from_options(opts));
    }
};

FeaturePlugin<HSPMaxHeuristicFeature> _plugin;

} // namespace
