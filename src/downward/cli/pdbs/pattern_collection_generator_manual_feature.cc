#include "downward/cli/pdbs/pattern_collection_generator_manual_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/pdbs/pattern_generator_options.h"

#include "downward/pdbs/pattern_collection_generator_manual.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::pdbs;
using namespace downward::cli::plugins;

namespace {
class PatternCollectionGeneratorManualFeature
    : public SharedTypedFeature<
          PatternCollectionGenerator,
          const std::vector<Pattern>&,
          Verbosity> {
public:
    PatternCollectionGeneratorManualFeature()
        : TypedFeature(
              "manual_patterns",
              &PatternCollectionGeneratorManualFeature::func)
    {
        make_required_argument(
            0,
            "patterns",
            "list of patterns (which are lists of variable numbers of the "
            "planning "
            "task).");
        add_generator_options_to_feature(*this, 1);
    }

    static shared_ptr<PatternCollectionGenerator> func(
        const std::vector<Pattern>& patterns,
        Verbosity verbosity)
    {
        return make_shared_from_arg_tuples<PatternCollectionGeneratorManual>(
            patterns,
            verbosity);
    }
};
} // namespace

namespace downward::cli::pdbs {

void add_pattern_collection_generator_manual_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<PatternCollectionGeneratorManualFeature>();
}

} // namespace downward::cli::pdbs
