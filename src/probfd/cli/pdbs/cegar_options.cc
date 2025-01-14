#include "probfd/cli/pdbs/cegar_options.h"

#include "downward/cli/plugins/plugin.h"

using namespace downward::cli::plugins;

namespace probfd::cli::pdbs {

void add_cegar_wildcard_option_to_feature(Feature& feature)
{
    feature.add_option<bool>(
        "use_wildcard_policies",
        "if true, compute wildcard plans which are sequences of sets of "
        "operators that induce the same transition; otherwise compute regular "
        "plans which are sequences of single operators",
        "false");
}

CEGARArgs get_cegar_wildcard_arguments_from_options(const Options& opts)
{
    return std::make_tuple(opts.get<bool>("use_wildcard_policies"));
}

} // namespace probfd::cli::pdbs
