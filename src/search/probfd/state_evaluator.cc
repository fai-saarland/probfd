#include "state_evaluator.h"

#include "../plugin.h"

namespace probabilistic {

static PluginTypePlugin<GlobalStateEvaluator>
    _plugin("GlobalStateEvaluator", "");

} // namespace probabilistic
