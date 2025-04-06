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
    bool line_has_started;

public:
    explicit Log(Verbosity verbosity)
        : stream(std::cout)
        , verbosity(verbosity)
        , line_has_started(false)
    {
    }

    template <typename T>
    Log& operator<<(const T& elem)
    {
        if (!line_has_started) {
            line_has_started = true;
            stream << "[t=" << g_timer << ", " << get_peak_memory_in_kb()
                   << " KB] ";
        }

        stream << elem;
        return *this;
    }

    using manip_function = std::ostream& (*)(std::ostream&);
    Log& operator<<(manip_function f)
    {
        if (f == static_cast<manip_function>(&std::endl)) {
            line_has_started = false;
        }

        stream << f;
        return *this;
    }

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
    virtual ~Context();
    virtual std::string
    decorate_block_name(const std::string& block_name) const;
    void enter_block(const std::string& block_name);
    void leave_block(const std::string& block_name);
    std::string str() const;

    [[noreturn]]
    virtual void error(const std::string& message) const;
    virtual void warn(const std::string& message) const;
};

class MemoryContext : public Context {
    // The following constants affect the formatting of output.
    static const int MEM_FIELD_WIDTH = 7;
    static const int TIME_FIELD_WIDTH = 7;

public:
    virtual std::string
    decorate_block_name(const std::string& block_name) const override;
};

extern MemoryContext _memory_context;

class TraceBlock {
    Context& context;
    std::string block_name;

public:
    explicit TraceBlock(Context& context, const std::string& block_name);
    ~TraceBlock();
};

extern void trace_memory(const std::string& msg = "");
} // namespace utils

namespace std {
template <class T>
ostream& operator<<(ostream& stream, const vector<T>& vec)
{
    stream << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i != 0) stream << ", ";
        stream << vec[i];
    }
    stream << "]";
    return stream;
}
} // namespace std

#endif
