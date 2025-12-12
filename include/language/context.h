#ifndef LANGUAGE_CONTEXT_H
#define LANGUAGE_CONTEXT_H

#include "downward/utils/exceptions.h"
#include "downward/utils/timer.h"

#include <stdexcept>
#include <iostream>
#include <memory>
#include <ostream>
#include <ranges>
#include <format>
#include <string>
#include <vector>

namespace language {

struct ContextError : std::runtime_error {
    using runtime_error::runtime_error;

    template <class... Args>
    explicit ContextError(std::format_string<Args...> fmt, Args&&... args)
        : ContextError(std::format(fmt, std::forward<Args>(args)...))
    {
    }
};

class Context {
protected:
    static constexpr char INDENT[] = "  ";

    size_t initial_stack_size =
        0; // TODO: Can be removed once we got rid of LazyValues
    std::vector<std::string> block_stack;

public:
    explicit Context() = default;
    Context(const Context& context);
    virtual ~Context() noexcept(false);
    virtual std::string
    decorate_block_name(const std::string& block_name) const;
    void enter_block(const std::string& block_name);
    void leave_block(const std::string& block_name);
    std::string str() const;

    [[noreturn]]
    virtual void error(const std::string& message) const;
    virtual void warn(const std::string& message) const;

    template <typename... Args>
    [[noreturn]]
    void error(std::format_string<Args...> text, Args&&... args) const
    {
        error(std::format(std::move(text), std::forward<Args>(args)...));
    }

    template <typename... Args>
    void warn(std::format_string<Args...> text, Args&&... args) const
    {
        warn(std::format(std::move(text), std::forward<Args>(args)...));
    }
};

class TraceBlock {
    Context& context;
    std::string block_name;

public:
    explicit TraceBlock(Context& context, const std::string& block_name);

    template <typename... Args>
    TraceBlock(
        Context& context,
        std::format_string<Args...> text,
        Args&&... args)
        : TraceBlock(
              context,
              std::format(std::move(text), std::forward<Args>(args)...))
    {
    }

    ~TraceBlock();
};

} // namespace downward::utils

#endif
