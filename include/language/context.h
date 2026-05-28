#ifndef LANGUAGE_CONTEXT_H
#define LANGUAGE_CONTEXT_H

#include <format>
#include <stdexcept>
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

    std::vector<std::string> block_stack;

public:
    explicit Context() = default;

    Context(const Context& context) = delete;

    virtual ~Context() = default;

    virtual std::string
    decorate_block_name(const std::string& block_name) const;
    void enter_block(const std::string& block_name);
    void leave_block(const std::string& block_name);
    std::string str() const;

    [[noreturn]]
    virtual void error(const std::string& message) const;

    [[noreturn]]
    virtual void warn(const std::string& message) const;

    template <typename... Args>
    [[noreturn]]
    void error(std::format_string<Args...> text, Args&&... args) const
    {
        error(std::format(std::move(text), std::forward<Args>(args)...));
	abort();
    }

    template <typename... Args>
    void warn(std::format_string<Args...> text, Args&&... args) const
    {
        warn(std::format(std::move(text), std::forward<Args>(args)...));
	abort();
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

} // namespace language

#endif
