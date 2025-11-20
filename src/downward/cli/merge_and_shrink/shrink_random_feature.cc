#include "downward/cli/merge_and_shrink/shrink_random_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/merge_and_shrink/shrink_bucket_based_options.h"

#include "downward/merge_and_shrink/shrink_random.h"

using namespace std;
using namespace downward::merge_and_shrink;
using namespace downward::utils;

using namespace downward::cli::plugins;

using downward::cli::merge_and_shrink::add_shrink_bucket_options_to_feature;

namespace {
class ShrinkRandomFeature : public SharedTypedFeature<ShrinkStrategy, int> {
public:
    ShrinkRandomFeature()
        : TypedFeature("shrink_random", &ShrinkRandomFeature::func)
    {
        document_title("Random");
        document_synopsis("");

        add_shrink_bucket_options_to_feature(*this, 0);
    }

    static shared_ptr<ShrinkStrategy> func(int random_seed)
    {
        return make_shared_from_arg_tuples<ShrinkRandom>(random_seed);
    }
};
} // namespace

namespace downward::cli::merge_and_shrink {

void add_shrink_random_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<ShrinkRandomFeature>();
}

} // namespace downward::cli::merge_and_shrink
