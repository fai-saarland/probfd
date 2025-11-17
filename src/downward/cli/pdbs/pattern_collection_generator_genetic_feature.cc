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
class PatternCollectionGeneratorGeneticFeature
    : public SharedTypedFeature<
          PatternCollectionGenerator,
          int,
          int,
          int,
          double,
          bool,
          int,
          Verbosity> {
public:
    PatternCollectionGeneratorGeneticFeature()
        : TypedFeature(
              "genetic",
              &PatternCollectionGeneratorGeneticFeature::func)
    {
        document_title("Genetic Algorithm Patterns");
        document_synopsis(
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

        document_note(
            "Note",
            "This pattern generation method uses the "
            "zero/one pattern database heuristic.");
        document_note(
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

        document_language_support("action costs", "supported");
        document_language_support("conditional effects", "not supported");
        document_language_support("axioms", "not supported");

        make_optional_argument_with_default(
            0,
            "pdb_max_size",
            "50000",
            "maximal number of states per pattern database");
        make_optional_argument_with_default(
            1,
            "num_collections",
            "5",
            "number of pattern collections to maintain in the genetic "
            "algorithm (population size)");
        make_optional_argument_with_default(
            2,
            "num_episodes",
            "30",
            "number of episodes for the genetic algorithm");
        make_optional_argument_with_default(
            3,
            "mutation_probability",
            "0.01",
            "probability for flipping a bit in the genetic algorithm");
        make_optional_argument_with_default(
            4,
            "disjoint",
            "false",
            "consider a pattern collection invalid (giving it very low "
            "fitness) if its patterns are not disjoint");
        const auto n = add_rng_options_to_feature(*this, 5);
        add_generator_options_to_feature(*this, n + 5);
    }

    static shared_ptr<PatternCollectionGenerator> func(
        const Context&,
        int pdb_max_size,
        int num_collections,
        int num_episodes,
        double mutation_probability,
        bool disjoint,
        int random_seed,
        Verbosity verbosity)
    {
        return make_shared_from_arg_tuples<PatternCollectionGeneratorGenetic>(
            pdb_max_size,
            num_collections,
            num_episodes,
            mutation_probability,
            disjoint,
            random_seed,
            verbosity);
    }
};
} // namespace

namespace downward::cli::pdbs {

void add_pattern_collection_generator_genetic_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<PatternCollectionGeneratorGeneticFeature>();
}

} // namespace downward::cli::pdbs
