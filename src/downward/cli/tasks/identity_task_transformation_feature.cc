#include "downward/cli/tasks/identity_task_transformation_feature.h"

#include "language/ast/internal_function_definition.h"

#include "downward/transformations/identity_transformation.h"

using namespace std;
using namespace downward;
using namespace language::parser;

namespace downward::cli::tasks {

InternalFunctionDefinitionBase&
add_identity_task_transformation_features(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        TaskTransformation,
        IdentityTaskTransformation>>(nspace, "no_transform");

    f.document_title("Identity task transformation");
    f.document_synopsis("Applies no transformation to the task.");

    return f;
}

} // namespace downward::cli::tasks
