#include "language/typed_ast/construct_context.h"

using namespace std;

namespace language::typed_ast {

void ConstructContext::push_variable(const std::any& value)
{
    local_variables.emplace_back(value);
}

void ConstructContext::pop_variables(std::size_t num_variables)
{
    local_variables.erase(
        std::prev(local_variables.end(), num_variables),
        local_variables.end());
}

std::any ConstructContext::get_variable(std::size_t index) const
{
    return local_variables[index];
}

} // namespace language::typed_ast