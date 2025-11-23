#include "probfd/cli/pdbs/pattern_collection_generator_disjoint_cegar.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/utils/logging_options.h"
#include "downward/cli/utils/rng_options.h"

#include "probfd/cli/pdbs/cegar/cegar_options.h"
#include "probfd/cli/pdbs/pattern_collection_generator_options.h"

#include "probfd/pdbs/pattern_collection_generator_disjoint_cegar.h"

using namespace std;

using namespace downward;
using namespace utils;

using namespace probfd;
using namespace probfd::pdbs;
using namespace probfd::pdbs::cegar;

using namespace downward::cli::plugins;

using downward::cli::utils::add_log_options_to_feature;
using downward::cli::utils::add_rng_options_to_feature;

using namespace probfd::cli::pdbs;

namespace {

std::size_t add_pattern_collection_generator_cegar_options_to_feature(
    InternalFunctionDefinitionBase& feature,
    std::size_t start_index)
{
    feature.make_optional_argument_with_default(
        start_index,
        "single_goal",
        "false",
        "whether to compute only a single abstraction from a random goal");
    feature.make_optional_argument_with_default(
        start_index + 1,
        "max_pdb_size",
        "1000000",
        "maximum allowed number of states in a pdb (not applied to initial "
        "goal variable pattern(s))");
    feature.make_optional_argument_with_default(
        start_index + 2,
        "max_collection_size",
        "infinity()",
        "limit for the total number of PDB entries across all PDBs (not "
        "applied to initial goal variable pattern(s))");
    feature.make_optional_argument_with_default(
        start_index + 3,
        "max_time",
        "seconds_max()",
        "maximum time in seconds for CEGAR pattern generation. "
        "This includes the creation of the initial PDB collection"
        " as well as the creation of the correlation matrix.");
    feature.make_optional_argument_with_default(
        start_index + 4,
        "subcollection_finder_factory",
        "finder_trivial_factory()",
        "The subcollection finder factory.");
    feature.make_optional_argument_with_default(
        start_index + 5,
        "flaw_strategy",
        "pucs_flaw_finder()",
        "strategy used to find flaws in a policy");

    const auto n = add_pattern_collection_generator_options_to_feature(
        feature,
        start_index + 6);
    const auto n2 =
        add_cegar_wildcard_option_to_feature(feature, n + start_index + 6);

    return n + n2 + 6;
}

InternalFunctionDefinitionBase&
add_pattern_collection_generator_disjoint_cegar_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "ppdbs_disjoint_cegar",
        &downward::cli::plugins::construct_shared<
            PatternCollectionGenerator,
            PatternCollectionGeneratorDisjointCegar,
            value_t,
            bool,
            bool,
            int,
            int,
            FSeconds,
            std::shared_ptr<RandomNumberGenerator>,
            const std::shared_ptr<SubCollectionFinderFactory>&,
            const std::shared_ptr<FlawFindingStrategy>&,
            Verbosity>);

    f.make_optional_argument_with_default(
        0,
        "convergence_epsilon",
        "10e-4",
        "The tolerance for convergence checks.");

    const auto n =
        add_pattern_collection_generator_cegar_options_to_feature(f, 1);
    add_rng_options_to_feature(f, n + 1);

    return f;
}

} // namespace

namespace probfd::cli::pdbs {

void add_pattern_collection_generator_disjoint_cegar_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_pattern_collection_generator_disjoint_cegar_to_namespace(n);
}

} // namespace probfd::cli::pdbs
