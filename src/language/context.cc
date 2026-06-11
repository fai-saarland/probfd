#include "language/context.h"

#include <iomanip>
#include <iostream>
#include <ranges>
#include <vector>

using namespace std;

namespace language {

Context::Context(const Context& context)
    : initial_stack_size(context.block_stack.size())
    , block_stack(context.block_stack)
{
}

Context::~Context() noexcept(false)
{
    if (block_stack.size() > initial_stack_size) {
        cerr << str() << endl;
        throw ContextError("A context was destructed with an non-empty stack.");
    }
}

string Context::decorate_block_name(const string& block_name) const
{
    return block_name;
}

void Context::enter_block(const string& block_name)
{
    block_stack.push_back(block_name);
}

void Context::leave_block(const string& block_name)
{
    if (block_stack.empty() || block_stack.back() != block_name) {
        cerr << str() << endl;
        throw ContextError(
            "Tried to pop a block '{}' from an empty stack or the block to "
            "remove is not on the top of the stack.",
            block_name);
    }
    block_stack.pop_back();
}

string Context::str() const
{
    if (block_stack.empty()) { return "Traceback: Empty\n"; }

    return std::format(
        "{}{:s}",
        INDENT,
        std::views::join_with(block_stack, "\n" + std::string(INDENT) + "-> "));
}

void Context::error(const string& message) const
{
    throw ContextError(str() + "\n\n" + message);
}

void Context::warn(const string& message) const
{
    std::cerr << str() << endl << endl << message;
}

TraceBlock::TraceBlock(Context& context, const string& block_name)
    : context(context)
    , block_name(context.decorate_block_name(block_name))
{
    context.enter_block(this->block_name);
}

TraceBlock::~TraceBlock()
{
    context.leave_block(block_name);
}

} // namespace language
