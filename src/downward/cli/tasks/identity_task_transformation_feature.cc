#include "downward/cli/tasks/identity_task_transformation_feature.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/transformations/identity_transformation.h"

using namespace std;
using namespace downward;
using namespace language::plugins;

namespace downward::cli::tasks {

InternalFunctionDefinitionBase&
add_identity_task_transformation_features(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "no_transform",
        &language::plugins::
            construct_shared<TaskTransformation, IdentityTaskTransformation>);
    f.document_title("Identity task transformation");
    f.document_synopsis("Applies no transformation to the task.");

    return f;
}

} // namespace downward::cli::tasks
