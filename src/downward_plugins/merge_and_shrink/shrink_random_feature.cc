#include "downward_plugins/plugins/plugin.h"

#include "downward_plugins/merge_and_shrink/shrink_bucket_based_options.h"

#include "downward/merge_and_shrink/shrink_random.h"

using namespace std;
using namespace merge_and_shrink;
using namespace utils;

using namespace downward_plugins::plugins;

using downward_plugins::merge_and_shrink::add_shrink_bucket_options_to_feature;
using downward_plugins::merge_and_shrink::
    get_shrink_bucket_arguments_from_options;

namespace {

class ShrinkRandomFeature : public TypedFeature<ShrinkStrategy, ShrinkRandom> {
public:
    ShrinkRandomFeature()
        : TypedFeature("shrink_random")
    {
        document_title("Random");
        document_synopsis("");

        add_shrink_bucket_options_to_feature(*this);
    }

    virtual shared_ptr<ShrinkRandom>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<ShrinkRandom>(
            get_shrink_bucket_arguments_from_options(opts));
    }
};

FeaturePlugin<ShrinkRandomFeature> _plugin;

} // namespace
