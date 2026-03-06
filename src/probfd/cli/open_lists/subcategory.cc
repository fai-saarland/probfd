#include "probfd/cli/open_lists/subcategory.h"

#include "probfd/cli/naming_conventions.h"

#include "probfd/open_lists/fifo_open_list.h"
#include "probfd/open_lists/lifo_open_list.h"

#include "probfd/bisimulation/types.h"

#include "probfd/quotients/quotient_system.h"

#include "downward/operator_id.h"
#include "downward/tuple_utils.h"

#include "language/ast/internal_function_definition.h"
#include "language/ast/internal_type_declaration.h"

#include <memory>

using namespace probfd::cli;

using namespace downward;

using namespace probfd;
using namespace probfd::algorithms;
using namespace probfd::open_lists;

using namespace language::parser;

namespace {
template <template <typename> typename S, bool Fret>
using Wrapped = std::conditional_t<
    Fret,
    S<quotients::QuotientAction<OperatorID>>,
    S<OperatorID>>;

template <bool Fret>
using OpenList = Wrapped<OpenList, Fret>;

template <bool Fret>
std::shared_ptr<OpenList<Fret>> create_fifo_open_list()
{
    return std::make_shared<Wrapped<FifoOpenList, Fret>>();
}

template <bool Fret>
std::shared_ptr<OpenList<Fret>> create_lifo_open_list()
{
    return std::make_shared<Wrapped<LifoOpenList, Fret>>();
}

template <bool Fret>
struct AddFifoOpenListFeatures {
    static void operator()(NamespaceLevelDeclarationList& nspace)
    {
        insert_function_definition<create_fifo_open_list<Fret>>(
            nspace,
            add_mdp_type_to_option<false, Fret>("fifo_open_list"));
    }
};

template <bool Fret>
struct AddLifoOpenListFeatures {
    static void operator()(NamespaceLevelDeclarationList& nspace)
    {
        insert_function_definition<create_lifo_open_list<Fret>>(
            nspace,
            add_mdp_type_to_option<false, Fret>("lifo_open_list"));
    }
};

} // namespace

namespace probfd::cli::open_lists {

template <bool Fret>
using OpenList = Wrapped<probfd::algorithms::OpenList, Fret>;

void add_open_list_categories(NamespaceLevelDeclarationList& nspace)
{
    insert_shared_type_declarations<open_lists::OpenList>(
        nspace,
        []<bool Fret>() {
            return add_mdp_type_to_category<false, Fret>("ProbFDOpenList");
        },
        []<bool>() { return "Open list."; });
}

void add_open_list_features(NamespaceLevelDeclarationList& nspace)
{
    insert_function_definitions<AddFifoOpenListFeatures>(nspace);
    insert_function_definitions<AddLifoOpenListFeatures>(nspace);
}

} // namespace probfd::cli::open_lists
