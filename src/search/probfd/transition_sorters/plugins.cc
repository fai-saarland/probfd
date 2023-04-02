#include "probfd/transition_sorters/vdiff_sorter_factory.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace transition_sorters {

static PluginTypePlugin<TaskTransitionSorterFactory>
    _type_plugin("TaskTransitionSorterFactory", "");

static Plugin<TaskTransitionSorterFactory> _plugin_value_gap(
    "value_gap_sort_factory",
    options::parse<TaskTransitionSorterFactory, VDiffSorterFactory>);

} // namespace transition_sorters
} // namespace probfd