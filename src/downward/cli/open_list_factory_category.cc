#include "downward/cli/plugins/plugin.h"

#include "downward/open_list_factory.h"

using namespace std;

using namespace downward::cli;

namespace {

class OpenListFactoryCategoryPlugin
    : public plugins::TypedCategoryPlugin<downward::OpenListFactory> {
public:
    OpenListFactoryCategoryPlugin()
        : TypedCategoryPlugin("OpenList")
    {
        // TODO: use document_synopsis() for the wiki page.
    }
};

OpenListFactoryCategoryPlugin _category_plugin;

} // namespace
