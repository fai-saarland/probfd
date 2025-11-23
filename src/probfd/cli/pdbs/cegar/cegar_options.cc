#include "probfd/cli/pdbs/cegar/cegar_options.h"

#include "downward/cli/plugins/plugin.h"

using namespace downward::cli::plugins;

namespace probfd::cli::pdbs {

std::size_t
add_cegar_wildcard_option_to_feature(InternalFunctionDefinitionBase& feature, std::size_t start_index)
{
    feature.make_optional_argument_with_default(
        start_index,
        "use_wildcard_policies",
        "false",
        "if true, compute wildcard plans which are sequences of sets of "
        "operators that induce the same transition; otherwise compute regular "
        "plans which are sequences of single operators");

    return 1;
}

} // namespace probfd::cli::pdbs
