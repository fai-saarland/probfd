#include "probfd_cli/cost_partitioning/task_pattern_collection_sorter_factory_category.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "probfd/cost_partitioning/pattern_collection_sorter_factory.h"

using namespace language::plugins;

namespace probfd::cli::cost_partitioning {

void add_task_pattern_collection_sorter_factory_category(
    RawRegistry& raw_registry)
{
    raw_registry.insert_category_plugin<
        probfd::cost_partitioning::PatternCollectionSorterFactory>(
        "TaskPatternCollectionSorterFactory");
}

} // namespace probfd::cli::heuristics