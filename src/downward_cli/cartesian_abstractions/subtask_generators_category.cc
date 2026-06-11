#include "downward_cli/cartesian_abstractions/subtask_generators_category.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "downward/cartesian_abstractions/subtask_generators.h"

using namespace downward::cartesian_abstractions;

using namespace language::plugins;

namespace downward::cli::cartesian_abstractions {

void add_subtask_generator_category(RawRegistry& raw_registry)
{
    auto& category = raw_registry.insert_category_plugin<SubtaskGenerator>(
        "SubtaskGenerator");
    category.document_synopsis(
        "Subtask generator (used by the CEGAR heuristic).");
}

} // namespace downward::cli::cartesian_abstractions
