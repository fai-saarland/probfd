#ifndef PARSER_TYPE_NODE_H
#define PARSER_TYPE_NODE_H

#include <string>

namespace downward::utils {
class Context;
}

namespace downward::cli::plugins {
class Type;
class TypeRegistry;
} // namespace downward::cli::plugins

namespace downward::cli::parser {

class TypeNode {
public:
    virtual ~TypeNode() = default;

    virtual const plugins::Type&
    get_type(utils::Context& context, plugins::TypeRegistry& type_registry)
        const = 0;
};

} // namespace downward::cli::parser

#endif
