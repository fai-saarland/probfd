#include "probfd_plugins/multi_feature_plugin.h"
#include "probfd_plugins/naming_conventions.h"

#include "probfd/open_lists/fifo_open_list.h"
#include "probfd/open_lists/lifo_open_list.h"

#include "probfd/bisimulation/types.h"

#include "probfd/quotients/quotient_system.h"

#include "downward/plugins/plugin.h"

#include <memory>

using namespace plugins;

using namespace probfd_plugins;

using namespace probfd;
using namespace probfd::algorithms;
using namespace probfd::open_lists;

namespace {

template <template <typename> typename S, bool Bisimulation, bool Fret>
using Wrapped = std::conditional_t<
    Bisimulation,
    std::conditional_t<
        Fret,
        S<quotients::QuotientAction<bisimulation::QuotientAction>>,
        S<bisimulation::QuotientAction>>,
    std::conditional_t<
        Fret,
        S<quotients::QuotientAction<OperatorID>>,
        S<OperatorID>>>;

template <bool Bisimulation, bool Fret>
using OpenList = Wrapped<OpenList, Bisimulation, Fret>;

template <bool Bisimulation, bool Fret>
class OpenListCategoryPlugin
    : public TypedCategoryPlugin<OpenList<Bisimulation, Fret>> {
public:
    OpenListCategoryPlugin()
        : OpenListCategoryPlugin::TypedCategoryPlugin(
              add_mdp_type_to_category<Bisimulation, Fret>("ProbFDOpenList"))
    {
        this->document_synopsis("Open list.");
    }
};

template <bool Bisimulation, bool Fret>
class FifoOpenListFeature
    : public TypedFeature<
          OpenList<Bisimulation, Fret>,
          Wrapped<FifoOpenList, Bisimulation, Fret>> {
public:
    FifoOpenListFeature()
        : FifoOpenListFeature::TypedFeature(
              add_mdp_type_to_option<Bisimulation, Fret>("fifo_open_list"))
    {
    }

    [[nodiscard]]
    std::shared_ptr<Wrapped<FifoOpenList, Bisimulation, Fret>>
    create_component(const Options&, const utils::Context&) const override
    {
        return std::make_shared<Wrapped<FifoOpenList, Bisimulation, Fret>>();
    }
};

template <bool Bisimulation, bool Fret>
class LifoOpenListFeature
    : public TypedFeature<
          OpenList<Bisimulation, Fret>,
          Wrapped<LifoOpenList, Bisimulation, Fret>> {
public:
    LifoOpenListFeature()
        : LifoOpenListFeature::TypedFeature(
              add_mdp_type_to_option<Bisimulation, Fret>("lifo_open_list"))
    {
    }

    [[nodiscard]]
    std::shared_ptr<Wrapped<LifoOpenList, Bisimulation, Fret>>
    create_component(const Options&, const utils::Context&) const override
    {
        return std::make_shared<Wrapped<LifoOpenList, Bisimulation, Fret>>();
    }
};

MultiCategoryPlugin<OpenListCategoryPlugin> _category_plugin;
MultiFeaturePlugin<FifoOpenListFeature> _plugin_fifo;
MultiFeaturePlugin<LifoOpenListFeature> _plugin_lifo;

} // namespace