#include "probfd/algorithms/fdr_types.h"
#include "probfd/algorithms/open_list.h"
#include "probfd/algorithms/policy_picker.h"
#include "probfd/algorithms/successor_sampler.h"
#include "probfd/algorithms/transition_sorter.h"

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
