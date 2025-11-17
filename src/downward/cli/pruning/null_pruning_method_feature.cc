#include "downward/cli/pruning/null_pruning_method_feature.h"

#include "downward/cli/pruning/pruning_method_options.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/pruning/null_pruning_method.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward::null_pruning_method;

using namespace downward::cli;
using namespace downward::cli::plugins;

namespace {
class NullPruningMethodFeature
    : public SharedTypedFeature<
          downward::PruningMethod,
          downward::utils::Verbosity> {
public:
    NullPruningMethodFeature()
        : TypedFeature("null", &NullPruningMethodFeature::func)
    {
        // document_group("");
        document_title("No pruning");
        document_synopsis(
            "This is a skeleton method that does not perform any pruning, "
            "i.e., "
            "all applicable operators are applied in all expanded states. ");

        add_pruning_options_to_feature(*this, 0);
    }

    static shared_ptr<downward::PruningMethod>
    func(const downward::utils::Context&, downward::utils::Verbosity verbosity)
    {
        return make_shared<NullPruningMethod>(verbosity);
    }
};
} // namespace

namespace downward::cli::pruning {

void add_null_pruning_method_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<NullPruningMethodFeature>();
}

} // namespace downward::cli::pruning
