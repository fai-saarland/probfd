#ifndef LANGUAGE_TYPED_AST_CONSTRUCT_CONTEXT_H
#define LANGUAGE_TYPED_AST_CONSTRUCT_CONTEXT_H

#include "language/context.h"

#include <any>
#include <string>
#include <unordered_map>

namespace language::parser {

class ConstructContext : public Context {
    std::unordered_map<std::string, std::any> variables;

public:
    void set_variable(const std::string& name, const std::any& value);
    void remove_variable(const std::string& name);
    bool has_variable(const std::string& name) const;
    std::any get_variable(const std::string& name) const;
};
} // namespace downward::cli::parser
#endif
