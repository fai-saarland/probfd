#ifndef LANGUAGE_AST_COMPILATION_CONTEXT_H
#define LANGUAGE_AST_COMPILATION_CONTEXT_H

#include "language/ast/namespace_level_declaration.h"

#include "language/documentation/topic.h"

#include <deque>
#include <format>
#include <map>
#include <set>
#include <stdexcept>
#include <vector>

namespace language::typed_ast {
class GlobalEnvironment;
}

namespace language::parser {

class DuplicateTopicError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;

    template <class... Args>
    explicit DuplicateTopicError(
        std::format_string<Args...> fmt,
        Args&&... args)
        : std::runtime_error(std::format(fmt, std::forward<Args>(args)...))
    {
    }
};

class CompilationContext : public NamespaceLevelDeclarationList {
    std::deque<documentation::DocumentationTopic> topics_;
    std::map<std::string, documentation::DocumentationTopic*> topics_by_name_;

public:
    template <std::derived_from<documentation::DocumentationTopic> T>
    documentation::DocumentationTopic& insert_documentation_topic()
    {
        auto& s = topics_.emplace_back(T());
        auto [it, inserted] = topics_by_name_.emplace(s.get_topic_name(), &s);

        if (!inserted) {
            throw DuplicateTopicError(
                "Documentation topic with name {} already exists.",
                s.get_topic_name());
        }

        return s;
    }

    auto get_topics() const { return std::views::all(topics_); }

    void register_declarations(
        Context& context,
        typed_ast::GlobalEnvironment& env,
        typed_ast::TypeRegistry& type_registry) const;
};

} // namespace language::parser

#endif
