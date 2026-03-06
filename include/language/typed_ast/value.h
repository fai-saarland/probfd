//
// Created by Thorsten Klößner on 31.12.2025.
// Copyright (c) 2025 ProbFD contributors.
//

#ifndef LANGUAGE_TYPED_AST_VALUE_H
#define LANGUAGE_TYPED_AST_VALUE_H

#include <deque>
#include <memory>
#include <ranges>
#include <string>
#include <vector>

namespace language::parser {
struct ArgumentInfo;
}

namespace language::typed_ast {
class FunctionType;
class Type;
class DecoratedExpressionNode;
} // namespace language::typed_ast

namespace language::typed_ast {

class Value {
protected:
    mutable std::deque<DecoratedExpressionNode*> users;

public:
    virtual ~Value() = default;

    virtual std::unique_ptr<DecoratedExpressionNode> create_load_node() = 0;

    void add_user(DecoratedExpressionNode* user) const;

    void remove_user(DecoratedExpressionNode* user) const;

    auto get_users() const { return users | std::views::all; }
};

class StackValue : public Value {
protected:
    std::size_t index;

public:
    explicit StackValue(std::size_t index);

    std::size_t get_index() const;
};

class GlobalValue : public Value {};

class GlobalObject : public GlobalValue {};

class FunctionValue : public GlobalValue {
public:
    virtual const std::vector<parser::ArgumentInfo>& get_arguments() = 0;
};

struct TypedValue {
    const Type* type;
    Value* declaration;
};

struct TypedFunctionValue {
    const FunctionType* type;
    FunctionValue* declaration;
};

} // namespace language::typed_ast

#endif // LANGUAGE_TYPED_AST_VALUE_H
