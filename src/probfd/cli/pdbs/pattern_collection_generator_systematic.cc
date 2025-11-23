#include "probfd/cli/pdbs/pattern_collection_generator_systematic.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/utils/logging_options.h"

#include "probfd/pdbs/pattern_collection_generator_systematic.h"

#include "downward/utils/markup.h"

using namespace downward;
using namespace probfd::pdbs;

using namespace downward::cli::plugins;

using downward::cli::utils::add_log_options_to_feature;

namespace {

InternalFunctionDefinitionBase&
add_pattern_collection_generator_systematic_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "psystematic",
        &downward::cli::plugins::construct_shared<
            PatternCollectionGenerator,
            PatternCollectionGeneratorSystematic,
            int,
            bool,
            utils::Verbosity>);

    f.document_title("Systematically generated patterns");
    f.document_synopsis(
        "Generates all (interesting) patterns with up to pattern_max_size "
        "variables. For details, see" +
        utils::format_conference_reference(
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

    f.make_optional_argument_with_default(
        0,
        "pattern_max_size",
        "1",
        "max number of variables per pattern");
    f.make_optional_argument_with_default(
        1,
        "only_interesting_patterns",
        "true",
        "Only consider the union of two disjoint patterns if the union has "
        "more information than the individual patterns.");
    add_log_options_to_feature(f, 2);

    return f;
}

} // namespace

namespace probfd::cli::pdbs {

void add_pattern_collection_generator_systematic_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_pattern_collection_generator_systematic_to_namespace(n);
}

} // namespace probfd::cli::pdbs
