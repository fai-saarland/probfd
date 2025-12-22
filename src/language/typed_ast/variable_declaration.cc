#include "language/typed_ast/variable_declaration.h"

#include "language/typed_ast/decorated_variable_node.h"

#include <cassert>
#include <utility>

using namespace std;

namespace language::parser {

VariableDeclaration::VariableDeclaration(std::string variable_name)
    : variable_name(std::move(variable_name))
{
}

} // namespace language::parser