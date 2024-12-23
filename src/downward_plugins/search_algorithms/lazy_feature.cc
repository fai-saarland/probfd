#include "downward_plugins/plugins/plugin.h"

#include "downward_plugins/search_algorithm_options.h"

#include "downward/search_algorithms/lazy_search.h"
#include "downward/search_algorithms/search_common.h"

using namespace std;
using namespace downward_plugins::plugins;

using downward_plugins::add_search_algorithm_options_to_feature;
using downward_plugins::add_successors_order_options_to_feature;
using downward_plugins::get_search_algorithm_arguments_from_options;
using downward_plugins::get_successors_order_arguments_from_options;

namespace {

class LazySearchFeature
    : public TypedFeature<SearchAlgorithm, lazy_search::LazySearch> {
public:
    LazySearchFeature()
        : TypedFeature("lazy")
    {
        document_title("Lazy best-first search");
        document_synopsis("");

        add_option<shared_ptr<OpenListFactory>>("open", "open list");
        add_option<bool>("reopen_closed", "reopen closed nodes", "false");
        add_list_option<shared_ptr<Evaluator>>(
            "preferred",
            "use preferred operators of these evaluators",
            "[]");
        add_successors_order_options_to_feature(*this);
        add_search_algorithm_options_to_feature(*this, "lazy");
    }

    virtual shared_ptr<lazy_search::LazySearch>
    create_component(const Options& opts, const utils::Context&) const override
    {
        return make_shared_from_arg_tuples<lazy_search::LazySearch>(
            opts.get<shared_ptr<OpenListFactory>>("open"),
            opts.get<bool>("reopen_closed"),
            opts.get_list<shared_ptr<Evaluator>>("preferred"),
            get_successors_order_arguments_from_options(opts),
            get_search_algorithm_arguments_from_options(opts));
    }
};

FeaturePlugin<LazySearchFeature> _plugin;

} // namespace
