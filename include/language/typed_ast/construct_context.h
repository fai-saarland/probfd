#ifndef LANGUAGE_TYPED_AST_CONSTRUCT_CONTEXT_H
#define LANGUAGE_TYPED_AST_CONSTRUCT_CONTEXT_H

#include "language/context.h"

#include <any>
#include <deque>

namespace language::typed_ast {

class ConstructContext : public Context {
    std::deque<std::any> global_variables;
    std::deque<std::any> local_variables;

public:
    void push_variable(const std::any& value);

    void pop_variables(std::size_t num_variables);

    std::any get_variable(std::size_t index) const;

    template <typename T>
    T get_variable(std::size_t index) const;
};

} // namespace language::typed_ast

namespace language::typed_ast {

template <typename ValueType>
struct AnyCaster {
    static ValueType cast(const std::any& operand)
    {
        return std::any_cast<ValueType>(operand);
    }
};

template <typename T>
struct AnyCaster<std::vector<T>> {
    static std::vector<T> cast(const std::any& operand)
    {
        if (operand.type() == typeid(std::vector<T>)) {
            return std::any_cast<std::vector<T>>(operand);
        }

        const auto* any_elements =
            std::any_cast<std::vector<std::any>>(&operand);

        std::vector<T> result;
        result.reserve(any_elements->size());
        for (const std::any& element : *any_elements) {
            result.push_back(AnyCaster<T>::cast(element));
        }
        return result;
    }
};

template <typename T>
T ConstructContext::get_variable(std::size_t index) const
{
    if (index >= local_variables.size()) {
        this->error(
            "Attempt to retrieve non-existing parameter (index {})",
            index);
    }

    const auto& obj = local_variables[index];

    if (!obj.has_value()) {
        this->error(
            "Attempt to retrieve unset parameter at index {} "
            "(type: {})",
            index,
            typeid(T).name());
    }

    try {
        return AnyCaster<T>::cast(obj);
    } catch (const std::bad_any_cast&) {
        this->error(
            "Invalid conversion while retrieving config options!\n"
            "Parameter {} is not of type {} but of type {}",
            index,
            typeid(T).name(),
            obj.type().name());
    }
}

} // namespace language::typed_ast

#endif
