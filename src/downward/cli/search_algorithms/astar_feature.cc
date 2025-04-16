#include "downward/cli/plugins/plugin.h"
#include "downward/open_list_factory.h"

#include "downward/cli/search_algorithms/eager_search_options.h"

#include "downward/search_algorithms/eager_search.h"
#include "downward/search_algorithms/search_common.h"

using namespace std;
using namespace downward;
using namespace downward::eager_search;

using namespace downward::cli::eager_search;
using namespace downward::cli::plugins;

namespace {

class AStarSearchFeature : public TypedFeature<SearchAlgorithm, EagerSearch> {
public:
    AStarSearchFeature()
        : TypedFeature("astar")
    {
        document_title("A* search (eager)");
        document_synopsis(
            "A* is a special case of eager best first search that uses g+h "
            "as f-function. "
            "We break ties using the evaluator. Closed nodes are re-opened.");

        add_option<shared_ptr<Evaluator>>("eval", "evaluator for h-value");
        add_option<shared_ptr<Evaluator>>(
            "lazy_evaluator",
            "An evaluator that re-evaluates a state before it is expanded.",
            ArgumentInfo::NO_DEFAULT);
        add_eager_search_options_to_feature(*this, "astar");

        document_note(
            "lazy_evaluator",
            "When a state s is taken out of the open list, the lazy evaluator "
            "h "
            "re-evaluates s. If h(s) changes (for example because h is "
            "path-dependent), "
            "s is not expanded, but instead reinserted into the open list. "
            "This option is currently only present for the A* algorithm.");
        document_note(
            "Equivalent statements using general eager search",
            "\n```\n--search astar(evaluator)\n```\n"
            "is equivalent to\n"
            "```\n--evaluator h=evaluator\n"
            "--search eager(tiebreaking([sum([g(), h]), h], "
            "unsafe_pruning=false),\n"
            "               reopen_closed=true, f_eval=sum([g(), h]))\n"
            "```\n",
            true);
    }

    virtual shared_ptr<EagerSearch>
    create_component(const Options& opts, const utils::Context&) const override
    {
        auto [open, f_eval] =
            search_common::create_astar_open_list_factory_and_f_eval(
                opts.get<shared_ptr<Evaluator>>("eval"),
                opts.get<utils::Verbosity>("verbosity"));

        return make_shared_from_arg_tuples<EagerSearch>(
            open->create_state_open_list(),
            true,
            f_eval,
            vector<shared_ptr<Evaluator>>{},
            opts.get<shared_ptr<Evaluator>>("lazy_evaluator", nullptr),
            get_eager_search_arguments_from_options(opts));
    }
};

FeaturePlugin<AStarSearchFeature> _plugin;

} // namespace
