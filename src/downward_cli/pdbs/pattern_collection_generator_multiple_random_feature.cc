#include "downward_cli/pdbs/pattern_collection_generator_multiple_random_feature.h"
#include "downward_cli/pdbs/pattern_collection_generator_multiple_options.h"
#include "downward_cli/pdbs/random_pattern_options.h"
#include "downward_cli/pdbs/utils.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "downward/pdbs/pattern_collection_generator_multiple_random.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::pdbs;

using namespace language;
using namespace language::plugins;

namespace {
class PatternCollectionGeneratorMultipleRandomFeature
    : public TypedFeature<PatternCollectionGenerator> {
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

    shared_ptr<PatternCollectionGenerator>
    create_component(const Options& opts, const Context& context) const override
    {
        return make_shared_from_arg_tuples<
            PatternCollectionGeneratorMultipleRandom>(
            get_random_pattern_bidirectional_arguments_from_options(
                context,
                opts),
            get_multiple_arguments_from_options(context, opts));
    }
};
} // namespace

namespace downward::cli::pdbs {

void add_pattern_collection_generator_multiple_random_feature(
    RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<
        PatternCollectionGeneratorMultipleRandomFeature>();
}

} // namespace downward::cli::pdbs
