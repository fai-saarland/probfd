#include "downward/cli/pdbs/pattern_collection_generator_disjoint_cegar_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/cli/utils/rng_options.h"

#include "downward/cli/pdbs/cegar_options.h"
#include "downward/cli/pdbs/pattern_generator_options.h"
#include "downward/cli/pdbs/utils.h"

#include "downward/pdbs/pattern_collection_generator_disjoint_cegar.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::pdbs;

using namespace downward::cli::plugins;

using downward::cli::pdbs::add_generator_options_to_feature;
using downward::cli::pdbs::get_generator_arguments_from_options;

using downward::cli::utils::add_rng_options_to_feature;
using downward::cli::utils::get_rng_arguments_from_options;

namespace {
class PatternCollectionGeneratorDisjointCegarFeature
    : public SharedTypedFeature<PatternCollectionGenerator> {
public:
    PatternCollectionGeneratorDisjointCegarFeature()
        : SharedTypedFeature("disjoint_cegar")
    {
        document_title("Disjoint CEGAR");
        document_synopsis(
            "This pattern collection generator uses the CEGAR algorithm to "
            "compute a pattern for the planning task. See below "
            "for a description of the algorithm and some implementation notes. "
            "The original algorithm (called single CEGAR) is described in the "
            "paper " +
            get_rovner_et_al_reference());

        // TODO: these options could be move to the base class; see issue1022.
        add_optional_argument_with_default<int>(
            "max_pdb_size",
            "1000000",
            "maximum number of states per pattern database (ignored for the "
            "initial collection consisting of a singleton pattern for each "
            "goal "
            "variable)");
        add_optional_argument_with_default<int>(
            "max_collection_size",
            "10000000",
            "maximum number of states in the pattern collection (ignored for "
            "the "
            "initial collection consisting of a singleton pattern for each "
            "goal "
            "variable)");
        add_optional_argument_with_default<FSeconds>(
            "max_time",
            "seconds_max()",
            "maximum time in seconds for this pattern collection generator "
            "(ignored for computing the initial collection consisting of a "
            "singleton pattern for each goal variable)");
        add_cegar_wildcard_option_to_feature(*this);
        add_rng_options_to_feature(*this);
        add_generator_options_to_feature(*this);

        add_cegar_implementation_notes_to_feature(*this);
    }

    virtual shared_ptr<PatternCollectionGenerator>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<
            PatternCollectionGeneratorDisjointCegar>(
            opts.get<int>("max_pdb_size"),
            opts.get<int>("max_collection_size"),
            opts.get<FSeconds>("max_time"),
            get_cegar_wildcard_arguments_from_options(opts),
            get_rng_arguments_from_options(opts),
            get_generator_arguments_from_options(opts));
    }
};
} // namespace

namespace downward::cli::pdbs {

void add_pattern_collection_generator_disjoint_cegar_feature(
    RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<
        PatternCollectionGeneratorDisjointCegarFeature>();
}

} // namespace downward::cli::pdbs
