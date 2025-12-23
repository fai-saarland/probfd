#include "downward/cli/mutexes/from_file_mutex_factory_feature.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/mutexes/from_file_mutex_factory.h"

#include <filesystem>

using namespace std;

using namespace downward;
using namespace downward::cli;
using namespace language::plugins;

namespace {

} // namespace

namespace downward::cli::mutexes {

InternalFunctionDefinitionBase&
add_from_file_mutex_factory_feature(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "mutexes_from_file",
        &language::plugins::construct_shared<
            TaskDependentFactory<MutexInformation>,
            FromFileMutexFactory,
            std::string>);
    f.document_title("Mutexes from a mutex file");
    f.document_synopsis("Produces pre-computed mutexes as read from a file.");

    f.make_required_argument(0, "file", "The mutex file");

    return f;
}

} // namespace downward::cli::mutexes
