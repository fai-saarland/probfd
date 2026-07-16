#ifndef LANGUAGE_PARSER_EXPRESSION_H
#define LANGUAGE_PARSER_EXPRESSION_H

#include "language/plugins/raw_registry.h"

#include <memory>
#include <string>

namespace language::plugins {
class Type;
} // namespace language::plugins

namespace language::parser {

class DecoratedExpression;
class DecorateContext;

struct TypedDecoratedAstNodePtr {
    std::unique_ptr<DecoratedExpression> ast_node;
    const plugins::Type* type;
};

class Expression {
public:
    virtual ~Expression() = default;

    std::unique_ptr<DecoratedExpression>
    decorate(const plugins::RawRegistry& raw_registry) const;

    virtual TypedDecoratedAstNodePtr
    decorate(DecorateContext& context) const = 0;

    virtual void dump(std::string indent = "+") const = 0;
};

} // namespace language::parser
#endif
