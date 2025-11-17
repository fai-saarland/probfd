#include "downward/cli/open_lists/alternation_open_list_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/open_lists/alternation_open_list.h"

using namespace std;
using namespace downward::utils;
using namespace downward::alternation_open_list;

using namespace downward::cli::plugins;

namespace {
template <typename T>
class AlternationOpenListFeature
    : public SharedTypedFeature<
          downward::TaskDependentFactory<downward::OpenList<T>>,
          const std::vector<shared_ptr<
              downward::TaskDependentFactory<downward::OpenList<T>>>>&,
          int> {
public:
    AlternationOpenListFeature()
        requires(std::same_as<T, downward::StateOpenListEntry>)
        : AlternationOpenListFeature::TypedFeature(
              "state_alt",
              &AlternationOpenListFeature::func)
    {
        this->document_title("Alternation state open list");
        this->document_synopsis("alternates between several open lists.");

        this->make_required_argument(
            0,
            "sublists",
            "open lists between which this one alternates");
        this->make_optional_argument_with_default(
            1,
            "boost",
            "0",
            "boost value for contained open lists that are restricted "
            "to preferred successors");
    }

    AlternationOpenListFeature()
        requires(std::same_as<T, downward::EdgeOpenListEntry>)
        : AlternationOpenListFeature::TypedFeature(
              "edge_alt",
              &AlternationOpenListFeature::func)
    {
        this->document_title("Alternation edge open list");
        this->document_synopsis("alternates between several open lists.");

        this->make_required_argument(
            0,
            "sublists",
            "open lists between which this one alternates");
        this->make_optional_argument_with_default(
            1,
            "boost",
            "0",
            "boost value for contained open lists that are restricted "
            "to preferred successors");
    }

    static shared_ptr<downward::TaskDependentFactory<downward::OpenList<T>>>
    func(
        const Context& context,
        const std::vector<
            shared_ptr<downward::TaskDependentFactory<downward::OpenList<T>>>>&
            sublists,
        int boost)
    {
        if (sublists.empty()) {
            context.error("List of open lists has to be non-empty.");
        }

        return make_shared<AlternationOpenListFactory<T>>(sublists, boost);
    }
};
} // namespace

namespace downward::cli::open_lists {

void add_alternation_open_list_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<
        AlternationOpenListFeature<downward::StateOpenListEntry>>();
    n.insert_feature_plugin<
        AlternationOpenListFeature<downward::EdgeOpenListEntry>>();
}

} // namespace downward::cli::open_lists
