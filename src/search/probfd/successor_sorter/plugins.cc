#include "probfd/successor_sorter/vdiff_sorter_factory.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace successor_sorting {

static PluginTypePlugin<TaskSuccessorSorterFactory>
    _type_plugin("TaskSuccessorSorterFactory", "");

static Plugin<TaskSuccessorSorterFactory> _plugin_value_gap(
    "value_gap_sort_factory",
    options::parse<TaskSuccessorSorterFactory, VDiffSorterFactory>);

} // namespace successor_sorting
} // namespace probfd