#include "downward/cli/tasks/identity_task_transformation_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/transformations/identity_transformation.h"

using namespace std;
using namespace downward;
using namespace downward::cli::plugins;

namespace {
class IdentityTaskTransformationFeature
    : public SharedTypedFeature<TaskTransformation> {
public:
    IdentityTaskTransformationFeature()
        : TypedFeature("no_transform", &IdentityTaskTransformationFeature::func)
    {
    }

    [[nodiscard]]
    static shared_ptr<TaskTransformation> func(const utils::Context&)
    {
        return std::make_shared<IdentityTaskTransformation>();
    }
};
} // namespace

namespace downward::cli::tasks {

void add_identity_task_transformation_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<IdentityTaskTransformationFeature>();
}

} // namespace downward::cli::tasks
