#include "downward/cli/plugins/plugin.h"

#include "downward/cli/pdbs/pattern_collection_generator_multiple_options.h"
#include "downward/cli/pdbs/random_pattern_options.h"
#include "downward/cli/pdbs/utils.h"

#include "downward/pdbs/pattern_collection_generator_multiple_random.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::pdbs;
using namespace downward::cli::plugins;

namespace {

class PatternCollectionGeneratorMultipleRandomFeature
    : public TypedFeature<
          PatternCollectionGenerator,
          PatternCollectionGeneratorMultipleRandom> {
public:
    PatternCollectionGeneratorMultipleRandomFeature()
        : TypedFeature("random_patterns")
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

        add_random_pattern_bidirectional_option_to_feature(*this);
        add_multiple_options_to_feature(*this);

        add_random_pattern_implementation_notes_to_feature(*this);
        add_multiple_algorithm_implementation_notes_to_feature(*this);
    }

    virtual shared_ptr<PatternCollectionGeneratorMultipleRandom>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<
            PatternCollectionGeneratorMultipleRandom>(
            get_random_pattern_bidirectional_arguments_from_options(opts),
            get_multiple_arguments_from_options(opts));
    }
};

FeaturePlugin<PatternCollectionGeneratorMultipleRandomFeature> _plugin;

} // namespace
