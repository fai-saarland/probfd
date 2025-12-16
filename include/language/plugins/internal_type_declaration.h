#ifndef LANGUAGE_PLUGINS_INTERNAL_TYPE_DECLARATION_H
#define LANGUAGE_PLUGINS_INTERNAL_TYPE_DECLARATION_H

#include <memory>
#include <string>
#include <type_traits>
#include <typeindex>

namespace language::plugins {

/*
  The InternalTypeDeclarationBase class contains meta-information for a C++
  type.
*/
class InternalTypeDeclarationBase {
    std::type_index pointer_type;

    std::string identifier;

    /*
      General documentation for the type.
      This is included at the top of the wiki page for this feature type.
    */
    std::string synopsis;

protected:
    InternalTypeDeclarationBase(
        std::type_index pointer_type,
        std::string type_identifier,
        std::string synopsis);

public:
    virtual ~InternalTypeDeclarationBase() = default;

    std::type_index get_pointer_type() const;
    std::string get_identifier() const;
    std::string get_class_name() const;
    std::string get_synopsis() const;
};

template <typename T>
    requires std::is_class_v<T>
class InternalTypeDeclaration : public InternalTypeDeclarationBase {
public:
    explicit InternalTypeDeclaration(
        std::string type_identifier,
        std::string synopsis)
        : InternalTypeDeclarationBase(
              typeid(T),
              std::move(type_identifier),
              std::move(synopsis))
    {
    }
};

template <typename T>
using InternalSharedTypeDeclaration =
    InternalTypeDeclaration<std::shared_ptr<T>>;

} // namespace language::plugins

#endif
