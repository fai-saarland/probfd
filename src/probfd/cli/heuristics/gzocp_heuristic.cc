#include "probfd/cli/heuristics/gzocp_heuristic.h"

#include "downward/cli/utils/rng_options.h"

#include "probfd/heuristics/gzocp_heuristic.h"

#include "language/ast/internal_enum_declaration.h"
#include "language/ast/internal_function_definition.h"

using namespace downward;
using namespace probfd;
using namespace probfd::pdbs;
using namespace probfd::heuristics;

using namespace probfd::cli::heuristics;

using namespace language::parser;

using downward::cli::utils::add_rng_options_to_feature;

namespace {

InternalFunctionDefinitionBase&
add_greedy_zero_one_cost_partitioning_heuristic_to_namespace(
    NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&construct_shared<
        TaskHeuristicFactory,
        GZOCPHeuristicFactory,
        std::shared_ptr<PatternCollectionGenerator>,
        GZOCPHeuristicFactory::OrderingStrategy,
        std::shared_ptr<utils::RandomNumberGenerator>>>(
        nspace,
        "gzocp_heuristic");

    f.document_title("Greedy Zero-One Operator Cost Partitioning Heuristic");
    f.document_synopsis(
        "This heuristic computes a greedy cost-partitioning estimate over "
        "a set of projections. The cost-partitioning assigns the full "
        "operator cost to the first projection in the sequence that is "
        "affected by it (induces a non-self-loop) and assigns a cost of "
        "zero for this operator for all subsequent projections.");

    f.make_optional_argument_with_default(
        0,
        "patterns",
        "classical_generator(generator=systematic(pattern_max_size=2))",
        "The pattern generation algorithm to construct the projections.");

    f.make_optional_argument_with_default(
        1,
        "order",
        "random",
        "The order in which the projections are considered.");

    add_rng_options_to_feature(f, 2);

    return f;
}

} // namespace

namespace probfd::cli::heuristics {

void add_gzocp_heuristic_feature(NamespaceLevelDeclarationList& nspace)
{
    insert_enum_declaration<GZOCPHeuristicFactory::OrderingStrategy>(
        nspace,
        "OrderingStrategy",
        {{"random", "the order is random"},
         {"size_asc", "orders the PDBs by increasing size"},
         {"size_desc", "orders the PDBs by decreasing size"},
         {"inherit",
          "inherits the order from the underlying pattern generation "
          "algorithm"}});

    add_greedy_zero_one_cost_partitioning_heuristic_to_namespace(nspace);
}

} // namespace probfd::cli::heuristics
