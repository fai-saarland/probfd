#include "downward/cli/open_lists/open_list_factory_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/open_list.h"

#include "downward/task_dependent_factory.h"

using namespace std;

using namespace downward::cli;

namespace {
template <typename T>
class OpenListFactoryCategoryPlugin;

template <>
class OpenListFactoryCategoryPlugin<downward::StateOpenListEntry>
    : public plugins::TypedCategoryPlugin<downward::TaskDependentFactory<
          downward::OpenList<downward::StateOpenListEntry>>> {
public:
    OpenListFactoryCategoryPlugin()
        : TypedCategoryPlugin("StateOpenList")
    {
        // TODO: use document_synopsis() for the wiki page.
    }
};

template <>
class OpenListFactoryCategoryPlugin<downward::EdgeOpenListEntry>
    : public plugins::TypedCategoryPlugin<downward::TaskDependentFactory<
          downward::OpenList<downward::EdgeOpenListEntry>>> {
public:
    OpenListFactoryCategoryPlugin()
        : TypedCategoryPlugin("EdgeOpenList")
    {
        // TODO: use document_synopsis() for the wiki page.
    }
};
}

namespace downward::cli::open_lists {

void add_open_list_factory_category(plugins::RawRegistry& raw_registry)
{
    raw_registry.insert_category_plugin<
        OpenListFactoryCategoryPlugin<downward::StateOpenListEntry>>();
    raw_registry.insert_category_plugin<
        OpenListFactoryCategoryPlugin<downward::EdgeOpenListEntry>>();
}

} // namespace
