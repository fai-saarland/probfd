#include "downward/cli/landmarks/landmark_factory_zhu_givan_feature.h"

#include "language/ast/compilation_context.h"
#include "language/ast/internal_function_definition.h"

#include "downward/cli/landmarks/landmark_factory_options.h"

#include "downward/landmarks/landmark_factory_zhu_givan.h"

using namespace std;
using namespace downward::landmarks;
using namespace downward::utils;

using namespace language::parser;

using downward::cli::landmarks::add_use_orders_option_to_feature;

using downward::cli::landmarks::add_landmark_factory_options_to_feature;

namespace downward::cli::landmarks {

InternalFunctionDefinitionBase&
add_landmark_factory_zhu_givan_feature(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        LandmarkFactory,
        LandmarkFactoryZhuGivan,
        bool,
        Verbosity>>(nspace, "lm_zg");

    f.document_title("Zhu/Givan Landmarks");
    f.document_synopsis(
        "The landmark generation method introduced by "
        "Zhu & Givan (ICAPS 2003 Doctoral Consortium).");

    const auto n = add_use_orders_option_to_feature(f, 0);
    add_landmark_factory_options_to_feature(f, n);

    // TODO: Make sure that conditional effects are indeed supported.
    f.document_language_support(
        "conditional_effects",
        "We think they are supported, but this is not 100% sure.");

    return f;
}

} // namespace downward::cli::landmarks
