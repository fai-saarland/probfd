#ifndef LANGUAGE_AST_TYPE_NODE_H
#define LANGUAGE_AST_TYPE_NODE_H

namespace language {
class Context;
}

namespace language::plugins {
class Type;
class TypeRegistry;
} // namespace language::plugins

namespace language::parser {
class VariableEnvironment;
}

namespace language::parser {

class TypeNode {
public:
    virtual ~TypeNode() = default;

    virtual const plugins::Type& get_type(
        Context& context,
        const VariableEnvironment& environment,
        plugins::TypeRegistry& type_registry) const = 0;
};

} // namespace language::parser

#endif
