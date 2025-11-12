#include "downward/cli/utils/verbosity_enum.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/utils/logging.h"

using namespace std;

using downward::utils::Verbosity;

namespace downward::cli::utils {

void add_verbosity_enum(plugins::Registry& raw_registry)
{
    raw_registry.insert_enum_plugin<Verbosity>(
        {{"silent", "only the most basic output"},
         {"normal", "relevant information to monitor progress"},
         {"verbose", "full output"},
         {"debug", "like verbose with additional debug output"}});
}

} // namespace downward::cli::utils
