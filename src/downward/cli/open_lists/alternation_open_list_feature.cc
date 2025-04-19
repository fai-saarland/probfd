#include "downward/cli/plugins/plugin.h"

#include "downward/open_lists/alternation_open_list.h"

using namespace std;
using namespace downward::utils;
using namespace downward::alternation_open_list;

using namespace downward::cli::plugins;

namespace {

template <typename T>
class AlternationOpenListFeature
    : public TypedFeature<
          downward::TaskDependentFactory<downward::OpenList<T>>,
          AlternationOpenListFactory<T>> {
public:
    AlternationOpenListFeature()
        requires(std::same_as<T, downward::StateOpenListEntry>)
        : AlternationOpenListFeature::TypedFeature("state_alt")
    {
        this->document_title("Alternation state open list");
        this->document_synopsis("alternates between several open lists.");

        this->template add_list_option<
            shared_ptr<downward::TaskDependentFactory<downward::OpenList<T>>>>(
            "sublists",
            "open lists between which this one alternates");
        this->template add_option<int>(
            "boost",
            "boost value for contained open lists that are restricted "
            "to preferred successors",
            "0");
    }

    AlternationOpenListFeature()
        requires(std::same_as<T, downward::EdgeOpenListEntry>)
        : AlternationOpenListFeature::TypedFeature("edge_alt")
    {
        this->document_title("Alternation edge open list");
        this->document_synopsis("alternates between several open lists.");

        this->template add_list_option<
            shared_ptr<downward::TaskDependentFactory<downward::OpenList<T>>>>(
            "sublists",
            "open lists between which this one alternates");
        this->template add_option<int>(
            "boost",
            "boost value for contained open lists that are restricted "
            "to preferred successors",
            "0");
    }

    virtual shared_ptr<AlternationOpenListFactory<T>>
    create_component(const Options& opts, const Context& context) const override
    {
        verify_list_non_empty<
            shared_ptr<downward::TaskDependentFactory<downward::OpenList<T>>>>(
            context,
            opts,
            "sublists");
        return make_shared_from_arg_tuples<AlternationOpenListFactory<T>>(
            opts.get_list<shared_ptr<
                downward::TaskDependentFactory<downward::OpenList<T>>>>(
                "sublists"),
            opts.get<int>("boost"));
    }
};

FeaturePlugin<AlternationOpenListFeature<downward::StateOpenListEntry>> _plugin;
FeaturePlugin<AlternationOpenListFeature<downward::EdgeOpenListEntry>> _plugin2;

} // namespace
