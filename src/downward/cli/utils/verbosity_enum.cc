#include "downward/cli/utils/verbosity_enum.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include "downward/utils/logging.h"

using namespace std;

using downward::utils::Verbosity;

namespace downward::cli::utils {

void add_verbosity_enum(plugins::Registry& registry)
{
    plugins::Namespace& n = registry.get_global_name_space();
    n.insert_enum_declaration<Verbosity>(
        {{"silent", "only the most basic output"},
         {"normal", "relevant information to monitor progress"},
         {"verbose", "full output"},
         {"debug", "like verbose with additional debug output"}});
}

} // namespace downward::cli::utils
