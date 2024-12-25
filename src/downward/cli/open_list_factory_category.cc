#include "downward/cli/plugins/plugin.h"

#include "downward/open_list_factory.h"

using namespace std;

namespace downward::cli {

class OpenListFactoryCategoryPlugin
    : public plugins::TypedCategoryPlugin<OpenListFactory> {
public:
    OpenListFactoryCategoryPlugin()
        : TypedCategoryPlugin("OpenList")
    {
        // TODO: use document_synopsis() for the wiki page.
    }
} _category_plugin;

} // namespace downward::cli