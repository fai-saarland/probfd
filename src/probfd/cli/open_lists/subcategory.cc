#include "probfd/cli/open_lists/subcategory.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/cli/naming_conventions.h"

#include "probfd/open_lists/fifo_open_list.h"
#include "probfd/open_lists/lifo_open_list.h"

#include "probfd/bisimulation/types.h"

#include "probfd/quotients/quotient_system.h"

#include "downward/operator_id.h"

#include <memory>

using namespace probfd::cli;

using namespace downward;

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
    : public SharedTypedCategoryPlugin<OpenList<Fret>> {
public:
    ProbfdOpenListCategoryPlugin()
        : ProbfdOpenListCategoryPlugin::SharedTypedCategoryPlugin(
              add_mdp_type_to_category<false, Fret>("ProbFDOpenList"))
    {
        this->document_synopsis("Open list.");
    }
};

template <bool Fret>
class FifoOpenListFeature : public SharedTypedFeature<OpenList<Fret>> {
public:
    FifoOpenListFeature()
        : FifoOpenListFeature::SharedTypedFeature(
              add_mdp_type_to_option<false, Fret>("fifo_open_list"))
    {
    }

    [[nodiscard]]
    std::shared_ptr<OpenList<Fret>>
    create_component(const Options&, const utils::Context&) const override
    {
        return std::make_shared<Wrapped<FifoOpenList, Fret>>();
    }
};

template <bool Fret>
class LifoOpenListFeature : public SharedTypedFeature<OpenList<Fret>> {
public:
    LifoOpenListFeature()
        : LifoOpenListFeature::SharedTypedFeature(
              add_mdp_type_to_option<false, Fret>("lifo_open_list"))
    {
    }

    [[nodiscard]]
    std::shared_ptr<OpenList<Fret>>
    create_component(const Options&, const utils::Context&) const override
    {
        return std::make_shared<Wrapped<LifoOpenList, Fret>>();
    }
};
} // namespace

namespace probfd::cli::open_lists {

void add_open_list_features(RawRegistry& raw_registry)
{
    raw_registry.insert_category_plugins<ProbfdOpenListCategoryPlugin>();
    raw_registry.insert_feature_plugins<FifoOpenListFeature>();
    raw_registry.insert_feature_plugins<LifoOpenListFeature>();
}

} // namespace probfd::cli::open_lists
