#ifndef UTILS_LOGGING_H
#define UTILS_LOGGING_H

#include "downward/utils/exceptions.h"
#include "downward/utils/system.h"
#include "downward/utils/timer.h"

#include <memory>
#include <ostream>
#include <ranges>
#include <string>
#include <vector>

namespace downward::utils {
// See add_log_options_to_feature for documentation.
enum class Verbosity { SILENT, NORMAL, VERBOSE, DEBUG };

/*
  Simple line-based logger that prepends time and peak memory info to each line
  of output. Lines should be eventually terminated by endl. Logs are written to
  stdout.

  Internal class encapsulated by LogProxy.
*/
class Log {
    std::ostream& stream;
    const Verbosity verbosity;

public:
    explicit Log(Verbosity verbosity)
        : stream(std::cout)
        , verbosity(verbosity)
    {
    }

    template <typename T>
    Log& operator<<(const T& elem)
    {
        stream << elem;
        return *this;
    }

    using manip_function = std::ostream& (*)(std::ostream&);

    Log& operator<<(manip_function f)
    {
        stream << f;
        return *this;
    }

    template <typename Char, typename... Args>
    void print(std::basic_format_string<Char, Args...> text, Args&&... args)
    {
        std::print(stream, text, std::forward<Args>(args)...);
    }

    template <typename Char, typename... Args>
    void println(std::basic_format_string<Char, Args...> text, Args&&... args)
    {
        std::println(stream, text, std::forward<Args>(args)...);
    }

    void println() { std::println(stream); }

    Verbosity get_verbosity() const { return verbosity; }
};

template <typename T>
concept Dumpable = requires(T t, std::ostream& out) { out << t; };

template <typename T>
struct RecursivelyDumpableHelper : std::false_type {};

template <std::ranges::input_range T>
struct RecursivelyDumpableHelper<T>
    : std::conditional_t<
          !Dumpable<T>,
          std::conditional_t<
              !Dumpable<std::ranges::range_reference_t<T>>,
              RecursivelyDumpableHelper<std::ranges::range_reference_t<T>>,
              std::true_type>,
          std::false_type> {};

template <typename T>
concept RecursivelyDumpable = RecursivelyDumpableHelper<T>::value;

/*
  This class wraps Log which holds onto the used stream (currently hard-coded
  to be cout) and any further options for modifying output (currently only
  the verbosity level).

  The wrapped class Log is a line-based logger that prepends time and peak
  memory info to each line of output. Lines should be eventually terminated by
  endl. Logs are written to stdout.

  We need to have this wrapper because we want to be able to return plain
  objects (as opposed to pointers) in function get_log_from_options below,
  which internally returns a wrapper to the default Log if default options
  are specified or a new instance if non-default options are specified.
*/
class LogProxy {
private:
    std::shared_ptr<Log> log;

public:
    explicit LogProxy(const std::shared_ptr<Log>& log)
        : log(log)
    {
    }

    LogProxy& operator<<(const Dumpable auto& elem)
    {
        (*log) << elem;
        return *this;
    }

    using manip_function = std::ostream& (*)(std::ostream&);

    LogProxy& operator<<(manip_function f)
    {
        (*log) << f;
        return *this;
    }

    template <typename... Args>
    void print(std::format_string<Args...> text, Args&&... args)
    {
        log->print(text, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void println(std::format_string<Args...> text, Args&&... args)
    {
        log->println(text, std::forward<Args>(args)...);
    }

    void println() { log->println(); }

    void print(std::string_view s) { *log << s; }

    void println(std::string_view s) { *log << s << "\n"; }

    bool is_at_least_normal() const
    {
        return log->get_verbosity() >= Verbosity::NORMAL;
    }

    bool is_at_least_verbose() const
    {
        return log->get_verbosity() >= Verbosity::VERBOSE;
    }

    bool is_at_least_debug() const
    {
        return log->get_verbosity() >= Verbosity::DEBUG;
    }

    // TODO: implement an option for logging warnings.
    bool is_warning() const
    {
        return log->get_verbosity() != Verbosity::SILENT;
    }

    template <typename T>
    friend LogProxy& operator<<(LogProxy& stream, const T& range)
        requires(RecursivelyDumpable<T>)
    {
        stream << "[";
        auto it = std::ranges::begin(range);
        auto end = std::ranges::end(range);
        if (it != end) {
            for (;;) {
                stream << *it;
                if (++it == end) break;
                stream << ", ";
            }
        }
        stream << "]";
        return stream;
    }
};

template <typename... Args>
void print(LogProxy& log, std::format_string<Args...> text, Args&&... args)
{
    log.print(text, std::forward<Args>(args)...);
}

template <typename... Args>
void println(LogProxy& log, std::format_string<Args...> text, Args&&... args)
{
    log.println(text, std::forward<Args>(args)...);
}

inline void println(LogProxy& log)
{
    log.println();
}

inline void print(LogProxy& log, std::string_view s)
{
    log << s;
}

inline void println(LogProxy& log, std::string_view s)
{
    log << s << "\n";
}

/*
  In the long term, this should not be global anymore. Instead, local LogProxy
  objects should be used everywhere. For classes constructed from the command
  line, they are parsed from Options. For other classes and functions, they
  must be passed in by the caller.
*/
extern LogProxy g_log;

extern LogProxy get_log_for_verbosity(const Verbosity& verbosity);
extern LogProxy get_silent_log();

class ContextError : public utils::Exception {
public:
    explicit ContextError(const std::string& msg);
};

class Context {
protected:
    static const std::string INDENT;
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
};

class TraceBlock {
    Context& context;
    std::string block_name;

public:
    explicit TraceBlock(Context& context, const std::string& block_name);
    ~TraceBlock();
};

} // namespace downward::utils

#endif
