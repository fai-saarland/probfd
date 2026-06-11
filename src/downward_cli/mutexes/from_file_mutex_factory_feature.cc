#include "downward_cli/mutexes/from_file_mutex_factory_feature.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "downward/mutexes/from_file_mutex_factory.h"

#include <filesystem>

using namespace std;

using namespace downward;
using namespace downward::cli;

using namespace language;
using namespace language::plugins;

namespace {
class FromFileMutexFactoryFeature
    : public TypedFeature<TaskDependentFactory<MutexInformation>> {
public:
    FromFileMutexFactoryFeature()
        : TypedFeature("mutexes_from_file")
    {
        document_title("Mutexes from a mutex file");
        document_synopsis("Produces pre-computed mutexes as read from a file.");

        add_option<std::string>("file", "The mutex file");
    }

    shared_ptr<TaskDependentFactory<MutexInformation>>
    create_component(const Options& opts, const Context& context) const override
    {
        const auto filename = opts.get<std::string>(context, "file");

        if (!std::filesystem::exists(filename)) {
            context.error("The mutex file {} does not exist!", filename);
        }

        return make_shared_from_arg_tuples<FromFileMutexFactory>(filename);
    }
};
} // namespace

namespace downward::cli::mutexes {

void add_from_file_mutex_factory_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<FromFileMutexFactoryFeature>();
}

} // namespace downward::cli::mutexes
