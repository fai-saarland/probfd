#include "downward/cli/pdbs/pattern_collection_generator_multiple_random_feature.h"
#include "downward/cli/pdbs/pattern_collection_generator_multiple_options.h"
#include "downward/cli/pdbs/random_pattern_options.h"
#include "downward/cli/pdbs/utils.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/pdbs/pattern_collection_generator_multiple_random.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::pdbs;
using namespace downward::cli::plugins;

namespace {
class PatternCollectionGeneratorMultipleRandomFeature
    : public SharedTypedFeature<
          PatternCollectionGenerator,
          bool,
          int,
          int,
          FSeconds,
          FSeconds,
          FSeconds,
          double,
          bool,
          int,
          Verbosity> {
public:
    PatternCollectionGeneratorMultipleRandomFeature()
        : TypedFeature(
              "random_patterns",
              &PatternCollectionGeneratorMultipleRandomFeature::func)
    {
        document_title("Multiple Random Patterns");
        document_synopsis(
            "This pattern collection generator implements the 'multiple "
            "randomized causal graph' (mRCG) algorithm described in "
            "experiments of the paper" +
            get_rovner_et_al_reference() +
            "It is an instantiation of the 'multiple algorithm framework'. "
            "To compute a pattern in each iteration, it uses the random "
            "pattern algorithm, called 'single randomized causal graph' (sRCG) "
            "in the paper. See below for descriptions of the algorithms.");

        add_random_pattern_implementation_notes_to_feature(*this);
        add_multiple_algorithm_implementation_notes_to_feature(*this);

        const auto n =
            add_random_pattern_bidirectional_option_to_feature(*this, 0);
        add_multiple_options_to_feature(*this, n);
    }

    static shared_ptr<PatternCollectionGenerator> func(
        const Context&,
        bool bidirectional,
        int max_pdb_size,
        int max_collection_size,
        FSeconds pattern_generation_max_time,
        FSeconds total_max_time,
        FSeconds stagnation_limit,
        double blacklist_trigger_percentage,
        bool enable_blacklist_on_stagnation,
        int random_seed,
        Verbosity verbosity)
    {
        return make_shared_from_arg_tuples<
            PatternCollectionGeneratorMultipleRandom>(
            bidirectional,
            max_pdb_size,
            max_collection_size,
            pattern_generation_max_time,
            total_max_time,
            stagnation_limit,
            blacklist_trigger_percentage,
            enable_blacklist_on_stagnation,
            random_seed,
            verbosity);
    }
};
} // namespace

namespace downward::cli::pdbs {

void add_pattern_collection_generator_multiple_random_feature(
    Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<PatternCollectionGeneratorMultipleRandomFeature>();
}

} // namespace downward::cli::pdbs
