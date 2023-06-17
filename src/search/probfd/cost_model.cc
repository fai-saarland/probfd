#include "probfd/cost_model.h"

#include "downward/plugins/plugin.h"

namespace probfd {

std::shared_ptr<CostModel> g_cost_model;

static class CostModelCategoryPlugin
    : public plugins::TypedCategoryPlugin<CostModel> {
public:
    CostModelCategoryPlugin()
        : TypedCategoryPlugin("CostModel")
    {
    }
} _category_plugin;

} // namespace probfd
