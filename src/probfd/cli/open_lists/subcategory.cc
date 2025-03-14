#include "downward/cli/plugins/plugin.h"

#include "probfd/cli/multi_feature_plugin.h"
#include "probfd/cli/naming_conventions.h"

#include "probfd/open_lists/fifo_open_list.h"
#include "probfd/open_lists/lifo_open_list.h"

#include "probfd/bisimulation/types.h"

#include "probfd/quotients/quotient_system.h"

#include "downward/operator_id.h"

#include <memory>

using namespace probfd::cli;

using namespace probfd;
using namespace probfd::algorithms;
using namespace probfd::open_lists;

using namespace downward::cli::plugins;

namespace {

template <template <typename> typename S, bool Fret>
using Wrapped = std::conditional_t<
    Fret,
    S<quotients::QuotientAction<OperatorID>>,
    S<OperatorID>>;

template <bool Fret>
using OpenList = Wrapped<OpenList, Fret>;

template <bool Fret>
class ProbfdOpenListCategoryPlugin
    : public TypedCategoryPlugin<OpenList<Fret>> {
public:
    ProbfdOpenListCategoryPlugin()
        : ProbfdOpenListCategoryPlugin::TypedCategoryPlugin(
              add_mdp_type_to_category<false, Fret>("ProbFDOpenList"))
    {
        this->document_synopsis("Open list.");
    }
};

template <bool Fret>
class FifoOpenListFeature
    : public TypedFeature<OpenList<Fret>, Wrapped<FifoOpenList, Fret>> {
public:
    FifoOpenListFeature()
        : FifoOpenListFeature::TypedFeature("fifo_open_list")
    {
    }

    [[nodiscard]]
    std::shared_ptr<Wrapped<FifoOpenList, Fret>>
    create_component(const Options&, const utils::Context&) const override
    {
        return std::make_shared<Wrapped<FifoOpenList, Fret>>();
    }
};

template <bool Fret>
class LifoOpenListFeature
    : public TypedFeature<OpenList<Fret>, Wrapped<LifoOpenList, Fret>> {
public:
    LifoOpenListFeature()
        : LifoOpenListFeature::TypedFeature("lifo_open_list")
    {
    }

    [[nodiscard]]
    std::shared_ptr<Wrapped<LifoOpenList, Fret>>
    create_component(const Options&, const utils::Context&) const override
    {
        return std::make_shared<Wrapped<LifoOpenList, Fret>>();
    }
};

MultiCategoryPlugin<ProbfdOpenListCategoryPlugin> _category_plugin;
MultiFeaturePlugin<FifoOpenListFeature> _plugin_fifo;
MultiFeaturePlugin<LifoOpenListFeature> _plugin_lifo;

} // namespace
