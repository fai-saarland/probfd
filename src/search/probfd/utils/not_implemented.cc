#include "probfd/utils/not_implemented.h"

#include "downward/utils/system.h"

namespace probfd {

void not_implemented(const std::source_location& source_location)
{
    std::cerr << "Function " << source_location.function_name()
              << " is not implemented." << std::endl;
    utils::exit_with(utils::ExitCode::SEARCH_UNIMPLEMENTED);
}

} // namespace probfd