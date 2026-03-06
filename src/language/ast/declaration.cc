#include "language/ast/declaration.h"

#include <utility>

using namespace std;

namespace language::parser {

Declaration::Declaration(std::string identifier)
    : identifier(std::move(identifier))
{
}

} // namespace language::parser