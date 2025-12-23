#include "probfd/cli/merge_and_shrink/shrink_strategy_random.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "probfd/merge_and_shrink/shrink_strategy_random.h"

#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "probfd/cli/merge_and_shrink/shrink_strategy_bucket_based_options.h"

using namespace std;
using namespace downward;
using namespace language::plugins;
using namespace probfd::merge_and_shrink;
using namespace probfd::cli::merge_and_shrink;

namespace probfd::cli::merge_and_shrink {

InternalFunctionDefinitionBase&
add_shrink_strategy_random_feature(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "pshrink_random",
        &language::plugins::construct_shared<
            ShrinkStrategy,
            ShrinkStrategyRandom,
            std::shared_ptr<utils::RandomNumberGenerator>>);

    f.document_title("Random Shrink Strategy");
    f.document_synopsis(
        "This strategy picks states to shrink uniformly at random.");

    add_bucket_based_shrink_options_to_feature(f, 0);

    return f;
}

} // namespace probfd::cli::merge_and_shrink
