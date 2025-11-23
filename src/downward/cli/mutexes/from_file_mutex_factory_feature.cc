#include "downward/cli/mutexes/from_file_mutex_factory_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/mutexes/from_file_mutex_factory.h"

#include <filesystem>

using namespace std;

using namespace downward;
using namespace downward::cli;
using namespace downward::cli::plugins;

namespace {

InternalFunctionDefinitionBase& add_from_file_mutex_factory_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "mutexes_from_file",
        &cli::plugins::construct_shared<
            TaskDependentFactory<MutexInformation>,
            FromFileMutexFactory,
            std::string>);
    f.document_title("Mutexes from a mutex file");
    f.document_synopsis("Produces pre-computed mutexes as read from a file.");

    f.make_required_argument(0, "file", "The mutex file");

    return f;
}

} // namespace

namespace downward::cli::mutexes {

void add_from_file_mutex_factory_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_from_file_mutex_factory_to_namespace(n);
}

} // namespace downward::cli::mutexes
