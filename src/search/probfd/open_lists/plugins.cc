#include "probfd/open_lists/fifo_open_list_factory.h"
#include "probfd/open_lists/lifo_open_list_factory.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace open_lists {

static PluginTypePlugin<TaskOpenListFactory>
    _type_plugin("TaskOpenListFactory", "");

static std::shared_ptr<TaskOpenListFactory>
_parse_fifo(options::OptionParser& parser)
{
    if (parser.dry_run()) return nullptr;
    if (parser.help_mode()) return nullptr;

    return std::make_shared<FifoOpenListFactory>();
}

static std::shared_ptr<TaskOpenListFactory>
_parse_lifo(options::OptionParser& parser)
{
    if (parser.dry_run()) return nullptr;
    if (parser.help_mode()) return nullptr;

    return std::make_shared<LifoOpenListFactory>();
}

static Plugin<TaskOpenListFactory>
    _plugin_fifo("fifo_open_list_factory", _parse_fifo);

static Plugin<TaskOpenListFactory>
    _plugin_lifo("lifo_open_list_factory", _parse_lifo);

} // namespace open_lists
} // namespace probfd