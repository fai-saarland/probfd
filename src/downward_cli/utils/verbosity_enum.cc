#include "downward_cli/utils/verbosity_enum.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "downward/utils/logging.h"

using namespace std;

using namespace language;

using downward::utils::Verbosity;

namespace downward::cli::utils {

void add_verbosity_enum(plugins::RawRegistry& raw_registry)
{
    raw_registry.insert_enum_plugin<Verbosity>(
        {{"silent", "only the most basic output"},
         {"normal", "relevant information to monitor progress"},
         {"verbose", "full output"},
         {"debug", "like verbose with additional debug output"}});
}

} // namespace downward::cli::utils
