#include "downward/cli/pdbs/pattern_collection_generator_hillclimbing_feature.h"

#include "downward/cli/pdbs/canonical_pdbs_heuristic_options.h"
#include "downward/cli/pdbs/pattern_collection_generator_hillclimbing_options.h"
#include "downward/cli/pdbs/pattern_generator_options.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/cli/heuristics/heuristic_options.h"

#include "downward/pdbs/canonical_pdbs_heuristic.h"
#include "downward/pdbs/pattern_collection_generator_hillclimbing.h"

#include "downward/utils/markup.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::pdbs;
using namespace downward::cli::plugins;

using downward::cli::pdbs::add_generator_options_to_feature;
using downward::cli::pdbs::get_generator_arguments_from_options;

using downward::cli::add_heuristic_options_to_feature;
using downward::cli::get_heuristic_arguments_from_options;

namespace {
std::string paper_references()
{
    return format_conference_reference(
               {"Patrik Haslum",
                "Adi Botea",
                "Malte Helmert",
                "Blai Bonet",
                "Sven Koenig"},
               "Domain-Independent Construction of Pattern Database Heuristics "
               "for"
               " Cost-Optimal Planning",
               "https://ai.dmi.unibas.ch/papers/haslum-et-al-aaai07.pdf",
               "Proceedings of the 22nd AAAI Conference on Artificial"
               " Intelligence (AAAI 2007)",
               "1007-1012",
               "AAAI Press",
               "2007") +
           "For implementation notes, see:" +
           format_conference_reference(
               {"Silvan Sievers", "Manuela Ortlieb", "Malte Helmert"},
               "Efficient Implementation of Pattern Database Heuristics for"
               " Classical Planning",
               "https://ai.dmi.unibas.ch/papers/sievers-et-al-socs2012.pdf",
               "Proceedings of the Fifth Annual Symposium on Combinatorial"
               " Search (SoCS 2012)",
               "105-111",
               "AAAI Press",
               "2012");
}

class PatternCollectionGeneratorHillclimbingFeature
    : public TypedFeature<PatternCollectionGenerator> {
public:
    PatternCollectionGeneratorHillclimbingFeature()
        : TypedFeature("hillclimbing")
    {
        document_title("Hill climbing");
        document_synopsis(
            "This algorithm uses hill climbing to generate patterns "
            "optimized for the Heuristic#Canonical_PDB heuristic. It it "
            "described "
            "in the following paper:" +
            paper_references());
        add_hillclimbing_options_to_feature(*this);
        add_generator_options_to_feature(*this);
    }

    virtual shared_ptr<PatternCollectionGenerator>
    create_component(const Options& opts, const Context& context) const override
    {
        if (opts.get<int>("min_improvement") > opts.get<int>("num_samples")) {
            context.error(
                "Minimum improvement must not be higher than number "
                "of samples");
        }

        return make_shared_from_arg_tuples<
            PatternCollectionGeneratorHillclimbing>(
            get_hillclimbing_arguments_from_options(opts),
            get_generator_arguments_from_options(opts));
    }
};
} // namespace

namespace downward::cli::pdbs {

void add_pattern_collection_generator_hillclimbing_feature(
    RawRegistry& raw_registry)
{
    raw_registry
        .insert_feature_plugin<PatternCollectionGeneratorHillclimbingFeature>();
}

} // namespace downward::cli::pdbs
