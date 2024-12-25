#include "downward/cli/plugins/plugin.h"

#include "downward/cli/heuristic_options.h"

#include "downward/heuristics/ff_heuristic.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace ff_heuristic;
using namespace utils;

using namespace downward::cli::plugins;

using downward::cli::add_heuristic_options_to_feature;
using downward::cli::get_heuristic_arguments_from_options;

namespace {

class FFHeuristicFeature : public TypedFeature<Evaluator, FFHeuristic> {
public:
    FFHeuristicFeature()
        : TypedFeature("ff")
    {
        document_title("FF heuristic");

        add_heuristic_options_to_feature(*this, "ff");

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

    virtual shared_ptr<FFHeuristic>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<FFHeuristic>(
            get_heuristic_arguments_from_options(opts));
    }
};

FeaturePlugin<FFHeuristicFeature> _plugin;

} // namespace
