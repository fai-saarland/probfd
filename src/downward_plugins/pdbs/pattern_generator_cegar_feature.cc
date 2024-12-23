#include "downward_plugins/plugins/plugin.h"

#include "downward_plugins/pdbs/cegar_options.h"
#include "downward_plugins/pdbs/pattern_generator_options.h"
#include "downward_plugins/pdbs/utils.h"

#include "downward_plugins/utils/rng_options.h"

#include "downward/pdbs/pattern_generator_cegar.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace utils;
using namespace pdbs;

using namespace downward_plugins::pdbs;
using namespace downward_plugins::plugins;

using downward_plugins::utils::add_rng_options_to_feature;
using downward_plugins::utils::get_rng_arguments_from_options;

namespace {

class PatternGeneratorCEGARFeature
    : public TypedFeature<PatternGenerator, PatternGeneratorCEGAR> {
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

        add_option<int>(
            "max_pdb_size",
            "maximum number of states in the final pattern database (possibly "
            "ignored by a singleton pattern consisting of a single goal "
            "variable)",
            "1000000",
            Bounds("1", "infinity"));
        add_option<double>(
            "max_time",
            "maximum time in seconds for the pattern generation",
            "infinity",
            Bounds("0.0", "infinity"));
        add_cegar_wildcard_option_to_feature(*this);
        add_rng_options_to_feature(*this);
        add_generator_options_to_feature(*this);

        add_cegar_implementation_notes_to_feature(*this);
    }

    virtual shared_ptr<PatternGeneratorCEGAR>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<PatternGeneratorCEGAR>(
            opts.get<int>("max_pdb_size"),
            opts.get<double>("max_time"),
            get_cegar_wildcard_arguments_from_options(opts),
            get_rng_arguments_from_options(opts),
            get_generator_arguments_from_options(opts));
    }
};

FeaturePlugin<PatternGeneratorCEGARFeature> _plugin;

} // namespace
