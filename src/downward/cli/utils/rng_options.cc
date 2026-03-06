#include "downward/cli/utils/rng_options.h"

#include "downward/utils/rng.h"

#include "language/ast/internal_function_definition.h"
#include "language/ast/internal_type_declaration.h"

using namespace std;

namespace downward::cli::utils {

void add_rng_type(language::parser::NamespaceLevelDeclarationList& nspace)
{
    insert_shared_type_declaration<downward::utils::RandomNumberGenerator>(
        nspace,
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

void add_default_rng_function(language::parser::NamespaceLevelDeclarationList& nspace)
{
    insert_function_definition< get_default_rng>(nspace,  "get_default_rng");
}

void add_seeded_rng_function(language::parser::NamespaceLevelDeclarationList& nspace)
{
    insert_function_definition< get_seeded_rng>(nspace,  "get_seeded_rng");
}

std::size_t add_rng_options_to_feature(
    language::parser::InternalFunctionDefinitionBase& feature,
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
