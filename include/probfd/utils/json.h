
#ifndef PROBFD_UTILS_JSON_H
#define PROBFD_UTILS_JSON_H

#include <concepts>
#include <format>
#include <iostream>
#include <ranges>
#include <vector>

namespace probfd::json {

template <typename T>
void write(std::ostream& os, const T& t);

template <typename T>
T read(std::istream& is);

template <typename T>
concept JsonReadable = requires(std::istream& is) {
    {
        T::read_json(is)
    } -> std::same_as<T>;
};

template <typename T>
concept Writable = requires(std::ostream& os, T t) { json::write(os, t); };

template <typename T>
concept CustomDumpable = requires(std::ostream& os, T t) { dump_json(os, t); };

template <typename T>
concept DumpableRange =
    std::ranges::input_range<T> && Writable<std::ranges::range_value_t<T>>;

template <typename T>
concept WritablePairLike = requires(const T& t) {
    {
        std::get<0>(t)
    } -> Writable;
    {
        std::get<1>(t)
    } -> Writable;
};

template <typename T>
concept WritableObject = std::ranges::input_range<T> &&
                         WritablePairLike<std::ranges::range_value_t<T>>;

template <WritableObject R>
struct JsonObjectWrapper {
    R&& range;
};

template <typename T>
struct is_object_wrapped_helper : std::false_type {};

template <typename T>
struct is_object_wrapped_helper<JsonObjectWrapper<T>> : std::true_type {};

template <typename T>
static constexpr bool is_object_wrapped = is_object_wrapped_helper<T>::value;

template <typename R>
auto as_object(R&& range)
{
    return JsonObjectWrapper<R>(range);
}

template <typename T>
void write(std::ostream& os, const T& t)
{
    if constexpr (
        std::is_same_v<T, bool> ||
        std::is_same_v<std::vector<bool>::const_reference, T>) {
        os << (t ? "true" : "false");
    } else if constexpr (std::integral<T>) {
        os << t;
    } else if constexpr (std::floating_point<T>) {
        switch (std::fpclassify(t)) {
        case FP_INFINITE:
            os << "\"";
            if (t < 0) os << "-";
            os << "inf\"";
            break;

        default: os << t;
        }
    } else if constexpr (
        std::is_convertible_v<T, const char*> ||
        std::is_same_v<T, std::string>) {
        os << '"' << t << '"';
    } else if constexpr (CustomDumpable<T>) {
        dump_json(os, t);
    } else if constexpr (DumpableRange<T>) {
        os << "[";
        if (!t.empty()) {
            write(os, t.front());
            for (const auto& elem : t | std::views::drop(1)) {
                os << ',';
                write(os, elem);
            }
        }
        os << "]";
    } else if constexpr (is_object_wrapped<T>) {
        os << "{";
        if (!t.range.empty()) {
            auto&& first = t.range.front();
            write(os, std::get<0>(first));
            os << ':';
            write(os, std::get<1>(first));
            for (const auto& elem : t.range | std::views::drop(1)) {
                os << ',';
                write(os, std::get<0>(elem));
                os << ':';
                write(os, std::get<1>(elem));
            }
        }
        os << "}";
    } else {
        static_assert(!std::is_same_v<T, T>, "Type not dumpable!");
    }
}

template <typename T, typename... R>
void write_array(std::ostream& os, T&& head, R&&... tail)
    requires Writable<std::remove_cvref<T>> &&
             (Writable<std::remove_cvref<R>> && ...)
{
    os << "[";
    write(os, head);
    (write(os << ',', tail), ...);
    os << "]";
}

template <typename T, typename... R>
void write_object(std::ostream& os, T&& head, R&&... tail)
    requires WritablePairLike<T> && (WritablePairLike<R> && ...)
{
    os << "{";
    write(os, std::get<0>(head));
    os << ':';
    write(os, std::get<1>(head));
    ((os << ',',
      write(os, std::get<0>(tail)),
      os << ':',
      write(os, std::get<1>(tail))),
     ...);
    os << "}";
}

inline void expect(std::istream& is, const std::string& text)
{
    for (char c : text) {
        if (is.get() != c)
            throw std::invalid_argument(std::format("expected \"{}\"", text));
    }
}

template <typename T>
class json_iterator;

template <typename T>
bool operator==(const json_iterator<T>& left, std::default_sentinel_t);

template <typename T>
class json_iterator {
    std::istream* is;

public:
    using value_type = T;
    using pointer = T*;
    using reference = T;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;

    explicit json_iterator(std::istream& is)
        : is(&is)
    {
    }

    T operator*() const { return read<T>(*is); }

    json_iterator& operator++()
    {
        (*is) >> std::ws;
        if (is->peek() == ',') {
            is->ignore();
            (*is) >> std::ws;
        }
        return *this;
    }

    void operator++(int) { ++(*this); }

    friend bool
    operator== <T>(const json_iterator<T>& left, std::default_sentinel_t);
};

template <typename T>
bool operator==(const json_iterator<T>& left, std::default_sentinel_t)
{
    return left.is->peek() == ']';
}

template <typename R>
concept Container = std::ranges::input_range<R> &&
                    std::constructible_from<
                        R,
                        json_iterator<std::ranges::range_value_t<R>>,
                        json_iterator<std::ranges::range_value_t<R>>>;

template <typename T>
T read(std::istream& is)
{
    if constexpr (std::is_same_v<T, bool>) {
        if (is.peek() == 't' && (is.ignore(), is.get() == 'r') &&
            is.get() == 'u' && is.get() == 'e') {
            return true;
        } else if (
            is.get() == 'f' && is.get() == 'a' && is.get() == 'l' &&
            is.get() == 's' && is.get() == 'e') {
            return false;
        }

        throw std::invalid_argument("Expected 'true' of 'false'!");
    } else if constexpr (std::is_integral_v<T>) {
        T number;
        is >> number;
        return number;
    } else if constexpr (std::floating_point<T>) {
        T number;

        if (is.peek() == '\"') {
            // Expect infinity
            is.ignore();
            if (is.peek() == '-') {
                is.ignore(5);
                return -std::numeric_limits<T>::infinity();
            } else {
                is.ignore(4);
                return std::numeric_limits<T>::infinity();
            }
        }

        is >> number;
        return number;
    } else if constexpr (std::is_same_v<T, std::string>) {
        std::string text;
        is >> text;
        return text;
    } else if constexpr (JsonReadable<T>) {
        return T::read_json(is);
    } else if constexpr (Container<T>) {
        is >> std::ws;
        expect(is, "[");
        is >> std::ws;

        json_iterator<std::ranges::range_value_t<T>> it(is);

        auto r = std::ranges::subrange{it, std::default_sentinel};
        auto r2 = std::ranges::common_view{r};

        T object(r2.begin(), r2.end());
        expect(is, "]");
        return object;
    } else {
        static_assert("Cannot read this type!");
    }
}

template <typename F, typename... T>
std::tuple<F, T...> read_array(std::istream& is)
{
    expect(is, "[");
    is >> std::ws;
    std::tuple<F, T...> t{
        read<F>(is),
        (expect(is, ","), is >> std::ws, read<T>(is))...};
    is >> std::ws;
    expect(is, "]");
    is >> std::ws;
    return t;
}

template <typename F, typename... T>
std::tuple<F, T...>
read_object(std::istream& is, auto&& first, auto&&... tokens)
{
    expect(is, "{");
    is >> std::ws;
    std::tuple<F, T...> t{
        (expect(is, "\""),
         expect(is, first),
         expect(is, "\""),
         is >> std::ws,
         expect(is, ":"),
         is >> std::ws,
         read<F>(is)),
        (is >> std::ws,
         expect(is, ","),
         is >> std::ws,
         expect(is, "\""),
         expect(is, tokens),
         expect(is, "\""),
         is >> std::ws,
         expect(is, ":"),
         is >> std::ws,
         read<T>(is))...};
    is >> std::ws;
    expect(is, "}");
    is >> std::ws;
    return t;
}

template <typename T, typename... Args>
T construct_from_tuple(std::tuple<Args...>&& tuple)
{
    auto construct_from_tuple = []<std::size_t... ints>(
                                    std::tuple<Args...>&& tuple,
                                    std::index_sequence<ints...>) {
        return T(std::get<ints>(tuple)...);
    };

    return construct_from_tuple(
        std::move(tuple),
        std::index_sequence_for<Args...>{});
}

template <typename T, typename... Args>
T construct_from_array(std::istream& is)
{
    return construct_from_tuple<T>(json::read_array<Args...>(is));
}

template <typename T, typename... Args>
T construct_from_object(std::istream& is, auto&&... tokens)
    requires(sizeof...(tokens) == sizeof...(Args))
{
    return construct_from_tuple<T>(read_object<Args...>(is, tokens...));
}

} // namespace probfd::json

#endif // PROBFD_UTILS_JSON_H
