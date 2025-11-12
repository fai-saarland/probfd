#include "downward/cli/pdbs/pattern_generator_cegar_feature.h"

#include "downward/cli/pdbs/cegar_options.h"
#include "downward/cli/pdbs/pattern_generator_options.h"
#include "downward/cli/pdbs/utils.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/utils/rng_options.h"

#include "downward/pdbs/pattern_generator_cegar.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::pdbs;
using namespace downward::cli::plugins;

using downward::cli::utils::add_rng_options_to_feature;
using downward::cli::utils::get_rng_arguments_from_options;

namespace {
class PatternGeneratorCEGARFeature : public SharedTypedFeature<PatternGenerator> {
public:
    PatternGeneratorCEGARFeature()
        : TypedFeature("cegar_pattern")
    {
        document_title("CEGAR");
        document_synopsis(
            "This pattern generator uses the CEGAR algorithm restricted to a "
            "random single goal of the task to compute a pattern. See below "
            "for a description of the algorithm and some implementation notes. "
            "The original algorithm (called single CEGAR) is described in the "
            "paper " +
            get_rovner_et_al_reference());

        add_optional_argument_with_default<int>(
            "max_pdb_size",
            "1000000",
            "maximum number of states in the final pattern database (possibly "
            "ignored by a singleton pattern consisting of a single goal "
            "variable)");
        add_optional_argument_with_default<FSeconds>(
            "max_time",
            "seconds_max()",
            "maximum time in seconds for the pattern generation");
        add_cegar_wildcard_option_to_feature(*this);
        add_rng_options_to_feature(*this);
        add_generator_options_to_feature(*this);

        add_cegar_implementation_notes_to_feature(*this);
    }

    virtual shared_ptr<PatternGenerator>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<PatternGeneratorCEGAR>(
            opts.get<int>("max_pdb_size"),
            opts.get<FSeconds>("max_time"),
            get_cegar_wildcard_arguments_from_options(opts),
            get_rng_arguments_from_options(opts),
            get_generator_arguments_from_options(opts));
    }
};
} // namespace

namespace downward::cli::pdbs {

void add_pattern_generator_cegar_feature(Registry& raw_registry)
{
    raw_registry.insert_feature_plugin<PatternGeneratorCEGARFeature>();
}

} // namespace downward::cli::pdbs
