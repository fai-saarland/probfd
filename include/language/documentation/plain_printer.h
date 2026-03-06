#ifndef LANGUAGE_DOCUMENTATION_PLAIN_PRINTER_H
#define LANGUAGE_DOCUMENTATION_PLAIN_PRINTER_H

#include "language/documentation/doc_printer.h"

namespace language::documentation {

class PlainPrinter : public DocPrinter {
    // If this is false, notes, properties and language_features are omitted.
    bool print_all;

public:
    explicit PlainPrinter(std::ostream& out, bool print_all = false);

protected:
    void
    print_header(const parser::InternalFunctionDefinitionBase& function_def)
        const override;

    void
    print_synopsis(const parser::InternalFunctionDefinitionBase& function_def)
        const override;

    void print_usage(const parser::InternalFunctionDefinitionBase& function_def)
        const override;

    void
    print_arguments(const parser::InternalFunctionDefinitionBase& function_def)
        const override;

    void print_notes(const parser::InternalFunctionDefinitionBase& function_def)
        const override;

    void print_language_features(
        const parser::InternalFunctionDefinitionBase& function_def)
        const override;

    void
    print_properties(const parser::InternalFunctionDefinitionBase& function_def)
        const override;

    void print_header(
        const parser::InternalTypeDeclaration& type_declaration) const override;

    void print_synopsis(
        const parser::InternalTypeDeclaration& type_declaration) const override;

    void print_footer(
        const parser::InternalTypeDeclaration& type_declaration) const override;

    void
    print_header(const parser::InternalEnumDeclarationBase& enum_declaration)
        const override;

    void
    print_synopsis(const parser::InternalEnumDeclarationBase& enum_declaration)
        const override;

    void print_enumerators(
        const parser::InternalEnumDeclarationBase& enum_def) const override;

    void
    print_footer(const parser::InternalEnumDeclarationBase& enum_declaration)
        const override;
};
} // namespace language::documentation

#endif
