#include "probfd/cli/open_lists/subcategory.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include "probfd/cli/naming_conventions.h"

#include "probfd/open_lists/fifo_open_list.h"
#include "probfd/open_lists/lifo_open_list.h"

#include "probfd/bisimulation/types.h"

#include "probfd/quotients/quotient_system.h"

#include "downward/operator_id.h"
#include "downward/tuple_utils.h"

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
class FifoOpenListFeature
    : public InternalFunctionDefinition<std::shared_ptr<OpenList<Fret>>()> {
public:
    FifoOpenListFeature()
        : FifoOpenListFeature::InternalFunctionDefinition(
              add_mdp_type_to_option<false, Fret>("fifo_open_list"),
              &FifoOpenListFeature::create_open_list)
    {
    }

    [[nodiscard]]
    static std::shared_ptr<OpenList<Fret>> create_open_list()
    {
        return std::make_shared<Wrapped<FifoOpenList, Fret>>();
    }
};

template <bool Fret>
class LifoOpenListFeature
    : public InternalFunctionDefinition<std::shared_ptr<OpenList<Fret>>()> {
public:
    LifoOpenListFeature()
        : LifoOpenListFeature::InternalFunctionDefinition(
              add_mdp_type_to_option<false, Fret>("lifo_open_list"),
              &LifoOpenListFeature::create_open_list)
    {
    }

    [[nodiscard]]
    static std::shared_ptr<OpenList<Fret>> create_open_list()
    {
        return std::make_shared<Wrapped<LifoOpenList, Fret>>();
    }
};
} // namespace

namespace probfd::cli::open_lists {

template <bool Fret>
using OpenList = Wrapped<probfd::algorithms::OpenList, Fret>;

void add_open_list_categories(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_type_declarations<open_lists::OpenList>(
        []<bool Fret>() {
            return add_mdp_type_to_category<false, Fret>("ProbFDOpenList");
        },
        []<bool>() { return "Open list."; });
}

void add_open_list_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_function_definitions<FifoOpenListFeature>();
    n.insert_function_definitions<LifoOpenListFeature>();
}

} // namespace probfd::cli::open_lists
