#include "downward/cli/plugins/plugin.h"

#include "downward/cli/pruning_method_options.h"

#include "downward/pruning/null_pruning_method.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward::null_pruning_method;

using namespace downward::cli;
using namespace downward::cli::plugins;

namespace {

class NullPruningMethodFeature
    : public TypedFeature<downward::PruningMethod, NullPruningMethod> {
public:
    NullPruningMethodFeature()
        : TypedFeature("null")
    {
        // document_group("");
        document_title("No pruning");
        document_synopsis(
            "This is a skeleton method that does not perform any pruning, "
            "i.e., "
            "all applicable operators are applied in all expanded states. ");

        add_pruning_options_to_feature(*this);
    }

    virtual shared_ptr<NullPruningMethod>
    create_component(const Options& opts, const downward::utils::Context&)
        const override
    {
        return make_shared_from_arg_tuples<NullPruningMethod>(
            get_pruning_arguments_from_options(opts));
    }
};

FeaturePlugin<NullPruningMethodFeature> _plugin;

} // namespace
