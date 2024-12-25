#include "downward/cli/plugins/plugin.h"

#include "downward/cli/search_algorithm_options.h"

#include "downward/search_algorithms/enforced_hill_climbing_search.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace enforced_hill_climbing_search;

using namespace downward::cli::plugins;

using downward::cli::add_search_algorithm_options_to_feature;
using downward::cli::get_search_algorithm_arguments_from_options;

namespace {

class EnforcedHillClimbingSearchFeature
    : public TypedFeature<SearchAlgorithm, EnforcedHillClimbingSearch> {
public:
    EnforcedHillClimbingSearchFeature()
        : TypedFeature("ehc")
    {
        document_title("Lazy enforced hill-climbing");
        document_synopsis("");

        add_option<shared_ptr<Evaluator>>("h", "heuristic");
        add_option<PreferredUsage>(
            "preferred_usage",
            "preferred operator usage",
            "prune_by_preferred");
        add_list_option<shared_ptr<Evaluator>>(
            "preferred",
            "use preferred operators of these evaluators",
            "[]");
        add_search_algorithm_options_to_feature(*this, "ehc");
    }

    virtual shared_ptr<EnforcedHillClimbingSearch>
    create_component(const Options& opts, const utils::Context&) const override
    {
        return make_shared_from_arg_tuples<EnforcedHillClimbingSearch>(
            opts.get<shared_ptr<Evaluator>>("h"),
            opts.get<PreferredUsage>("preferred_usage"),
            opts.get_list<shared_ptr<Evaluator>>("preferred"),
            get_search_algorithm_arguments_from_options(opts));
    }
};

FeaturePlugin<EnforcedHillClimbingSearchFeature> _plugin;

TypedEnumPlugin<PreferredUsage> _enum_plugin(
    {{"prune_by_preferred",
      "prune successors achieved by non-preferred operators"},
     {"rank_preferred_first",
      "first insert successors achieved by preferred operators, "
      "then those by non-preferred operators"}});

} // namespace
