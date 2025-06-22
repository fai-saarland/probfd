#include "probfd/merge_and_shrink/merge_tree_factory_manual.h"

#include "probfd/merge_and_shrink/merge_tree.h"

#include "probfd/cli/merge_and_shrink/merge_tree_factory_options.h"

#include "downward/cli/plugins/plugin.h"

using namespace std;
using namespace downward::cli::plugins;
using namespace downward;
using namespace probfd::merge_and_shrink;
using namespace probfd::cli::merge_and_shrink;

namespace {

class MergeTreeFactoryLinearFeature
    : public TypedFeature<MergeTreeFactory, MergeTreeFactoryManual> {
public:
    MergeTreeFactoryLinearFeature()
        : TypedFeature("pmanual")
    {
        document_title("Manual linear merge tree");

        add_merge_tree_factory_options_to_feature(*this);

        add_list_option<int>(
            "order",
            "the order in which atomic transition systems are merged",
            ArgumentInfo::NO_DEFAULT);
    }

protected:
    shared_ptr<MergeTreeFactoryManual>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return make_shared_from_arg_tuples<MergeTreeFactoryManual>(
            get_merge_tree_factory_args_from_options(options),
            options.get_list<int>("order"));
    }
};

FeaturePlugin<MergeTreeFactoryLinearFeature> _plugin;

} // namespace
