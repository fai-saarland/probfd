#ifndef LANGUAGE_TYPED_AST_NAMESPACE_LEVEL_DECLARATION_VISITOR_H
#define LANGUAGE_TYPED_AST_NAMESPACE_LEVEL_DECLARATION_VISITOR_H

namespace language::parser {

class InternalEnumDeclarationBase;
class InternalFunctionDefinitionBase;
class InternalTypeDeclaration;
class NamespaceDeclaration;

class NamespaceLevelDeclarationVisitor {
public:
    virtual ~NamespaceLevelDeclarationVisitor() = default;

    virtual void accept(const InternalEnumDeclarationBase& declaration) = 0;
    virtual void accept(const InternalFunctionDefinitionBase& declaration) = 0;
    virtual void accept(const InternalTypeDeclaration& declaration) = 0;
    virtual void accept(const NamespaceDeclaration& declaration) = 0;
};

} // namespace language::parser

#endif
