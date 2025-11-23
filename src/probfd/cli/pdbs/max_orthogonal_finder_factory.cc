#include "probfd/cli/pdbs/max_orthogonal_finder_factory.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/pdbs/max_orthogonal_finder_factory.h"

using namespace downward;
using namespace utils;

using namespace probfd::pdbs;

using namespace downward::cli::plugins;

namespace {

Feature& add_additive_max_orthogonality_factory_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_typed_feature_plugin(
        "additive_max_orthogonality_factory",
        &downward::cli::plugins::make_shared<
            SubCollectionFinderFactory,
            AdditiveMaxOrthogonalityFinderFactory>);

    return f;
}

Feature&
add_multiplicative_max_orthogonality_factory_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_typed_feature_plugin(
        "multiplicative_max_orthogonality_factory",
        &downward::cli::plugins::make_shared<
            SubCollectionFinderFactory,
            AdditiveMaxOrthogonalityFinderFactory>);

    return f;
}

} // namespace

namespace probfd::cli::pdbs {

void add_max_orthogonal_finder_factory_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_additive_max_orthogonality_factory_to_namespace(n);
    add_multiplicative_max_orthogonality_factory_to_namespace(n);
}

} // namespace probfd::cli::pdbs
