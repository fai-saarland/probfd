#include "downward/cli/pdbs/pattern_generator_random_feature.h"

#include "downward/cli/pdbs/pattern_generator_options.h"
#include "downward/cli/pdbs/random_pattern_options.h"
#include "downward/cli/pdbs/utils.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/utils/rng_options.h"

#include "downward/pdbs/pattern_generator_random.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::pdbs;
using namespace downward::cli::plugins;

using downward::cli::utils::add_rng_options_to_feature;

namespace {
class PatternGeneratorRandomFeature
    : public SharedTypedFeature<
          PatternGenerator,
          int,
          FSeconds,
          bool,
          int,
          Verbosity> {
public:
    PatternGeneratorRandomFeature()
        : TypedFeature("random_pattern", &PatternGeneratorRandomFeature::func)
    {
        document_title("Random Pattern");
        document_synopsis(
            "This pattern generator implements the 'single randomized "
            "causal graph' algorithm described in experiments of the the "
            "paper" +
            get_rovner_et_al_reference() +
            "See below for a description of the algorithm and some "
            "implementation "
            "notes.");

        add_random_pattern_implementation_notes_to_feature(*this);

        make_optional_argument_with_default(
            0,
            "max_pdb_size",
            "1000000",
            "maximum number of states in the final pattern database (possibly "
            "ignored by a singleton pattern consisting of a single goal "
            "variable)");
        make_optional_argument_with_default(
            1,
            "max_time",
            "seconds_max()",
            "maximum time in seconds for the pattern generation");
        const auto n =
            add_random_pattern_bidirectional_option_to_feature(*this, 2);
        const auto n2 = add_rng_options_to_feature(*this, n + 2);
        add_generator_options_to_feature(*this, n + n2 + 2);
    }

    static shared_ptr<PatternGenerator> func(
        int max_pdb_size,
        FSeconds max_time,
        bool bidirectional,
        int random_seed,
        Verbosity verbosity)
    {
        return make_shared_from_arg_tuples<PatternGeneratorRandom>(
            max_pdb_size,
            max_time,
            bidirectional,
            random_seed,
            verbosity);
    }
};

} // namespace

namespace downward::cli::pdbs {

void add_pattern_generator_random_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<PatternGeneratorRandomFeature>();
}

} // namespace downward::cli::pdbs
