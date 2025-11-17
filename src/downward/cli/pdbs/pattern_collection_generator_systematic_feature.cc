#include "downward/cli/pdbs/pattern_collection_generator_systematic_feature.h"
#include "downward/cli/pdbs/pattern_generator_options.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/pdbs/pattern_collection_generator_systematic.h"

#include "downward/utils/logging.h"
#include "downward/utils/markup.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::pdbs;
using namespace downward::cli::plugins;

namespace {
class PatternCollectionGeneratorSystematicFeature
    : public SharedTypedFeature<
          PatternCollectionGenerator,
          int,
          bool,
          Verbosity> {
public:
    PatternCollectionGeneratorSystematicFeature()
        : TypedFeature(
              "systematic",
              &PatternCollectionGeneratorSystematicFeature::func)
    {
        document_title("Systematically generated patterns");
        document_synopsis(
            "Generates all (interesting) patterns with up to pattern_max_size "
            "variables. "
            "For details, see" +
            format_conference_reference(
                {"Florian Pommerening", "Gabriele Roeger", "Malte Helmert"},
                "Getting the Most Out of Pattern Databases for Classical "
                "Planning",
                "https://ai.dmi.unibas.ch/papers/"
                "pommerening-et-al-ijcai2013.pdf",
                "Proceedings of the Twenty-Third International Joint"
                " Conference on Artificial Intelligence (IJCAI 2013)",
                "2357-2364",
                "AAAI Press",
                "2013"));

        make_optional_argument_with_default(
            0,
            "pattern_max_size",
            "1",
            "max number of variables per pattern");
        make_optional_argument_with_default(
            1,
            "only_interesting_patterns",
            "true",
            "Only consider the union of two disjoint patterns if the union has "
            "more information than the individual patterns.");
        add_generator_options_to_feature(*this, 2);
    }

    static shared_ptr<PatternCollectionGenerator> func(
        const Context&,
        int pattern_max_size,
        bool only_interesting_patterns,
        Verbosity verbosity)
    {
        return make_shared_from_arg_tuples<
            PatternCollectionGeneratorSystematic>(
            pattern_max_size,
            only_interesting_patterns,
            verbosity);
    }
};
} // namespace

namespace downward::cli::pdbs {

void add_pattern_collection_generator_systematic_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<PatternCollectionGeneratorSystematicFeature>();
}

} // namespace downward::cli::pdbs
