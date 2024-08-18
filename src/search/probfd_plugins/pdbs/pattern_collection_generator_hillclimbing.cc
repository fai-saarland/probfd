#include "probfd_plugins/pdbs/pattern_collection_generator.h"

#include "probfd/pdbs/pattern_collection_generator_hillclimbing.h"

#include "downward/utils/rng_options.h"

#include "downward/plugins/plugin.h"

using namespace utils;

using namespace probfd_plugins::pdbs;
using namespace probfd::pdbs;

namespace {

class PatternCollectionGeneratorHillclimbingFeature
    : public plugins::TypedFeature<
          PatternCollectionGenerator,
          PatternCollectionGeneratorHillclimbing> {
public:
    PatternCollectionGeneratorHillclimbingFeature()
        : TypedFeature("hillclimbing_probabilistic")
    {
        add_option<std::shared_ptr<PatternCollectionGenerator>>(
            "initial_generator",
            "generator for the initial pattern database ",
            "psystematic(pattern_max_size=1)");

        add_option<std::shared_ptr<SubCollectionFinderFactory>>(
            "subcollection_finder_factory",
            "The subcollection finder factory.",
            "finder_trivial_factory()");

        add_option<int>(
            "pdb_max_size",
            "maximal number of states per pattern database ",
            "2M",
            plugins::Bounds("1", "infinity"));
        add_option<int>(
            "collection_max_size",
            "maximal number of states in the pattern collection",
            "10M",
            plugins::Bounds("1", "infinity"));
        add_option<int>(
            "search_space_max_size",
            "maximal number of states in the pattern search space",
            "30M",
            plugins::Bounds("1", "infinity"));
        add_option<int>(
            "num_samples",
            "number of samples (random states) on which to evaluate each "
            "candidate pattern collection",
            "1000",
            plugins::Bounds("1", "infinity"));
        add_option<int>(
            "min_improvement",
            "minimum number of samples on which a candidate pattern "
            "collection must improve on the current one to be considered "
            "as the next pattern collection ",
            "10",
            plugins::Bounds("1", "infinity"));
        add_option<double>(
            "max_time",
            "maximum time in seconds for improving the initial pattern "
            "collection via hill climbing. If set to 0, no hill climbing "
            "is performed at all. Note that this limit only affects hill "
            "climbing. Use max_time_dominance_pruning to limit the time "
            "spent for pruning dominated patterns.",
            "infinity",
            plugins::Bounds("0.0", "infinity"));

        utils::add_rng_options_to_feature(*this);
        add_pattern_collection_generator_options_to_feature(*this);
    }

    std::shared_ptr<PatternCollectionGeneratorHillclimbing> create_component(
        const plugins::Options& opts,
        const utils::Context& context) const override
    {
        if (opts.get<int>("min_improvement") > opts.get<int>("num_samples")) {
            context.error("Minimum improvement must not be higher than number "
                          "of samples");
        }

        return plugins::make_shared_from_arg_tuples<
            PatternCollectionGeneratorHillclimbing>(
            opts.get<std::shared_ptr<PatternCollectionGenerator>>(
                "initial_generator"),
            opts.get<std::shared_ptr<SubCollectionFinderFactory>>(
                "subcollection_finder_factory"),
            opts.get<int>("pdb_max_size"),
            opts.get<int>("collection_max_size"),
            opts.get<int>("search_space_max_size"),
            opts.get<int>("num_samples"),
            opts.get<int>("min_improvement"),
            opts.get<double>("max_time"),
            utils::get_rng(
                std::get<0>(utils::get_rng_arguments_from_options(opts))),
            get_collection_generator_arguments_from_options(opts));
    }
};

plugins::FeaturePlugin<PatternCollectionGeneratorHillclimbingFeature> _plugin;

} // namespace
