#include "downward/cli/plugins/plugin.h"

#include "downward/cli/search_algorithms/eager_search_options.h"

#include "downward/open_list_factory.h"

#include "downward/search_algorithms/eager_search.h"
#include "downward/search_algorithms/search_common.h"

using namespace std;
using namespace downward;
using namespace downward::eager_search;

using namespace downward::cli::eager_search;
using namespace downward::cli::plugins;

namespace {

class EagerWAstarSearchFeature
    : public TypedFeature<SearchAlgorithm, EagerSearch> {
public:
    EagerWAstarSearchFeature()
        : TypedFeature("eager_wastar")
    {
        document_title("Eager weighted A* search");
        document_synopsis("");

        add_list_option<shared_ptr<Evaluator>>("evals", "evaluators");
        add_list_option<shared_ptr<Evaluator>>(
            "preferred",
            "use preferred operators of these evaluators",
            "[]");
        add_option<bool>("reopen_closed", "reopen closed nodes", "true");
        add_option<int>(
            "boost",
            "boost value for preferred operator open lists",
            "0");
        add_option<int>("w", "evaluator weight", "1");
        add_eager_search_options_to_feature(*this, "eager_wastar");

        document_note(
            "Open lists and equivalent statements using general eager search",
            "See corresponding notes for \"(Weighted) A* search (lazy)\"");
        document_note(
            "Note",
            "Eager weighted A* search uses an alternation open list "
            "while A* search uses a tie-breaking open list. Consequently, "
            "\n```\n--search eager_wastar([h()], w=1)\n```\n"
            "is **not** equivalent to\n```\n--search astar(h())\n```\n");
    }

    virtual shared_ptr<EagerSearch>
    create_component(const Options& opts, const utils::Context&) const override
    {
        auto open = search_common::create_wastar_open_list_factory(
            opts.get_list<shared_ptr<Evaluator>>("evals"),
            opts.get_list<shared_ptr<Evaluator>>("preferred"),
            opts.get<int>("boost"),
            opts.get<int>("w"),
            opts.get<utils::Verbosity>("verbosity"));

        return make_shared_from_arg_tuples<EagerSearch>(
            open->create_state_open_list(),
            opts.get<bool>("reopen_closed"),
            nullptr,
            opts.get_list<shared_ptr<Evaluator>>("preferred"),
            nullptr,
            get_eager_search_arguments_from_options(opts));
    }
};

FeaturePlugin<EagerWAstarSearchFeature> _plugin;

} // namespace
