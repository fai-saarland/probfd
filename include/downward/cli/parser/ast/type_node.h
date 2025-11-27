#ifndef PARSER_TYPE_NODE_H
#define PARSER_TYPE_NODE_H

#include <string>

namespace downward::cli::plugins {
class Type;
class TypeRegistry;
} // namespace downward::cli::plugins

namespace downward::cli::parser {

class TypeNode {
public:
    virtual ~TypeNode() = default;

    virtual const plugins::Type&
    get_type(plugins::TypeRegistry& type_registry) const = 0;

    virtual void dump(std::string indent) const = 0;
};

} // namespace downward::cli::parser

#endif
