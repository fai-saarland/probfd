#include "downward/cli/plugins/plugin.h"

#include "downward/open_lists/alternation_open_list.h"

using namespace std;
using namespace utils;
using namespace alternation_open_list;

using namespace downward::cli::plugins;

namespace {

class AlternationOpenListFeature
    : public TypedFeature<OpenListFactory, AlternationOpenListFactory> {
public:
    AlternationOpenListFeature()
        : TypedFeature("alt")
    {
        document_title("Alternation open list");
        document_synopsis("alternates between several open lists.");

        add_list_option<shared_ptr<OpenListFactory>>(
            "sublists",
            "open lists between which this one alternates");
        add_option<int>(
            "boost",
            "boost value for contained open lists that are restricted "
            "to preferred successors",
            "0");
    }

    virtual shared_ptr<AlternationOpenListFactory>
    create_component(const Options& opts, const Context& context) const override
    {
        verify_list_non_empty<shared_ptr<OpenListFactory>>(
            context,
            opts,
            "sublists");
        return make_shared_from_arg_tuples<AlternationOpenListFactory>(
            opts.get_list<shared_ptr<OpenListFactory>>("sublists"),
            opts.get<int>("boost"));
    }
};

FeaturePlugin<AlternationOpenListFeature> _plugin;

} // namespace
