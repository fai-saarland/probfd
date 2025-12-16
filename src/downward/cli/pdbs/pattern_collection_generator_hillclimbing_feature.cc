#include "downward/cli/pdbs/pattern_collection_generator_hillclimbing_feature.h"

#include "downward/cli/pdbs/canonical_pdbs_heuristic_options.h"
#include "downward/cli/pdbs/pattern_collection_generator_hillclimbing_options.h"
#include "downward/cli/pdbs/pattern_generator_options.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/cli/heuristics/heuristic_options.h"

#include "downward/pdbs/canonical_pdbs_heuristic.h"
#include "downward/pdbs/pattern_collection_generator_hillclimbing.h"

#include "downward/utils/markup.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::pdbs;
using namespace language::plugins;

using downward::cli::pdbs::add_generator_options_to_feature;

using downward::cli::add_heuristic_options_to_feature;

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

InternalFunctionDefinitionBase&
add_pattern_collection_generator_hillclimbing_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "hillclimbing",
        &language::plugins::construct_shared<
            PatternCollectionGenerator,
            PatternCollectionGeneratorHillclimbing,
            int,
            int,
            int,
            int,
            FSeconds,
            std::shared_ptr<RandomNumberGenerator>,
            Verbosity>);

    f.document_title("Hill climbing");
    f.document_synopsis(
        "This algorithm uses hill climbing to generate patterns "
        "optimized for the Heuristic#Canonical_PDB heuristic. It it "
        "described "
        "in the following paper:" +
        paper_references());
    const auto n = add_hillclimbing_options_to_feature(f, 0);
    add_generator_options_to_feature(f, n);

    return f;
}

} // namespace

namespace downward::cli::pdbs {

void add_pattern_collection_generator_hillclimbing_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_pattern_collection_generator_hillclimbing_to_namespace(n);
}

} // namespace downward::cli::pdbs
