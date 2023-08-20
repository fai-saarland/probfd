#include "probfd/engines/fdr_types.h"
#include "probfd/engines/open_list.h"
#include "probfd/engines/policy_picker.h"
#include "probfd/engines/successor_sampler.h"
#include "probfd/engines/transition_sorter.h"

#include "downward/plugins/plugin.h"

namespace probfd {

static class FDROpenListCategoryPlugin
    : public plugins::TypedCategoryPlugin<FDROpenList> {
public:
    FDROpenListCategoryPlugin()
        : TypedCategoryPlugin("FDROpenList")
    {
    }
} _category_plugin_open_list;

} // namespace probfd
