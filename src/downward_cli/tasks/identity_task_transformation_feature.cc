#include "downward_cli/tasks/identity_task_transformation_feature.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "downward/transformations/identity_transformation.h"

using namespace std;
using namespace downward;

using namespace language;
using namespace language::plugins;

namespace {
class IdentityTaskTransformationFeature
    : public TypedFeature<TaskTransformation> {
public:
    IdentityTaskTransformationFeature()
        : TypedFeature("no_transform")
    {
    }

    [[nodiscard]]
    shared_ptr<TaskTransformation>
    create_component(const Options&, const Context&) const override
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
