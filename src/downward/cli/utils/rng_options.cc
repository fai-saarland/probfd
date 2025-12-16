#include "downward/cli/utils/rng_options.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"
#include "downward/utils/rng.h"

using namespace std;

namespace downward::cli::utils {

void add_rng_type(language::plugins::Registry& registry)
{
    auto& n = registry.get_global_name_space();

    n.insert_shared_type_declaration<downward::utils::RandomNumberGenerator>(
        "RandomNumberGenerator",
        "");
}

static std::shared_ptr<downward::utils::RandomNumberGenerator> get_default_rng()
{
    return downward::utils::get_default_rng();
}

static std::shared_ptr<downward::utils::RandomNumberGenerator>
get_seeded_rng(int random_seed)
{
    return std::make_shared<downward::utils::RandomNumberGenerator>(
        random_seed);
}

void add_default_rng_function(language::plugins::Registry& registry)
{
    auto& n = registry.get_global_name_space();

    n.insert_function_definition("get_default_rng", get_default_rng);
}

void add_seeded_rng_function(language::plugins::Registry& registry)
{
    auto& n = registry.get_global_name_space();

    n.insert_function_definition("get_seeded_rng", get_seeded_rng);
}

std::size_t add_rng_options_to_feature(
    language::plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index)
{
    feature.make_optional_argument_with_default(
        start_index,
        "rng",
        "get_default_rng()",
        "A random number generator.");
    return 1;
}

} // namespace downward::cli::utils
