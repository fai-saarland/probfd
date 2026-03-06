#include "probfd/cli/merge_and_shrink/shrink_strategy_equal_distance.h"

#include "probfd/cli/merge_and_shrink/shrink_strategy_bucket_based_options.h"

#include "probfd/merge_and_shrink/shrink_strategy_equal_distance.h"

#include "probfd/merge_and_shrink/transition_system.h"

#include "language/ast/internal_enum_declaration.h"
#include "language/ast/internal_function_definition.h"

using namespace std;
using namespace downward;
using namespace language::parser;
using namespace probfd::merge_and_shrink;
using namespace probfd::cli::merge_and_shrink;

namespace {

InternalFunctionDefinitionBase& add_shrink_strategy_equal_distance_to_namespace(
    NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        ShrinkStrategy,
        ShrinkStrategyEqualDistance,
        std::shared_ptr<utils::RandomNumberGenerator>,
        ShrinkStrategyEqualDistance::Priority>>(
        nspace,
        "shrink_equal_distance");

    f.document_title("distance-preserving shrink strategy");

    f.document_note(
        "Note",
        "The strategy first partitions all states according to their "
        "h-values. States sorted last are shrinked together until reaching "
        "max_states.");

    const auto n = add_bucket_based_shrink_options_to_feature(f, 0);

    f.make_optional_argument_with_default(
        n,
        "priority",
        "low",
        "in which direction the distance based shrink priority is ordered");

    return f;
}

} // namespace

namespace probfd::cli::merge_and_shrink {

void add_shrink_strategy_equal_distance_feature(
    NamespaceLevelDeclarationList& nspace)
{
    insert_enum_declaration<ShrinkStrategyEqualDistance::Priority>(
        nspace,
        "Priority",
        {{"high", "prefer shrinking states with high value"},
         {"low", "prefer shrinking states with low value"}});

    add_shrink_strategy_equal_distance_to_namespace(nspace);
}

} // namespace probfd::cli::merge_and_shrink
