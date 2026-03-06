#include "downward/cli/mutexes/from_file_mutex_factory_feature.h"

#include "language/ast/internal_function_definition.h"

#include "downward/mutexes/from_file_mutex_factory.h"

using namespace std;

using namespace downward;
using namespace downward::cli;
using namespace language::parser;

namespace downward::cli::mutexes {

InternalFunctionDefinitionBase&
add_from_file_mutex_factory_feature(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        TaskDependentFactory<MutexInformation>,
        FromFileMutexFactory,
        std::string>>(nspace, "mutexes_from_file");

    f.document_title("Mutexes from a mutex file");
    f.document_synopsis("Produces pre-computed mutexes as read from a file.");

    f.make_required_argument(0, "file", "The mutex file");

    return f;
}

} // namespace downward::cli::mutexes
