#include "language/typed_ast/construct_context.h"

using namespace std;

namespace downward::cli::parser {

void ConstructContext::set_variable(const string& name, const std::any& value)
{
    variables[name] = value;
}

void ConstructContext::remove_variable(const string& name)
{
    variables.erase(name);
}

bool ConstructContext::has_variable(const string& name) const
{
    return variables.contains(name);
}

std::any ConstructContext::get_variable(const string& name) const
{
    std::any variable = variables.at(name);
    return variable;
}

} // namespace downward::cli::parser