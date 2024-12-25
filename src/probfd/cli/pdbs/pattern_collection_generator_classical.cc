#include "downward/cli/plugins/plugin.h"

#include "downward/cli/utils/logging_options.h"

#include "probfd/cli/pdbs/pattern_collection_generator.h"

#include "probfd/pdbs/pattern_collection_generator_classical.h"

using namespace utils;

using namespace probfd::pdbs;
using namespace probfd::cli::pdbs;

using namespace downward::cli::plugins;

using downward::cli::utils::get_log_arguments_from_options;

namespace {

class PatternCollectionGeneratorClassicalFeature
    : public TypedFeature<
          PatternCollectionGenerator,
          PatternCollectionGeneratorClassical> {
public:
    PatternCollectionGeneratorClassicalFeature()
        : TypedFeature("classical_generator")
    {
        document_title("Classical Pattern Generation Adapter");
        document_synopsis(
            "Uses a classical pattern generation method on the determinization "
            "of the input task. If classical PDBs are constructed by the "
            "generation algorithm, they are used as a heuristic to compute the "
            "corresponding probability-aware PDBs.");

        add_pattern_collection_generator_options_to_feature(*this);

        add_option<std::shared_ptr<::pdbs::PatternCollectionGenerator>>(
            "generator",
            "The classical pattern collection generator.",
            "systematic()");

        add_option<std::shared_ptr<SubCollectionFinderFactory>>(
            "subcollection_finder_factory",
            "The subcollection finder factory.",
            "finder_trivial_factory()");
    }

    std::shared_ptr<PatternCollectionGeneratorClassical>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<PatternCollectionGeneratorClassical>(
            opts.get<std::shared_ptr<::pdbs::PatternCollectionGenerator>>(
                "generator"),
            opts.get<std::shared_ptr<SubCollectionFinderFactory>>(
                "subcollection_finder_factory"),
            get_log_arguments_from_options(opts));
    }
};

FeaturePlugin<PatternCollectionGeneratorClassicalFeature> _plugin;

} // namespace
