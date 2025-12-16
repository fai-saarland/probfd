#include "downward/cli/pruning/null_pruning_method_feature.h"

#include "downward/cli/pruning/pruning_method_options.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/pruning/null_pruning_method.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward::null_pruning_method;

using namespace downward::cli;
using namespace language::plugins;

namespace {

InternalFunctionDefinitionBase& add_null_pruning_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "null_pruning",
        &language::plugins::construct_shared<
            downward::PruningMethod,
            NullPruningMethod,
            downward::utils::Verbosity>);
    // document_group("");
    f.document_title("No pruning");
    f.document_synopsis(
        "This is a skeleton method that does not perform any pruning, "
        "i.e., "
        "all applicable operators are applied in all expanded states. ");

    add_pruning_options_to_feature(f, 0);

    return f;
}

} // namespace

namespace downward::cli::pruning {

void add_null_pruning_method_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_null_pruning_to_namespace(n);
}

} // namespace downward::cli::pruning
