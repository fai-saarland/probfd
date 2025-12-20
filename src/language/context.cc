#include "language/context.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

namespace language {

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
    ostringstream message;
    message << "Traceback:" << endl;
    if (block_stack.empty()) {
        message << INDENT << "Empty";
    } else {
        message << INDENT;
        const auto sep = "\n" + std::string(INDENT) + "-> ";
        auto it = std::ranges::begin(block_stack);
        const auto end = std::ranges::end(block_stack);

        if (it != end) {
            std::print(message, "{}", *it);
            for (++it; it != end; ++it) {
                std::print(message, "{}{}", sep, *it);
            }
        }
    }
    return message.str();
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
