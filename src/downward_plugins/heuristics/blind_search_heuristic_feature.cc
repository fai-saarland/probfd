#include "downward_plugins/plugins/plugin.h"

#include "downward_plugins/heuristic_options.h"

#include "downward/heuristics/blind_search_heuristic.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace utils;
using namespace blind_search_heuristic;

using namespace downward_plugins::plugins;

using downward_plugins::add_heuristic_options_to_feature;
using downward_plugins::get_heuristic_arguments_from_options;

namespace {

class BlindSearchHeuristicFeature
    : public TypedFeature<Evaluator, BlindSearchHeuristic> {
public:
    BlindSearchHeuristicFeature()
        : TypedFeature("blind")
    {
        document_title("Blind heuristic");
        document_synopsis(
            "Returns cost of cheapest action for non-goal states, "
            "0 for goal states");

        add_heuristic_options_to_feature(*this, "blind");

        document_language_support("action costs", "supported");
        document_language_support("conditional effects", "supported");
        document_language_support("axioms", "supported");

        document_property("admissible", "yes");
        document_property("consistent", "yes");
        document_property("safe", "yes");
        document_property("preferred operators", "no");
    }

    virtual shared_ptr<BlindSearchHeuristic>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<BlindSearchHeuristic>(
            get_heuristic_arguments_from_options(opts));
    }
};

FeaturePlugin<BlindSearchHeuristicFeature> _plugin;

} // namespace
