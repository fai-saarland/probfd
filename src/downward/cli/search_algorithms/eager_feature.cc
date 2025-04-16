#include "downward/cli/plugins/plugin.h"

#include "downward/cli/search_algorithms/eager_search_options.h"

#include "downward/search_algorithms/eager_search.h"
#include "downward/search_algorithms/search_common.h"

using namespace std;
using namespace downward;
using namespace downward::eager_search;

using namespace downward::cli::eager_search;
using namespace downward::cli::plugins;

namespace {

class EagerSearchFeature
    : public TypedFeature<SearchAlgorithm, EagerSearch> {
public:
    EagerSearchFeature()
        : TypedFeature("eager")
    {
        document_title("Eager best-first search");
        document_synopsis("");

        add_option<shared_ptr<OpenListFactory>>("open", "open list");
        add_option<bool>("reopen_closed", "reopen closed nodes", "false");
        add_option<shared_ptr<Evaluator>>(
            "f_eval",
            "set evaluator for jump statistics. "
            "(Optional; if no evaluator is used, jump statistics will not be "
            "displayed.)",
            ArgumentInfo::NO_DEFAULT);
        add_list_option<shared_ptr<Evaluator>>(
            "preferred",
            "use preferred operators of these evaluators",
            "[]");
        add_eager_search_options_to_feature(*this, "eager");
    }

    virtual shared_ptr<EagerSearch>
    create_component(const Options& opts, const utils::Context&) const override
    {
        return make_shared_from_arg_tuples<EagerSearch>(
            opts.get<shared_ptr<OpenListFactory>>("open"),
            opts.get<bool>("reopen_closed"),
            opts.get<shared_ptr<Evaluator>>("f_eval", nullptr),
            opts.get_list<shared_ptr<Evaluator>>("preferred"),
            nullptr,
            get_eager_search_arguments_from_options(opts));
    }
};

FeaturePlugin<EagerSearchFeature> _plugin;

} // namespace
