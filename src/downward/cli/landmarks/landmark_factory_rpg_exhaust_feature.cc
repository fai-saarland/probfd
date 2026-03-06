#include "downward/cli/landmarks/landmark_factory_rpg_exhaust_feature.h"

#include "language/ast/compilation_context.h"
#include "language/ast/internal_function_definition.h"

#include "downward/cli/landmarks/landmark_factory_options.h"

#include "downward/landmarks/landmark_factory_rpg_exhaust.h"

using namespace std;
using namespace downward::utils;

using namespace language::parser;

using downward::cli::landmarks::add_landmark_factory_options_to_feature;

using namespace downward::landmarks;

namespace downward::cli::landmarks {

InternalFunctionDefinitionBase&
add_landmark_factory_rpg_exhaust_feature(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        LandmarkFactory,
        LandmarkFactoryRpgExhaust,
        bool,
        Verbosity>>(nspace, "lm_exhaust");
    f.document_title("Exhaustive Landmarks");
    f.document_synopsis(
        "Exhaustively checks for each fact if it is a landmark."
        "This check is done using relaxed planning.");

    f.document_language_support(
        "conditional_effects",
        "ignored, i.e. not supported");

    f.make_optional_argument_with_default(
        0,
        "use_unary_relaxation",
        "false",
        "compute landmarks of the unary relaxation, i.e., landmarks "
        "for the delete relaxation of a task transformation such that each "
        "operator is split into one operator for each of its effects. This "
        "kind of landmark was previously known as \"causal landmarks\". "
        "Setting the option to true can reduce the overall number of "
        "landmarks, which can make the search more memory-efficient but "
        "potentially less informative.");
    add_landmark_factory_options_to_feature(f, 1);

    return f;
}

} // namespace downward::cli::landmarks
