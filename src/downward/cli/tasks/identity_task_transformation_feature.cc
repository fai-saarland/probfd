#include "downward/cli/plugins/plugin.h"

#include "downward/transformations/identity_transformation.h"

using namespace std;
using namespace downward::cli::plugins;

namespace {

class IdentityTaskTransformationFeature
    : public TypedFeature<TaskTransformation, IdentityTaskTransformation> {
public:
    IdentityTaskTransformationFeature()
        : TypedFeature("no_transform")
    {
    }

    [[nodiscard]]
    shared_ptr<IdentityTaskTransformation>
    create_component(const Options&, const utils::Context&) const override
    {
        return std::make_shared<IdentityTaskTransformation>();
    }
};

FeaturePlugin<IdentityTaskTransformationFeature> plugin;

} // namespace
