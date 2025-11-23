#ifndef PLUGINS_DOC_PRINTER_H
#define PLUGINS_DOC_PRINTER_H

#include "plugin.h"

#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <vector>

namespace downward::cli::plugins {
class InternalTypeDeclarationBase;
}

namespace downward::cli::plugins {
class InternalFunctionDefinitionBase;
class Registry;

class DocPrinter {
    virtual void print_type_declaration(
        const InternalTypeDeclarationBase& type_declaration) const;
    virtual void print_topic(const DocumentationTopic& topic) const;
    virtual void print_function_declaration(
        const InternalFunctionDefinitionBase& function_def) const;

protected:
    std::ostream& os;
    const Registry& registry;

    virtual void
    print_header(const InternalFunctionDefinitionBase& function_def) const = 0;
    virtual void print_synopsis(
        const InternalFunctionDefinitionBase& function_def) const = 0;
    virtual void
    print_usage(const InternalFunctionDefinitionBase& function_def) const = 0;
    virtual void print_arguments(
        const InternalFunctionDefinitionBase& function_def) const = 0;
    virtual void
    print_notes(const InternalFunctionDefinitionBase& function_def) const = 0;
    virtual void print_language_features(
        const InternalFunctionDefinitionBase& function_def) const = 0;
    virtual void print_properties(
        const InternalFunctionDefinitionBase& function_def) const = 0;

    virtual void print_type_declaration_header(
        const InternalTypeDeclarationBase& type_declaration) const = 0;
    virtual void
    print_type_declaration_synopsis(const std::string& synopsis) const = 0;

    virtual void print_type_declaration_related_functions(
        const InternalTypeDeclarationBase&,
        const std::vector<const InternalFunctionDefinitionBase*>) const
    {
    }

    virtual void print_type_declaration_footer() const = 0;

    virtual void print_topic_members(
        const DocumentationTopic&,
        const std::vector<const InternalFunctionDefinitionBase*>) const
    {
    }

public:
    DocPrinter(std::ostream& out, Registry& registry);
    virtual ~DocPrinter() = default;

    void print_all() const;
    void print_topic(const std::string& name) const;
    void print_type_declaration(const std::string& name) const;
    void print_function_declaration(const std::string& name) const;
};

class Txt2TagsPrinter : public DocPrinter {
protected:
    void print_header(
        const InternalFunctionDefinitionBase& function_def) const override;
    void print_synopsis(
        const InternalFunctionDefinitionBase& function_def) const override;
    void print_usage(
        const InternalFunctionDefinitionBase& function_def) const override;
    void print_arguments(
        const InternalFunctionDefinitionBase& function_def) const override;
    void print_notes(
        const InternalFunctionDefinitionBase& function_def) const override;
    void print_language_features(
        const InternalFunctionDefinitionBase& function_def) const override;
    void print_properties(
        const InternalFunctionDefinitionBase& function_def) const override;
    void print_type_declaration_header(
        const InternalTypeDeclarationBase& type_declaration) const override;
    void
    print_type_declaration_synopsis(const std::string& synopsis) const override;
    void print_type_declaration_footer() const override;

public:
    Txt2TagsPrinter(std::ostream& out, Registry& registry);
};

class PlainPrinter : public DocPrinter {
    // If this is false, notes, properties and language_features are omitted.
    bool print_all;

protected:
    void print_header(
        const InternalFunctionDefinitionBase& function_def) const override;
    void print_synopsis(
        const InternalFunctionDefinitionBase& function_def) const override;
    void print_usage(
        const InternalFunctionDefinitionBase& function_def) const override;
    void print_arguments(
        const InternalFunctionDefinitionBase& function_def) const override;
    void print_notes(
        const InternalFunctionDefinitionBase& function_def) const override;
    void print_language_features(
        const InternalFunctionDefinitionBase& function_def) const override;
    void print_properties(
        const InternalFunctionDefinitionBase& function_def) const override;
    void print_type_declaration_header(
        const InternalTypeDeclarationBase& type_declaration) const override;
    void
    print_type_declaration_synopsis(const std::string& synopsis) const override;
    void print_type_declaration_footer() const override;

public:
    PlainPrinter(std::ostream& out, Registry& registry, bool print_all = false);
};
} // namespace downward::cli::plugins

#endif
