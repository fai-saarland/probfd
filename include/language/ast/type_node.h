#ifndef LANGUAGE_AST_TYPE_NODE_H
#define LANGUAGE_AST_TYPE_NODE_H

namespace language {
class Context;
}

namespace language::typed_ast {
class GlobalEnvironment;
class Type;
class TypeRegistry;
} // namespace language::typed_ast

namespace language::parser {

class TypeNode {
public:
    virtual ~TypeNode() = default;

    virtual const typed_ast::Type& get_type(
        Context& context,
        const typed_ast::GlobalEnvironment& environment,
        typed_ast::TypeRegistry& type_registry) const = 0;
};

} // namespace language::parser

#endif
