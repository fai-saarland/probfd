#ifndef LANGUAGE_CONTEXT_H
#define LANGUAGE_CONTEXT_H

#include <format>
#include <stdexcept>
#include <string>
#include <vector>

namespace language {

struct ContextError : std::runtime_error {
    using std::runtime_error::runtime_error;

    template <class... Args>
    explicit ContextError(std::format_string<Args...> fmt, Args&&... args)
        : std::runtime_error(std::format(fmt, std::forward<Args>(args)...))
    {
    }
};

class Context {
protected:
    static constexpr char INDENT[] = "  ";

    // TODO: Can be removed once we got rid of LazyValues
    size_t initial_stack_size = 0;
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
    explicit TraceBlock(
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
