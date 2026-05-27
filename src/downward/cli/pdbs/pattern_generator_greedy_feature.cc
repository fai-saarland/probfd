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
class PatternGeneratorGreedyFeature : public SharedTypedFeature<PatternGenerator> {
public:
    PatternGeneratorGreedyFeature()
        : TypedFeature("greedy")
    {
        add_optional_argument_with_default<int>(
            "max_states",
            "1000000",
            "maximal number of abstract states in the pattern database.");
        add_generator_options_to_feature(*this);
    }

    virtual shared_ptr<PatternGenerator>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<PatternGeneratorGreedy>(
            opts.get<int>("max_states"),
            get_generator_arguments_from_options(opts));
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
