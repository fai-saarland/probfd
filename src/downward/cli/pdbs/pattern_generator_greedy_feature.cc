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
class PatternGeneratorGreedyFeature
    : public SharedTypedFeature<PatternGenerator, int, Verbosity> {
public:
    PatternGeneratorGreedyFeature()
        : TypedFeature("greedy", &PatternGeneratorGreedyFeature::func)
    {
        make_optional_argument_with_default(
            0,
            "max_states",
            "1000000",
            "maximal number of abstract states in the pattern database.");
        add_generator_options_to_feature(*this, 1);
    }

    static shared_ptr<PatternGenerator>
    func(const Context&, int max_states, Verbosity verbosity)
    {
        return make_shared_from_arg_tuples<PatternGeneratorGreedy>(
            max_states,
            verbosity);
    }
};
} // namespace

namespace downward::cli::pdbs {

void add_pattern_generator_greedy_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<PatternGeneratorGreedyFeature>();
}

} // namespace downward::cli::pdbs
