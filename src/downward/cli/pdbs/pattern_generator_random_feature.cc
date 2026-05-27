#include "downward/cli/pdbs/pattern_generator_random_feature.h"

#include "downward/cli/pdbs/pattern_generator_options.h"
#include "downward/cli/pdbs/random_pattern_options.h"
#include "downward/cli/pdbs/utils.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/cli/utils/rng_options.h"

#include "downward/pdbs/pattern_generator_random.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::pdbs;
using namespace downward::cli::plugins;

using downward::cli::utils::add_rng_options_to_feature;
using downward::cli::utils::get_rng_arguments_from_options;

namespace {
class PatternGeneratorRandomFeature : public SharedTypedFeature<PatternGenerator> {
public:
    PatternGeneratorRandomFeature()
        : SharedTypedFeature("random_pattern")
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

        add_optional_argument_with_default<int>(
            "max_pdb_size",
            "1000000",
            "maximum number of states in the final pattern database (possibly "
            "ignored by a singleton pattern consisting of a single goal "
            "variable)");
        add_optional_argument_with_default<downward::utils::FSeconds>(
            "max_time",
            "seconds_max()",
            "maximum time in seconds for the pattern generation");
        add_random_pattern_bidirectional_option_to_feature(*this);
        add_rng_options_to_feature(*this);
        add_generator_options_to_feature(*this);

        add_random_pattern_implementation_notes_to_feature(*this);
    }

    virtual shared_ptr<PatternGenerator>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<PatternGeneratorRandom>(
            opts.get<int>("max_pdb_size"),
            opts.get<downward::utils::FSeconds>("max_time"),
            get_random_pattern_bidirectional_arguments_from_options(opts),
            get_rng_arguments_from_options(opts),
            get_generator_arguments_from_options(opts));
    }
};

} // namespace

namespace downward::cli::pdbs {

void add_pattern_generator_random_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<PatternGeneratorRandomFeature>();
}

} // namespace downward::cli::pdbs
