#include "language/typed_ast/symbol_node.h"

#include "downward/utils/logging.h"

using namespace std;

namespace language::parser {

SymbolNode::SymbolNode(const string& value)
    : value(value)
{
}

std::any SymbolNode::construct(ConstructContext&) const
{
    return value;
}

void SymbolNode::print(std::ostream& out, std::size_t indent, bool) const
{
    std::print(out, "{}", std::string(indent, ' '));
    std::print(out, "{}", value);
}

} // namespace language::parser