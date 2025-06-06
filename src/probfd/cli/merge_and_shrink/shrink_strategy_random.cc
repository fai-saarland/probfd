#include "probfd/merge_and_shrink/shrink_strategy_random.h"

#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "probfd/cli/merge_and_shrink/shrink_strategy_bucket_based_options.h"

#include "downward/cli/plugins/plugin.h"

using namespace std;
using namespace downward;
using namespace downward::cli::plugins;
using namespace probfd::merge_and_shrink;
using namespace probfd::cli::merge_and_shrink;

namespace {

class ShrinkRandomFeature
    : public TypedFeature<ShrinkStrategy, ShrinkStrategyRandom> {
public:
    ShrinkRandomFeature()
        : TypedFeature("pshrink_random")
    {
        document_title("Random Shrink Strategy");
        document_synopsis(
            "This strategy picks states to shrink uniformly at random.");

        add_bucket_based_shrink_options_to_feature(*this);
    }

protected:
    shared_ptr<ShrinkStrategyRandom>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return make_shared_from_arg_tuples<ShrinkStrategyRandom>(
            get_bucket_based_shrink_args_from_options(options));
    }
};

FeaturePlugin<ShrinkRandomFeature> _plugin;

} // namespace
