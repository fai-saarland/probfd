#include "probfd/utils/not_implemented.h"

#include "downward/utils/system.h"

using namespace downward;

namespace probfd {

void not_implemented(const std::source_location& source_location)
{
    println(
        std::cerr,
        "Function {} is not implemented.",
        source_location.function_name());
    utils::exit_with(utils::ExitCode::SEARCH_UNIMPLEMENTED);
}

} // namespace probfd