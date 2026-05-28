#include "downward/cli/pdbs/pattern_collection_generator_genetic_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/pdbs/pattern_generator_options.h"

#include "downward/cli/utils/rng_options.h"

#include "downward/pdbs/pattern_collection_generator_genetic.h"

#include "downward/utils/logging.h"
#include "downward/utils/markup.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::pdbs;

using namespace downward::cli::plugins;

using downward::cli::pdbs::add_generator_options_to_feature;

using downward::cli::utils::add_rng_options_to_feature;

namespace {

Feature&
add_pattern_collection_generator_genetic_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_typed_feature_plugin(
        "genetic",
        &downward::cli::plugins::make_shared<
            PatternCollectionGenerator,
            PatternCollectionGeneratorGenetic,
            int,
            int,
            int,
            double,
            bool,
            std::shared_ptr<RandomNumberGenerator>,
            Verbosity>);
    f.document_title("Genetic Algorithm Patterns");
    f.document_synopsis(
        "The following paper describes the automated creation of pattern "
        "databases with a genetic algorithm. Pattern collections are "
        "initially "
        "created with a bin-packing algorithm. The genetic algorithm is "
        "used "
        "to optimize the pattern collections with an objective function "
        "that "
        "estimates the mean heuristic value of the the pattern "
        "collections. "
        "Pattern collections with higher mean heuristic estimates are more "
        "likely selected for the next generation." +
        format_conference_reference(
            {"Stefan Edelkamp"},
            "Automated Creation of Pattern Database Search Heuristics",
            "https://www.springerlink.com/content/20613345434608x1/",
            "Proceedings of the 4th Workshop on Model Checking and "
            "Artificial"
            " Intelligence (!MoChArt 2006)",
            "35-50",
            "AAAI Press",
            "2007"));

    f.document_note(
        "Note",
        "This pattern generation method uses the "
        "zero/one pattern database heuristic.");
    f.document_note(
        "Implementation Notes",
        "The standard genetic algorithm procedure as described in the "
        "paper is "
        "implemented in Fast Downward. The implementation is close to the "
        "paper.\n\n"
        " * Initialization<<BR>>"
        "In Fast Downward bin-packing with the next-fit strategy is used. "
        "A "
        "bin corresponds to a pattern which contains variables up to "
        "``pdb_max_size``. With this method each variable occurs exactly "
        "in "
        "one pattern of a collection. There are ``num_collections`` "
        "collections created.\n"
        " * Mutation<<BR>>"
        "With probability ``mutation_probability`` a bit is flipped "
        "meaning "
        "that either a variable is added to a pattern or deleted from a "
        "pattern.\n"
        " * Recombination<<BR>>"
        "Recombination isn't implemented in Fast Downward. In the paper "
        "recombination is described but not used.\n"
        " * Evaluation<<BR>>"
        "For each pattern collection the mean heuristic value is computed. "
        "For "
        "a single pattern database the mean heuristic value is the sum of "
        "all "
        "pattern database entries divided through the number of entries. "
        "Entries with infinite heuristic values are ignored in this "
        "calculation. The sum of these individual mean heuristic values "
        "yield "
        "the mean heuristic value of the collection.\n"
        " * Selection<<BR>>"
        "The higher the mean heuristic value of a pattern collection is, "
        "the "
        "more likely this pattern collection should be selected for the "
        "next "
        "generation. Therefore the mean heuristic values are normalized "
        "and "
        "converted into probabilities and Roulette Wheel Selection is "
        "used.\n",
        true);

    f.document_language_support("action costs", "supported");
    f.document_language_support("conditional effects", "not supported");
    f.document_language_support("axioms", "not supported");

    f.make_optional_argument_with_default(
        0,
        "pdb_max_size",
        "50000",
        "maximal number of states per pattern database");
    f.make_optional_argument_with_default(
        1,
        "num_collections",
        "5",
        "number of pattern collections to maintain in the genetic "
        "algorithm (population size)");
    f.make_optional_argument_with_default(
        2,
        "num_episodes",
        "30",
        "number of episodes for the genetic algorithm");
    f.make_optional_argument_with_default(
        3,
        "mutation_probability",
        "0.01",
        "probability for flipping a bit in the genetic algorithm");
    f.make_optional_argument_with_default(
        4,
        "disjoint",
        "false",
        "consider a pattern collection invalid (giving it very low "
        "fitness) if its patterns are not disjoint");
    const auto n = add_rng_options_to_feature(f, 5);
    add_generator_options_to_feature(f, n + 5);

    return f;
}

} // namespace

namespace downward::cli::pdbs {

void add_pattern_collection_generator_genetic_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_pattern_collection_generator_genetic_to_namespace(n);
}

} // namespace downward::cli::pdbs
