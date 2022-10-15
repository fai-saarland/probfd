#include "probfd/state_evaluator.h"

#include "plugin.h"

namespace probfd {

static PluginTypePlugin<GlobalStateEvaluator>
    _plugin("GlobalStateEvaluator", "");

} // namespace probfd
