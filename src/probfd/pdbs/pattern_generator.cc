#include "probfd/pdbs/pattern_generator.h"

#include <utility>

namespace probfd::pdbs {

PatternGenerator::PatternGenerator(utils::Verbosity verbosity)
    : log_(utils::get_log_for_verbosity(verbosity))
{
}

PatternGenerator::PatternGenerator(utils::LogProxy log)
    : log_(std::move(log))
{
}

} // namespace probfd::pdbs
