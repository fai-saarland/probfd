#include "probfd_cli/pdbs/pattern_collection_generator_classical.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "downward_cli/utils/logging_options.h"

#include "probfd_cli/pdbs/pattern_collection_generator_options.h"

#include "probfd/pdbs/pattern_collection_generator_classical.h"

using namespace downward;
using namespace utils;

using namespace probfd::pdbs;
using namespace probfd::cli::pdbs;

using namespace language;
using namespace language::plugins;

using downward::cli::utils::get_log_arguments_from_options;

namespace {
class PatternCollectionGeneratorClassicalFeature
    : public TypedFeature<PatternCollectionGenerator> {
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

    std::shared_ptr<PatternCollectionGenerator>
    create_component(const Options& opts, const Context& context) const override
    {
        return make_shared_from_arg_tuples<PatternCollectionGeneratorClassical>(
            opts.get<std::shared_ptr<::pdbs::PatternCollectionGenerator>>(
                context,
                "generator"),
            opts.get<std::shared_ptr<SubCollectionFinderFactory>>(
                context,
                "subcollection_finder_factory"),
            get_log_arguments_from_options(context, opts));
    }
};
} // namespace

namespace probfd::cli::pdbs {

void add_pattern_collection_generator_classical_feature(
    RawRegistry& raw_registry)
{
    raw_registry
        .insert_feature_plugin<PatternCollectionGeneratorClassicalFeature>();
}

} // namespace probfd::cli::pdbs
