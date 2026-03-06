//
// Created by Thorsten Klößner on 31.12.2025.
// Copyright (c) 2025 ProbFD contributors.
//

#include "language/typed_ast/value.h"

namespace language::typed_ast {

void Value::add_user(DecoratedExpressionNode* user) const
{
    users.push_back(user);
}

void Value::remove_user(DecoratedExpressionNode* user) const
{
    users.erase(std::ranges::find(users, user));
}

StackValue::StackValue(std::size_t index)
    : index(index)
{
}

std::size_t StackValue::get_index() const
{
    return index;
}

} // namespace language::parser
