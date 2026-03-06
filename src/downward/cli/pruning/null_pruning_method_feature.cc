#include "downward/cli/pruning/null_pruning_method_feature.h"

#include "downward/cli/pruning/pruning_method_options.h"

#include "language/ast/internal_function_definition.h"

#include "downward/pruning/null_pruning_method.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward::null_pruning_method;

using namespace downward::cli;
using namespace language::parser;

namespace downward::cli::pruning {

InternalFunctionDefinitionBase&
add_null_pruning_method_feature(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        PruningMethod,
        NullPruningMethod,
        utils::Verbosity>>(nspace, "null_pruning");

    // document_group("");
    f.document_title("No pruning");
    f.document_synopsis(
        "This is a skeleton method that does not perform any pruning, "
        "i.e., "
        "all applicable operators are applied in all expanded states. ");

    add_pruning_options_to_feature(f, 0);

    return f;
}

} // namespace downward::cli::pruning
