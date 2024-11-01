#include "probfd_plugins/pdbs/cegar_options.h"

#include "downward/plugins/plugin.h"

namespace probfd_plugins::pdbs {

void add_cegar_wildcard_option_to_feature(plugins::Feature& feature)
{
    feature.add_option<bool>(
        "use_wildcard_policies",
        "if true, compute wildcard plans which are sequences of sets of "
        "operators that induce the same transition; otherwise compute regular "
        "plans which are sequences of single operators",
        "false");
}

std::tuple<bool>
get_cegar_wildcard_arguments_from_options(const plugins::Options& opts)
{
    return std::make_tuple(opts.get<bool>("use_wildcard_policies"));
}

} // namespace probfd_plugins::pdbs
