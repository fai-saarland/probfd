#include "language/ast/internal_function_definition.h"

#include "language/ast/namespace_level_declaration_visitor.h"

using namespace std;

namespace language::parser {

InternalFunctionDefinitionBase::InternalFunctionDefinitionBase(
    string identifier,
    typed_ast::AnyFunctionType func,
    std::type_index return_type,
    std::vector<std::type_index> arg_types)
    : NamespaceLevelDeclaration(std::move(identifier))
    , func(func)
    , return_type(std::move(return_type))
    , arg_types(std::move(arg_types))
    , arguments(this->arg_types.size(), ArgumentInfo::make_required())
    , argument_docs(this->arg_types.size())
{
}

std::type_index InternalFunctionDefinitionBase::get_return_type() const
{
    return return_type;
}

const std::vector<std::type_index>&
InternalFunctionDefinitionBase::get_arg_types() const
{
    return arg_types;
}

void InternalFunctionDefinitionBase::document_title(const string& title)
{
    this->title = title;
}

void InternalFunctionDefinitionBase::document_synopsis(const string& note)
{
    synopsis = note;
}

void InternalFunctionDefinitionBase::document_property(
    const string& property,
    const string& note)
{
    properties.emplace_back(property, note);
}

void InternalFunctionDefinitionBase::document_language_support(
    const string& feature,
    const string& note)
{
    language_support.emplace_back(feature, note);
}

void InternalFunctionDefinitionBase::document_note(
    const string& name,
    const string& note,
    bool long_text)
{
    notes.emplace_back(name, note, long_text);
}

string InternalFunctionDefinitionBase::get_title() const
{
    return title;
}

string InternalFunctionDefinitionBase::get_synopsis() const
{
    return synopsis;
}

const vector<std::string>&
InternalFunctionDefinitionBase::get_argument_docs() const
{
    return argument_docs;
}

const vector<documentation::PropertyInfo>&
InternalFunctionDefinitionBase::get_properties() const
{
    return properties;
}

const vector<documentation::LanguageSupportInfo>&
InternalFunctionDefinitionBase::get_language_support() const
{
    return language_support;
}

const vector<documentation::NoteInfo>&
InternalFunctionDefinitionBase::get_notes() const
{
    return notes;
}

void InternalFunctionDefinitionBase::register_declarations(
    Context& context,
    typed_ast::GlobalEnvironment& env,
    typed_ast::NamespaceScope& scope,
    typed_ast::TypeRegistry& type_registry) const
{
    const auto& type = type_registry.get_function_type(return_type, arg_types);

    auto& value = env.emplace_global_value<InternalFunctionValue>(func);

    scope.insert_declaration(identifier, type, value, context);
}

void InternalFunctionDefinitionBase::visit(
    NamespaceLevelDeclarationVisitor& visitor)
{
    visitor.accept(*this);
}

} // namespace language::parser
