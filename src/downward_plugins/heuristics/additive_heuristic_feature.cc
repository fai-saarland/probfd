#include "downward_plugins/plugins/plugin.h"

#include "downward_plugins/heuristic_options.h"

#include "downward/heuristics/additive_heuristic.h"

using namespace std;
using namespace utils;
using namespace additive_heuristic;

using namespace downward_plugins::plugins;

using downward_plugins::add_heuristic_options_to_feature;
using downward_plugins::get_heuristic_arguments_from_options;

namespace {

class AdditiveHeuristicFeature
    : public TypedFeature<Evaluator, AdditiveHeuristic> {
public:
    AdditiveHeuristicFeature()
        : TypedFeature("add")
    {
        document_title("Additive heuristic");

        add_heuristic_options_to_feature(*this, "add");

        document_language_support("action costs", "supported");
        document_language_support("conditional effects", "supported");
        document_language_support(
            "axioms",
            "supported (in the sense that the planner won't complain -- "
            "handling of axioms might be very stupid "
            "and even render the heuristic unsafe)");

        document_property("admissible", "no");
        document_property("consistent", "no");
        document_property("safe", "yes for tasks without axioms");
        document_property("preferred operators", "yes");
    }

    virtual shared_ptr<AdditiveHeuristic>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<AdditiveHeuristic>(
            get_heuristic_arguments_from_options(opts));
    }
};

FeaturePlugin<AdditiveHeuristicFeature> _plugin;

} // namespace
