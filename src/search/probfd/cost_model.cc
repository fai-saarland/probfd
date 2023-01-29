#include "probfd/cost_model.h"

#include "plugin.h"

namespace probfd {

std::shared_ptr<CostModel> g_cost_model;

static PluginTypePlugin<CostModel> _plugin_type("CostModel", "");

} // namespace probfd
