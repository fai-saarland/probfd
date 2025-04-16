#include "downward/cli/plugins/plugin.h"

#include "downward/mutexes/from_file_mutex_factory.h"

#include <filesystem>

using namespace std;

using namespace downward;
using namespace downward::cli;
using namespace downward::cli::plugins;

namespace {

class FromFileMutexFactoryFeature
    : public TypedFeature<TaskDependentFactory<MutexInformation>, FromFileMutexFactory> {
public:
    FromFileMutexFactoryFeature()
        : TypedFeature("mutexes_from_file")
    {
        document_title("Mutexes from a mutex file");
        document_synopsis("Produces pre-computed mutexes as read from a file.");

        add_option<std::string>("file", "The mutex file");
    }

    virtual shared_ptr<FromFileMutexFactory>
    create_component(const Options& opts, const utils::Context& context)
        const override
    {
        auto filename = opts.get<std::string>("file");

        if (!std::filesystem::exists(filename)) {
            context.error("The mutex file " + filename + " does not exist!");
        }

        return make_shared_from_arg_tuples<FromFileMutexFactory>(filename);
    }
};

FeaturePlugin<FromFileMutexFactoryFeature> _plugin;

} // namespace
