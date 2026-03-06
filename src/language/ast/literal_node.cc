#include "language/ast/literal_node.h"

#include "language/typed_ast/variable_environment.h"

#include "language/typed_ast/decorated_bool_literal_node.h"
#include "language/typed_ast/decorated_feature_literal_node.h"
#include "language/typed_ast/decorated_float_literal_node.h"
#include "language/typed_ast/decorated_function_call_node.h"
#include "language/typed_ast/decorated_int_literal_node.h"
#include "language/typed_ast/decorated_string_literal_node.h"

#include "language/ast/compilation_context.h"
#include "language/ast/internal_function_definition.h"
#include "language/typed_ast/types.h"

#include "language/critical_error.h"

#include <cassert>
#include <charconv>
#include <vector>

using namespace std;

namespace language::parser {

LiteralNode::LiteralNode(const Token& value)
    : value(value)
{
}

TypedDecoratedAstNodePtr LiteralNode::static_analysis(
    Context& context,
    typed_ast::GlobalEnvironment& env,
    typed_ast::LocalEnvironment&,
    typed_ast::TypeRegistry& type_registry) const
{
    switch (value.type) {
    case TokenType::TRUE: {
        return {
            std::make_unique<typed_ast::BoolLiteralNode>(true),
            &type_registry.get_type<bool>()};
    }
    case TokenType::FALSE: {
        return {
            std::make_unique<typed_ast::BoolLiteralNode>(false),
            &type_registry.get_type<bool>()};
    }
    case TokenType::STRING: {
        if (value.content.front() != '"' || value.content.back() != '"') {
            throw CriticalError(
                "String literal value is not enclosed in quotation marks.");
        }

        /*
          We are not doing any further syntax checking. Escaped symbols other
          than
          \n will just ignore the escaping \ (e.g., \t is treated as t, not as a
          tab). Strings ending in \ will not produce an error but should be
          excluded by the previous steps.
        */
        string result;
        result.reserve(value.content.length() - 2);
        bool escaped = false;
        for (const char c : value.content.substr(1, value.content.size() - 2)) {
            if (escaped) {
                escaped = false;
                if (c == 'n') {
                    result += '\n';
                } else {
                    result += c;
                }
            } else if (c == '\\') {
                escaped = true;
            } else {
                result += c;
            }
        }

        return {
            std::make_unique<typed_ast::StringLiteralNode>(std::move(result)),
            &type_registry.get_type<string>()};
    }
    case TokenType::INTEGER: {
        const char* first = value.content.data();
        const char* last = value.content.data() + value.content.size();

        int x;
        const auto [data, ec] = std::from_chars(first, last, x);

        switch (ec) {
        case std::errc::invalid_argument:
            throw CriticalError(
                "Could not parse integer literal '{}'.",
                value.content);
        case std::errc::result_out_of_range:
            context.error(
                "Integer value is out of range: '{}'.",
                value.content);
        default:
            if (data == last) {
                return {
                    std::make_unique<typed_ast::IntLiteralNode>(x),
                    &type_registry.get_type<int>()};
            }

            const auto operator_fname = std::format(
                "__operator_int_{}__",
                std::string_view{data, last});

            const auto result =
                env.get_global_namespace().get_variable_declaration(
                    operator_fname);

            if (std::holds_alternative<std::monostate>(result)) {
                context.error(
                    "User-defined int literal function '{}' not found.",
                    operator_fname);
            }

            const auto& function_decls = std::get<2>(result);

            if (function_decls->size() > 1) {
                throw CriticalError(
                    "Overload resolution currently not supported.");
            }

            const auto& [type, function_decl] = (*function_decls)[0];

            std::vector<typed_ast::FunctionArgument> arguments;
            arguments.emplace_back(
                std::make_unique<typed_ast::IntLiteralNode>(x),
                false);

            return {
                std::make_unique<typed_ast::DecoratedFunctionCallNode>(
                    function_decl->create_load_node(),
                    std::move(arguments)),
                &type->get_return_type()};
        }
    }
    case TokenType::FLOAT: {
        const char* first = value.content.data();
        const char* last = value.content.data() + value.content.size();

        double x;
        const auto [data, ec] = std::from_chars(first, last, x);

        switch (ec) {
        case std::errc::invalid_argument:
            throw CriticalError(
                "Could not parse float literal '{}'.",
                value.content);
        case std::errc::result_out_of_range:
            context.error("Float value is out of range: '{}'.", value.content);
        default:
            if (data == last) {
                return {
                    std::make_unique<typed_ast::FloatLiteralNode>(x),
                    &type_registry.get_type<double>()};
            }

            const auto operator_fname =
                std::format("__operator_float_{}__", string_view{data, last});

            const auto result =
                env.get_global_namespace().get_variable_declaration(
                    operator_fname);

            if (std::holds_alternative<std::monostate>(result)) {
                context.error(
                    "User-defined int literal function '{}' not found.",
                    operator_fname);
            }

            const auto& function_decls = std::get<2>(result);

            if (function_decls->size() > 1) {
                throw CriticalError(
                    "Overload resolution currently not supported.");
            }

            const auto& [type, function_decl] = (*function_decls)[0];

            std::vector<typed_ast::FunctionArgument> arguments;
            arguments.emplace_back(
                std::make_unique<typed_ast::FloatLiteralNode>(x),
                false);

            return {
                std::make_unique<typed_ast::DecoratedFunctionCallNode>(
                    function_decl->create_load_node(),
                    std::move(arguments)),
                &type->get_return_type()};
        }
    }
    default:
        throw CriticalError(
            "LiteralNode has unexpected token type '{}'.",
            token_type_name(value.type));
    }
}

} // namespace language::parser
