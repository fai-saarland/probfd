#include "downward_cli/pdbs/pattern_generator_greedy_feature.h"

#include "downward_cli/pdbs/pattern_generator_options.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "downward/pdbs/pattern_generator_greedy.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::pdbs;

using namespace language;
using namespace language::plugins;

namespace {
class PatternGeneratorGreedyFeature : public TypedFeature<PatternGenerator> {
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

    shared_ptr<PatternGenerator>
    create_component(const Options& opts, const Context& context) const override
    {
        return make_shared_from_arg_tuples<PatternGeneratorGreedy>(
            opts.get<int>(context, "max_states"),
            get_generator_arguments_from_options(context, opts));
    }
};
} // namespace

namespace downward::cli::pdbs {

void add_pattern_generator_greedy_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<PatternGeneratorGreedyFeature>();
}

} // namespace downward::cli::pdbs
