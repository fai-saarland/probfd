#include "downward/cli/landmarks/landmark_factory_rpg_sasp_feature.h"

#include "language/ast/compilation_context.h"
#include "language/ast/internal_function_definition.h"

#include "downward/cli/landmarks/landmark_factory_options.h"

#include "downward/landmarks/landmark_factory_rpg_sasp.h"

using namespace std;
using namespace downward::landmarks;
using namespace downward::utils;

using namespace language::parser;

using downward::cli::landmarks::add_landmark_factory_options_to_feature;

using downward::cli::landmarks::add_use_orders_option_to_feature;

namespace downward::cli::landmarks {

InternalFunctionDefinitionBase&
add_landmark_factory_rpg_sasp_feature(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        LandmarkFactory,
        LandmarkFactoryRpgSasp,
        bool,
        bool,
        Verbosity>>(nspace, "lm_rhw");
    f.document_title("RHW Landmarks");
    f.document_synopsis(
        "The landmark generation method introduced by "
        "Richter, Helmert and Westphal (AAAI 2008).");
    f.document_language_support("conditional_effects", "supported");

    f.make_optional_argument_with_default(
        0,
        "disjunctive_landmarks",
        "true",
        "keep disjunctive landmarks");
    const auto n = add_use_orders_option_to_feature(f, 1);
    add_landmark_factory_options_to_feature(f, n + 1);

    return f;
}

} // namespace downward::cli::landmarks
