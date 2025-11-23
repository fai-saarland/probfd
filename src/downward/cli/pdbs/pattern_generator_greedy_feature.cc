#include "downward/cli/pdbs/pattern_generator_greedy_feature.h"

#include "downward/cli/pdbs/pattern_generator_options.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/pdbs/pattern_generator_greedy.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::pdbs;
using namespace downward::cli::plugins;

namespace {

Feature& add_pattern_generator_greedy_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_typed_feature_plugin(
        "greedy",
        &downward::cli::plugins::make_shared<
            PatternGenerator,
            PatternGeneratorGreedy,
            int,
            Verbosity>);
    f.make_optional_argument_with_default(
        0,
        "max_states",
        "1000000",
        "maximal number of abstract states in the pattern database.");
    add_generator_options_to_feature(f, 1);

    return f;
}

} // namespace

namespace downward::cli::pdbs {

void add_pattern_generator_greedy_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_pattern_generator_greedy_to_namespace(n);
}

} // namespace downward::cli::pdbs
