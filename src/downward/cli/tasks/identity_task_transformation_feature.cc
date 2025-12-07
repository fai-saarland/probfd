#include "downward/cli/tasks/identity_task_transformation_feature.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include "downward/transformations/identity_transformation.h"

using namespace std;
using namespace downward;
using namespace downward::cli::plugins;

namespace {

InternalFunctionDefinitionBase& add_identity_task_transformation_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "no_transform",
        &cli::plugins::construct_shared<
            TaskTransformation,
            IdentityTaskTransformation>);
    f.document_title("Identity task transformation");
    f.document_synopsis("Applies no transformation to the task.");

    return f;
}

} // namespace

namespace downward::cli::tasks {

void add_identity_task_transformation_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_identity_task_transformation_to_namespace(n);
}

} // namespace downward::cli::tasks
