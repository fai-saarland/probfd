#include "downward/cli/pdbs/pattern_generator_greedy_feature.h"

#include "downward/cli/pdbs/pattern_generator_options.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/pdbs/pattern_generator_greedy.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::pdbs;
using namespace downward::cli::plugins;

namespace {
class PatternGeneratorGreedyFeature
    : public TypedFeature<PatternGenerator, PatternGeneratorGreedy> {
public:
    PatternGeneratorGreedyFeature()
        : TypedFeature("greedy")
    {
        add_option<int>(
            "max_states",
            "maximal number of abstract states in the pattern database.",
            "1000000",
            Bounds("1", "infinity"));
        add_generator_options_to_feature(*this);
    }

    virtual shared_ptr<PatternGeneratorGreedy>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<PatternGeneratorGreedy>(
            opts.get<int>("max_states"),
            get_generator_arguments_from_options(opts));
    }
};
}

namespace downward::cli::pdbs {

void add_pattern_generator_greedy_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<PatternGeneratorGreedyFeature>();
}

} // namespace
