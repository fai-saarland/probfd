#include "downward_cli/merge_and_shrink/shrink_random_feature.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "downward_cli/merge_and_shrink/shrink_bucket_based_options.h"

#include "downward/merge_and_shrink/shrink_random.h"

using namespace std;
using namespace downward::merge_and_shrink;
using namespace downward::utils;

using namespace language;
using namespace language::plugins;

using downward::cli::merge_and_shrink::add_shrink_bucket_options_to_feature;
using downward::cli::merge_and_shrink::get_shrink_bucket_arguments_from_options;

namespace {
class ShrinkRandomFeature : public TypedFeature<ShrinkStrategy> {
public:
    ShrinkRandomFeature()
        : TypedFeature("shrink_random")
    {
        document_title("Random");
        document_synopsis("");

        add_shrink_bucket_options_to_feature(*this);
    }

    shared_ptr<ShrinkStrategy>
    create_component(const Options& opts, const Context& context) const override
    {
        return make_shared_from_arg_tuples<ShrinkRandom>(
            get_shrink_bucket_arguments_from_options(context, opts));
    }
};
} // namespace

namespace downward::cli::merge_and_shrink {

void add_shrink_random_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<ShrinkRandomFeature>();
}

} // namespace downward::cli::merge_and_shrink
