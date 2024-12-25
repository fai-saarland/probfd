#include "downward/cli/plugins/plugin.h"

#include "downward/cli/heuristic_options.h"

#include "downward/heuristics/lm_cut_heuristic.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace lm_cut_heuristic;
using namespace utils;

using namespace downward::cli::plugins;

using downward::cli::add_heuristic_options_to_feature;
using downward::cli::get_heuristic_arguments_from_options;

namespace {

class LandmarkCutHeuristicFeature
    : public TypedFeature<Evaluator, LandmarkCutHeuristic> {
public:
    LandmarkCutHeuristicFeature()
        : TypedFeature("lmcut")
    {
        document_title("Landmark-cut heuristic");

        add_heuristic_options_to_feature(*this, "lmcut");

        document_language_support("action costs", "supported");
        document_language_support("conditional effects", "not supported");
        document_language_support("axioms", "not supported");

        document_property("admissible", "yes");
        document_property("consistent", "no");
        document_property("safe", "yes");
        document_property("preferred operators", "no");
    }

    virtual shared_ptr<LandmarkCutHeuristic>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<LandmarkCutHeuristic>(
            get_heuristic_arguments_from_options(opts));
    }
};

FeaturePlugin<LandmarkCutHeuristicFeature> _plugin;

} // namespace
