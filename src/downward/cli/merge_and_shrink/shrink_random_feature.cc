#include "downward/cli/merge_and_shrink/shrink_random_feature.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include "downward/cli/merge_and_shrink/shrink_bucket_based_options.h"

#include "downward/merge_and_shrink/shrink_random.h"

using namespace std;
using namespace downward::merge_and_shrink;
using namespace downward::utils;

using namespace language::plugins;

using downward::cli::merge_and_shrink::add_shrink_bucket_options_to_feature;

namespace {

InternalFunctionDefinitionBase& add_shrink_strategy_random_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "shrink_random",
        &language::plugins::construct_shared<
            ShrinkStrategy,
            ShrinkRandom,
            std::shared_ptr<RandomNumberGenerator>>);
    f.document_title("Random Shrinking");
    f.document_synopsis("");

    add_shrink_bucket_options_to_feature(f, 0);

    return f;
}

} // namespace

namespace downward::cli::merge_and_shrink {

void add_shrink_random_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_shrink_strategy_random_to_namespace(n);
}

} // namespace downward::cli::merge_and_shrink
