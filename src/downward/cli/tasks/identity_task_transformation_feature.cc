#include "downward/cli/tasks/identity_task_transformation_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/transformations/identity_transformation.h"

using namespace std;
using namespace downward;
using namespace downward::cli::plugins;

namespace {
class IdentityTaskTransformationFeature
    : public SharedTypedFeature<TaskTransformation> {
public:
    IdentityTaskTransformationFeature()
        : SharedTypedFeature("no_transform")
    {
    }

    [[nodiscard]]
    shared_ptr<TaskTransformation>
    create_component(const Options&, const utils::Context&) const override
    {
        return std::make_shared<IdentityTaskTransformation>();
    }
};
} // namespace

namespace downward::cli::tasks {

void add_identity_task_transformation_features(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<IdentityTaskTransformationFeature>();
}

} // namespace downward::cli::tasks
