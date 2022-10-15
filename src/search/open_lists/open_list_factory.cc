#include "open_lists/open_list_factory.h"

#include "plugin.h"

OpenListFactory::OpenListFactory() {}
OpenListFactory::~OpenListFactory() {}

static PluginTypePlugin<OpenListFactory> _plugin_type("OpenList", "");

