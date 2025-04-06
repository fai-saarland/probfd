#include "downward/cli/plugins/plugin.h"

#include "downward/cli/heuristic_options.h"

#include "downward/heuristics/cg_heuristic.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward::cg_heuristic;
using namespace downward::utils;

using namespace downward::cli::plugins;

using downward::cli::add_heuristic_options_to_feature;
using downward::cli::get_heuristic_arguments_from_options;

namespace {

class CGHeuristicFeature
    : public TypedFeature<downward::Evaluator, CGHeuristic> {
public:
    CGHeuristicFeature()
        : TypedFeature("cg")
    {
        document_title("Causal graph heuristic");

        add_option<int>(
            "max_cache_size",
            "maximum number of cached entries per variable (set to 0 to "
            "disable cache)",
            "1000000",
            Bounds("0", "infinity"));
        add_heuristic_options_to_feature(*this, "cg");

        document_language_support("action costs", "supported");
        document_language_support("conditional effects", "supported");
        document_language_support(
            "axioms",
            "supported (in the sense that the planner won't complain -- "
            "handling of axioms might be very stupid "
            "and even render the heuristic unsafe)");

        document_property("admissible", "no");
        document_property("consistent", "no");
        document_property("safe", "no");
        document_property("preferred operators", "yes");
    }

    virtual shared_ptr<CGHeuristic>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<CGHeuristic>(
            opts.get<int>("max_cache_size"),
            get_heuristic_arguments_from_options(opts));
    }
};

FeaturePlugin<CGHeuristicFeature> _plugin;

} // namespace
