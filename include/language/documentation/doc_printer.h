#ifndef LANGUAGE_DOCUMENTATION_DOC_PRINTER_H
#define LANGUAGE_DOCUMENTATION_DOC_PRINTER_H

#include <ostream>
#include <vector>

namespace language::parser {
class InternalTypeDeclaration;
class InternalEnumDeclarationBase;
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclaration;

class CompilationContext;
} // namespace language::parser

namespace language::documentation {
class DocumentationTopic;
}

namespace language::documentation {

class DocPrinter {
protected:
    std::ostream& os;

public:
    virtual ~DocPrinter() = default;
    explicit DocPrinter(std::ostream& out);

    void print_documentation(
        const parser::CompilationContext& ccontext,
        std::string topic_regex,
        std::string type_regex,
        std::string enum_regex,
        std::string function_regex) const;

protected:
    virtual void print_header(
        const parser::InternalFunctionDefinitionBase& function_def) const = 0;

    virtual void print_synopsis(
        const parser::InternalFunctionDefinitionBase& function_def) const = 0;

    virtual void print_usage(
        const parser::InternalFunctionDefinitionBase& function_def) const = 0;

    virtual void print_arguments(
        const parser::InternalFunctionDefinitionBase& function_def) const = 0;

    virtual void print_notes(
        const parser::InternalFunctionDefinitionBase& function_def) const = 0;

    virtual void print_language_features(
        const parser::InternalFunctionDefinitionBase& function_def) const = 0;

    virtual void print_properties(
        const parser::InternalFunctionDefinitionBase& function_def) const = 0;

    virtual void print_header(
        const parser::InternalTypeDeclaration& type_declaration) const = 0;

    virtual void print_synopsis(
        const parser::InternalTypeDeclaration& type_declaration) const = 0;

    virtual void print_footer(
        const parser::InternalTypeDeclaration& type_declaration) const = 0;

    virtual void
    print_header(const parser::InternalEnumDeclarationBase& enum_def) const = 0;

    virtual void print_synopsis(
        const parser::InternalEnumDeclarationBase& enum_def) const = 0;

    virtual void print_enumerators(
        const parser::InternalEnumDeclarationBase& enum_def) const = 0;

    virtual void print_footer(
        const parser::InternalEnumDeclarationBase& type_declaration) const = 0;

    virtual void print_type_declaration_related_functions(
        const parser::InternalTypeDeclaration&,
        const std::vector<const parser::InternalFunctionDefinitionBase*>) const
    {
    }

    virtual void print_topic_members(
        const DocumentationTopic&,
        const std::vector<const parser::InternalFunctionDefinitionBase*>) const
    {
    }

private:
    virtual void print_topic(const DocumentationTopic& topic) const;

    virtual void print_type_declaration(
        const parser::InternalTypeDeclaration& type_declaration) const;

    virtual void print_enum_declaration(
        const parser::InternalEnumDeclarationBase& enum_declaration) const;

    virtual void print_function_declaration(
        const parser::InternalFunctionDefinitionBase& function_def) const;
};

} // namespace language::documentation

#endif
