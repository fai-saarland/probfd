#include "downward/cli/plugins/plugin.h"

#include "downward/cli/heuristic_options.h"

#include "downward/heuristics/goal_count_heuristic.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward::goal_count_heuristic;
using namespace downward::utils;

using namespace downward::cli::plugins;

using downward::cli::add_heuristic_options_to_feature;
using downward::cli::get_heuristic_arguments_from_options;

namespace {

class GoalCountHeuristicFeature
    : public TypedFeature<downward::Evaluator, GoalCountHeuristic> {
public:
    GoalCountHeuristicFeature()
        : TypedFeature("goalcount")
    {
        document_title("Goal count heuristic");

        add_heuristic_options_to_feature(*this, "goalcount");

        document_language_support("action costs", "ignored by design");
        document_language_support("conditional effects", "supported");
        document_language_support("axioms", "supported");

        document_property("admissible", "no");
        document_property("consistent", "no");
        document_property("safe", "yes");
        document_property("preferred operators", "no");
    }

    virtual shared_ptr<GoalCountHeuristic>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<GoalCountHeuristic>(
            get_heuristic_arguments_from_options(opts));
    }
};

FeaturePlugin<GoalCountHeuristicFeature> _plugin;

} // namespace
