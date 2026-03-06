#include "downward/cli/landmarks/landmark_factory_category.h"

#include "downward/landmarks/landmark_factory.h"

#include "language/ast/internal_type_declaration.h"

using namespace downward::landmarks;

using namespace language::parser;

namespace downward::cli::landmarks {

void add_landmark_factory_category(NamespaceLevelDeclarationList& nspace)
{
    insert_shared_type_declaration<LandmarkFactory>(
        nspace,
        "LandmarkFactory",
        "A landmark factory specification is either a newly created "
        "instance or a landmark factory that has been defined previously. "
        "This page describes how one can specify a new landmark factory "
        "instance. For re-using landmark factories, see "
        "OptionSyntax#Landmark_Predefinitions.");
}

} // namespace downward::cli::landmarks
