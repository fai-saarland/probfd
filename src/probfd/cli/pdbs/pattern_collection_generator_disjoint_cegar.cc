#include "probfd/cli/pdbs/pattern_collection_generator_disjoint_cegar.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/cli/utils/logging_options.h"
#include "downward/cli/utils/rng_options.h"

#include "probfd/cli/pdbs/cegar/cegar_options.h"
#include "probfd/cli/pdbs/pattern_collection_generator_options.h"

#include "probfd/pdbs/pattern_collection_generator_disjoint_cegar.h"

#include "downward/utils/rng_options.h"

using namespace std;

using namespace downward;
using namespace utils;

using namespace probfd;
using namespace probfd::pdbs;
using namespace probfd::pdbs::cegar;

using namespace downward::cli::plugins;

using downward::cli::utils::add_log_options_to_feature;
using downward::cli::utils::get_log_arguments_from_options;

using downward::cli::utils::add_rng_options_to_feature;
using downward::cli::utils::get_rng_arguments_from_options;

using namespace probfd::cli::pdbs;

namespace {

void add_pattern_collection_generator_cegar_options_to_feature(Feature& feature)
{
    feature.add_optional_argument_with_default<bool>(
        "single_goal",
        "false",
        "whether to compute only a single abstraction from a random goal");
    feature.add_optional_argument_with_default<int>(
        "max_pdb_size",
        "1000000",
        "maximum allowed number of states in a pdb (not applied to initial "
        "goal variable pattern(s))");
    feature.add_optional_argument_with_default<int>(
        "max_collection_size",
        "infinity",
        "limit for the total number of PDB entries across all PDBs (not "
        "applied to initial goal variable pattern(s))");
    feature.add_optional_duration_argument_with_default(
        "max_time",
        "infinite",
        "maximum time in seconds for CEGAR pattern generation. "
        "This includes the creation of the initial PDB collection"
        " as well as the creation of the correlation matrix.");
    feature.add_optional_argument_with_default<
        std::shared_ptr<SubCollectionFinderFactory>>(
        "subcollection_finder_factory",
        "finder_trivial_factory()",
        "The subcollection finder factory.");
    feature.add_optional_argument_with_default<
        std::shared_ptr<FlawFindingStrategy>>(
        "flaw_strategy",
        "pucs_flaw_finder()",
        "strategy used to find flaws in a policy");

    add_pattern_collection_generator_options_to_feature(feature);
    add_cegar_wildcard_option_to_feature(feature);
}

class PatternCollectionGeneratorDisjointCEGARFeature
    : public SharedTypedFeature<PatternCollectionGenerator> {
public:
    PatternCollectionGeneratorDisjointCEGARFeature()
        : SharedTypedFeature("ppdbs_disjoint_cegar")
    {
        add_optional_argument_with_default<value_t>(
            "convergence_epsilon",
            "10e-4",
            "The tolerance for convergence checks.");

        add_pattern_collection_generator_cegar_options_to_feature(*this);
        add_rng_options_to_feature(*this);
    }

    virtual shared_ptr<PatternCollectionGenerator>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<
            PatternCollectionGeneratorDisjointCegar>(
            opts.get<value_t>("convergence_epsilon"),
            get_cegar_wildcard_arguments_from_options(opts),
            opts.get<bool>("single_goal"),
            opts.get<int>("max_pdb_size"),
            opts.get<int>("max_collection_size"),
            opts.get<utils::FSeconds>("max_time"),
            get_rng(std::get<0>(get_rng_arguments_from_options(opts))),
            opts.get_shared<SubCollectionFinderFactory>(
                "subcollection_finder_factory"),
            opts.get_shared<FlawFindingStrategy>("flaw_strategy"),
            get_pattern_collection_generator_arguments_from_options(opts));
    }
};
} // namespace

namespace probfd::cli::pdbs {

void add_pattern_collection_generator_disjoint_cegar_feature(
    RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<
        PatternCollectionGeneratorDisjointCEGARFeature>();
}

} // namespace probfd::cli::pdbs
