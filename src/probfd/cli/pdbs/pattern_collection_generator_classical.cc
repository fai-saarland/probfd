#include "probfd/cli/pdbs/pattern_collection_generator_classical.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/utils/logging_options.h"

#include "probfd/cli/pdbs/pattern_collection_generator_options.h"

#include "probfd/pdbs/pattern_collection_generator_classical.h"

using namespace downward;
using namespace utils;

using namespace probfd::pdbs;
using namespace probfd::cli::pdbs;

using namespace downward::cli::plugins;

namespace {
class PatternCollectionGeneratorClassicalFeature
    : public SharedTypedFeature<
          PatternCollectionGenerator,
          std::shared_ptr<pdbs::PatternCollectionGenerator>,
          std::shared_ptr<SubCollectionFinderFactory>,
          Verbosity> {
public:
    PatternCollectionGeneratorClassicalFeature()
        : TypedFeature(
              "classical_generator",
              &PatternCollectionGeneratorClassicalFeature::func)
    {
        document_title("Classical Pattern Generation Adapter");
        document_synopsis(
            "Uses a classical pattern generation method on the determinization "
            "of the input task. If classical PDBs are constructed by the "
            "generation algorithm, they are used as a heuristic to compute the "
            "corresponding probability-aware PDBs.");

        make_optional_argument_with_default(
            0,
            "generator",
            "systematic()",
            "The classical pattern collection generator.");

        make_optional_argument_with_default(
            1,
            "subcollection_finder_factory",
            "finder_trivial_factory()",
            "The subcollection finder factory.");

        add_pattern_collection_generator_options_to_feature(*this, 2);
    }

    static std::shared_ptr<PatternCollectionGenerator> func(
        std::shared_ptr<pdbs::PatternCollectionGenerator> generator,
        std::shared_ptr<SubCollectionFinderFactory>
            subcollection_finder_factory,
        Verbosity verbosity)
    {
        return make_shared_from_arg_tuples<PatternCollectionGeneratorClassical>(
            std::move(generator),
            std::move(subcollection_finder_factory),
            verbosity);
    }
};
} // namespace

namespace probfd::cli::pdbs {

void add_pattern_collection_generator_classical_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<PatternCollectionGeneratorClassicalFeature>();
}

} // namespace probfd::cli::pdbs
