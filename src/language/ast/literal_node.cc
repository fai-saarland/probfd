#include "language/ast/literal_node.h"

#include "language/ast/variable_environment.h"

#include "language/typed_ast/decorated_bool_literal_node.h"
#include "language/typed_ast/decorated_feature_literal_node.h"
#include "language/typed_ast/decorated_float_literal_node.h"
#include "language/typed_ast/decorated_function_call_node.h"
#include "language/typed_ast/decorated_int_literal_node.h"
#include "language/typed_ast/decorated_string_literal_node.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"
#include "language/plugins/types.h"

#include "downward/utils/system.h"

#include <charconv>
#include <vector>

using namespace std;

namespace language::parser {

LiteralNode::LiteralNode(const Token& value)
    : value(value)
{
}

TypedDecoratedAstNodePtr
LiteralNode::static_analysis(Context& context, VariableEnvironment& env)
    const
{
    switch (value.type) {
    case TokenType::TRUE: {
        return {
            std::make_unique<BoolLiteralNode>(true),
            &plugins::TypeRegistry::instance()->get_type<bool>()};
    }
    case TokenType::FALSE: {
        return {
            std::make_unique<BoolLiteralNode>(false),
            &plugins::TypeRegistry::instance()->get_type<bool>()};
    }
    case TokenType::STRING: {
        if (value.content.front() != '"' || value.content.back() != '"') {
            throw downward::utils::CriticalError(
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
            std::make_unique<StringLiteralNode>(std::move(result)),
            &plugins::TypeRegistry::instance()->get_type<string>()};
    }
    case TokenType::INTEGER: {
        const char* first = value.content.data();
        const char* last = value.content.data() + value.content.size();

        int x;
        const auto [data, ec] = std::from_chars(first, last, x);

        switch (ec) {
        case std::errc::invalid_argument:
            throw downward::utils::CriticalError(
                "Could not parse integer literal '{}'.",
                value.content);
        case std::errc::result_out_of_range:
            context.error(
                "Integer value is out of range: '{}'.",
                value.content);
        default:
            if (data == last) {
                return {
                    std::make_unique<IntLiteralNode>(x),
                    &plugins::TypeRegistry::instance()->get_type<int>()};
            }

            const auto& n = env.get_registry().get_global_name_space();
            const auto operator_fname = std::format(
                "__operator_int_{}__",
                std::string_view{data, last});

            if (!n.has_function(operator_fname)) {
                context.error(
                    "User-defined int literal function '{}' not found.",
                    operator_fname);
            }

            const auto& feature = n.get_function_definition(operator_fname);

            std::vector<FunctionArgument> arguments;
            arguments.emplace_back(std::make_unique<IntLiteralNode>(x), false);

            return {
                std::make_unique<DecoratedFunctionCallNode>(
                    std::make_unique<FeatureLiteralNode>(feature),
                    std::move(arguments),
                    ""),
                &feature.get_type().get_return_type()};
        }
    }
    case TokenType::FLOAT: {
        const char* first = value.content.data();
        const char* last = value.content.data() + value.content.size();

        double x;
        const auto [data, ec] = std::from_chars(first, last, x);

        switch (ec) {
        case std::errc::invalid_argument:
            throw downward::utils::CriticalError(
                "Could not parse float literal '{}'.",
                value.content);
        case std::errc::result_out_of_range:
            context.error("Float value is out of range: '{}'.", value.content);
        default:
            if (data == last) {
                return {
                    std::make_unique<FloatLiteralNode>(x),
                    &plugins::TypeRegistry::instance()->get_type<double>()};
            }

            const auto& n = env.get_registry().get_global_name_space();
            const auto operator_fname =
                std::format("__operator_float_{}__", string_view{data, last});

            if (!n.has_function(operator_fname)) {
                context.error(
                    "User-defined float literal function '{}' not found.",
                    operator_fname);
            }

            const auto& feature = n.get_function_definition(operator_fname);

            std::vector<FunctionArgument> arguments;
            arguments.emplace_back(
                std::make_unique<FloatLiteralNode>(x),
                false);

            return {
                std::make_unique<DecoratedFunctionCallNode>(
                    std::make_unique<FeatureLiteralNode>(feature),
                    std::move(arguments),
                    ""),
                &feature.get_type().get_return_type()};
        }
    }
    default:
        throw downward::utils::CriticalError(
            "LiteralNode has unexpected token type '{}'.",
            token_type_name(value.type));
    }
}

} // namespace language::parser
