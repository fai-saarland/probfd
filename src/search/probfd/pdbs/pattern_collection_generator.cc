#include <utility>

#include "probfd/pdbs/pattern_collection_generator.h"

namespace probfd::pdbs {

PatternCollectionGenerator::PatternCollectionGenerator(
    utils::Verbosity verbosity)
    : log_(utils::get_log_for_verbosity(verbosity))
{
}

PatternCollectionGenerator::PatternCollectionGenerator(utils::LogProxy log)
    : log_(std::move(log))
{
}

} // namespace probfd::pdbs
