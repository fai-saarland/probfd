#include "downward/cli/pdbs/pattern_collection_generator_systematic_feature.h"
#include "downward/cli/pdbs/pattern_generator_options.h"

#include "language/ast/compilation_context.h"
#include "language/ast/internal_function_definition.h"

#include "downward/pdbs/pattern_collection_generator_systematic.h"

#include "downward/utils/logging.h"
#include "downward/utils/markup.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::pdbs;
using namespace language::parser;

namespace downward::cli::pdbs {

InternalFunctionDefinitionBase&
add_pattern_collection_generator_systematic_feature(
    NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        PatternCollectionGenerator,
        PatternCollectionGeneratorSystematic,
        int,
        bool,
        Verbosity>>(nspace, "systematic");

    f.document_title("Systematically generated patterns");
    f.document_synopsis(
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
    add_generator_options_to_feature(f, 2);

    return f;
}

} // namespace downward::cli::pdbs
