#include "downward/cli/pdbs/pattern_generator_manual_feature.h"

#include "downward/cli/pdbs/pattern_generator_options.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/pdbs/pattern_generator_manual.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::pdbs;
using namespace downward::cli::plugins;

namespace {
class PatternGeneratorManualFeature
    : public SharedTypedFeature<
          PatternGenerator,
          const std::vector<int>&,
          Verbosity> {
public:
    PatternGeneratorManualFeature()
        : TypedFeature("manual_pattern", &PatternGeneratorManualFeature::func)
    {
        make_required_argument(
            0,
            "pattern",
            "list of variable numbers of the planning task that should be used "
            "as the pattern.");
        add_generator_options_to_feature(*this, 1);
    }

    static shared_ptr<PatternGenerator>
    func(const std::vector<int>& pattern, Verbosity verbosity)
    {
        return make_shared_from_arg_tuples<PatternGeneratorManual>(
            pattern,
            verbosity);
    }
};
} // namespace

namespace downward::cli::pdbs {

void add_pattern_generator_manual_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<PatternGeneratorManualFeature>();
}

} // namespace downward::cli::pdbs
