#include "probfd/analysis_objectives/analysis_objective.h"

#include "plugin.h"

namespace probfd {

std::shared_ptr<analysis_objectives::AnalysisObjective> g_analysis_objective;

static PluginTypePlugin<analysis_objectives::AnalysisObjective>
    _plugin_type("AnalysisObjective", "");

} // namespace probfd
