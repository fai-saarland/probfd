#include "probfd_plugins/pdbs/pattern_collection_generator_disjoint_cegar.h"

#include "probfd_plugins/pdbs/cegar_options.h"
#include "probfd_plugins/pdbs/pattern_collection_generator.h"

#include "probfd/pdbs/pattern_collection_generator_disjoint_cegar.h"

#include "downward/utils/rng_options.h"

#include "downward/plugins/plugin.h"

using namespace std;

using namespace probfd::pdbs;
using namespace probfd_plugins::pdbs;
using namespace probfd::pdbs::cegar;

namespace probfd_plugins::pdbs {

void add_pattern_collection_generator_cegar_options_to_feature(
    plugins::Feature& feature)
{
    feature.add_option<bool>(
        "single_goal",
        "whether to compute only a single abstraction from a random goal",
        "false");
    feature.add_option<int>(
        "max_pdb_size",
        "maximum allowed number of states in a pdb (not applied to initial "
        "goal variable pattern(s))",
        "1000000",
        plugins::Bounds("1", "infinity"));
    feature.add_option<int>(
        "max_collection_size",
        "limit for the total number of PDB entries across all PDBs (not "
        "applied to initial goal variable pattern(s))",
        "infinity",
        plugins::Bounds("1", "infinity"));
    feature.add_option<double>(
        "max_time",
        "maximum time in seconds for CEGAR pattern generation. "
        "This includes the creation of the initial PDB collection"
        " as well as the creation of the correlation matrix.",
        "infinity",
        plugins::Bounds("0.0", "infinity"));
    feature.add_option<std::shared_ptr<SubCollectionFinderFactory>>(
        "subcollection_finder_factory",
        "The subcollection finder factory.",
        "finder_trivial_factory()");
    feature.add_option<std::shared_ptr<FlawFindingStrategy>>(
        "flaw_strategy",
        "strategy used to find flaws in a policy",
        "pucs_flaw_finder()");

    add_pattern_collection_generator_options_to_feature(feature);
    add_cegar_wildcard_option_to_feature(feature);
}

namespace {

class PatternCollectionGeneratorDisjointCEGARFeature
    : public plugins::TypedFeature<
          PatternCollectionGenerator,
          PatternCollectionGeneratorDisjointCegar> {
public:
    PatternCollectionGeneratorDisjointCEGARFeature()
        : TypedFeature("ppdbs_disjoint_cegar")
    {
        add_pattern_collection_generator_cegar_options_to_feature(*this);
        utils::add_rng_options_to_feature(*this);
    } // namespace probfd::pdbs

    virtual shared_ptr<PatternCollectionGeneratorDisjointCegar>
    create_component(const plugins::Options& opts, const utils::Context&)
        const override
    {
        return plugins::make_shared_from_arg_tuples<
            PatternCollectionGeneratorDisjointCegar>(
            opts.get<bool>("use_wildcard_policies"),
            opts.get<bool>("single_goal"),
            opts.get<int>("max_pdb_size"),
            opts.get<int>("max_collection_size"),
            opts.get<double>("max_time"),
            utils::get_rng(
                std::get<0>(utils::get_rng_arguments_from_options(opts))),
            opts.get<std::shared_ptr<SubCollectionFinderFactory>>(
                "subcollection_finder_factory"),
            opts.get<std::shared_ptr<FlawFindingStrategy>>("flaw_strategy"),
            utils::get_log_arguments_from_options(opts));
    }
};

plugins::FeaturePlugin<PatternCollectionGeneratorDisjointCEGARFeature> _plugin;

} // namespace

} // namespace probfd_plugins::pdbs
