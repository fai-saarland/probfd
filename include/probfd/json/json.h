
#ifndef PROBFD_UTILS_JSON_H
#define PROBFD_UTILS_JSON_H

#include "probfd/json/tokenize.h"

#include <cmath>
#include <concepts>
#include <filesystem>
#include <format>
#include <forward_list>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <probfd/concepts.h>
#include <ranges>
#include <unordered_map>
#include <utility>
#include <vector>

namespace probfd::json {

class JsonElement {
public:
    enum class ElementID {
        OBJECT,
        ARRAY,
        STRING,
        BOOLEAN,
        INTEGER,
        FLOAT,
        NUL
    };

    ElementID id;

    explicit JsonElement(ElementID id)
        : id(id)
    {
    }

    virtual ~JsonElement() = default;

    virtual void print(std::ostream& os, int indent) = 0;
};

template <typename T, typename It>
class JsonObjectView;

class JsonObject : public JsonElement {
public:
    using iterator =
        std::map<std::string, std::unique_ptr<JsonElement>>::iterator;
    using const_iterator =
        std::map<std::string, std::unique_ptr<JsonElement>>::const_iterator;

    template <typename T>
    using read_view_t = JsonObjectView<T, const_iterator>;

private:
    std::map<std::string, std::unique_ptr<JsonElement>> members;

public:
    explicit JsonObject(
        std::map<std::string, std::unique_ptr<JsonElement>> members);

    template <std::ranges::input_range R>
    JsonObject(std::from_range_t, R&& range)
        : JsonElement(ElementID::ARRAY)
    {
        for (std::ranges::range_reference_t<R> p : range) {
            members.emplace(std::forward<std::ranges::range_value_t<R>>(p));
        }
    }

    template <typename... T>
    explicit JsonObject(T... range)
        : JsonElement(ElementID::ARRAY)
    {
        (members.emplace(std::get<0>(range), std::move(std::get<1>(range))),
         ...);
    }

    const JsonElement* get_field(const std::string& s) const;

    template <typename T>
    T read(const std::string& s) const;

    template <typename T>
    read_view_t<T> read_view() const;

    iterator begin() { return members.begin(); }

    iterator end() { return members.end(); }

    const_iterator begin() const { return members.begin(); }

    const_iterator end() const { return members.end(); }

    void print(std::ostream& os, int indent) override;
};

template <typename T, typename It>
class JsonArrayView;

class JsonArray : public JsonElement {
public:
    using iterator = std::vector<std::unique_ptr<JsonElement>>::iterator;
    using const_iterator =
        std::vector<std::unique_ptr<JsonElement>>::const_iterator;

    template <typename T>
    using read_view_t = JsonArrayView<T, const_iterator>;

private:
    std::vector<std::unique_ptr<JsonElement>> elements;

public:
    explicit JsonArray(std::vector<std::unique_ptr<JsonElement>> elements);

    template <std::ranges::input_range R>
    JsonArray(std::from_range_t, R&& range)
        : JsonElement(ElementID::ARRAY)
    {
        using E = std::ranges::range_reference_t<R>;

        for (E&& elem : range) { elements.push_back(std::forward<E>(elem)); }
    }

    template <typename... T>
        requires(std::convertible_to<T, std::unique_ptr<JsonElement>> && ...)
    explicit JsonArray(T&&... range)
        : JsonElement(ElementID::ARRAY)
        , elements({range...})
    {
    }

    void print(std::ostream& os, int indent) override;

    template <typename T>
    T read(std::size_t index) const;

    template <typename T>
    read_view_t<T> read_view() const;

    iterator begin() { return elements.begin(); }

    iterator end() { return elements.end(); }

    const_iterator begin() const { return elements.begin(); }

    const_iterator end() const { return elements.end(); }
};

template <typename T, typename It>
class JsonArrayView {
    class JsonArrayIterator {
        It it;

    public:
        using value_type = std::remove_reference_t<T>;
        using reference = T;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;

        JsonArrayIterator() = default;

        explicit JsonArrayIterator(It it);

        T operator*() const;

        JsonArrayIterator& operator++();

        void operator++(int) { ++*this; }

        friend bool operator==(
            const JsonArrayIterator& left,
            const JsonArrayIterator& right) = default;
    };

    const JsonArray& array;

public:
    explicit JsonArrayView(const JsonArray& array)
        : array(array)
    {
    }

    auto begin() { return JsonArrayIterator(array.begin()); }

    auto end() { return JsonArrayIterator(array.end()); }

    auto begin() const { return JsonArrayIterator(array.begin()); }

    auto end() const { return JsonArrayIterator(array.end()); }
};

template <typename T, typename It>
class JsonObjectView {
    class JsonObjectIterator {
        It it;

    public:
        using value_type = std::pair<std::string, std::remove_reference_t<T>>;
        using reference = std::pair<std::string, T>;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;

        JsonObjectIterator() = default;

        explicit JsonObjectIterator(It it);

        std::pair<std::string, T> operator*() const;

        JsonObjectIterator& operator++();

        void operator++(int) { ++*this; }

        friend bool operator==(
            const JsonObjectIterator& left,
            const JsonObjectIterator& right) = default;
    };

    const JsonObject& object;

public:
    explicit JsonObjectView(const JsonObject& object)
        : object(object)
    {
    }

    auto begin() { return JsonObjectIterator(object.begin()); }

    auto end() { return JsonObjectIterator(object.end()); }

    auto begin() const { return JsonObjectIterator(object.begin()); }

    auto end() const { return JsonObjectIterator(object.end()); }
};

template <typename T>
auto JsonArray::read_view() const -> read_view_t<T>
{
    return JsonArrayView<T, const_iterator>(*this);
}

template <typename T>
auto JsonObject::read_view() const -> read_view_t<T>
{
    return JsonObjectView<T, const_iterator>(*this);
}

struct JsonString final : JsonElement {
    std::string string;

    explicit JsonString(std::string string)
        : JsonElement(ElementID::STRING)
        , string(std::move(string))
    {
    }

    void print(std::ostream& os, int indent) override;
};

struct JsonBoolean final : JsonElement {
    bool value;

    explicit JsonBoolean(bool value)
        : JsonElement(ElementID::BOOLEAN)
        , value(value)
    {
    }

    void print(std::ostream& os, int indent) override;
};

struct JsonInteger final : JsonElement {
    long long int value;

    explicit JsonInteger(long long int value)
        : JsonElement(ElementID::INTEGER)
        , value(value)
    {
    }

    void print(std::ostream& os, int indent) override;
};

struct JsonFloat final : JsonElement {
    long double value;

    explicit JsonFloat(long double value)
        : JsonElement(ElementID::FLOAT)
        , value(value)
    {
    }

    void print(std::ostream& os, int indent) override;
};

struct JsonNull final : JsonElement {
    explicit JsonNull()
        : JsonElement(ElementID::NUL)
    {
    }

    void print(std::ostream& os, int indent) override;
};

template <typename T>
    requires(std::is_object_v<T>)
T read(std::istream& is);

template <typename T>
    requires(std::is_object_v<T>)
T read(const JsonElement& element);

template <typename T>
T JsonObject::read(const std::string& s) const
{
    const auto* field = get_field(s);

    if (!field) {
        throw std::invalid_argument(
            std::format("Object does not have field {}", s));
    }

    return json::read<T>(*field);
}

template <typename T, typename It>
JsonArrayView<T, It>::JsonArrayIterator::JsonArrayIterator(It it)
    : it(std::move(it))
{
}

template <typename T, typename It>
T JsonArrayView<T, It>::JsonArrayIterator::operator*() const
{
    return json::read<T>(**it);
}

template <typename T, typename It>
auto JsonArrayView<T, It>::JsonArrayIterator::operator++() -> JsonArrayIterator&
{
    ++it;
    return *this;
}

template <typename T, typename It>
JsonObjectView<T, It>::JsonObjectIterator::JsonObjectIterator(It it)
    : it(std::move(it))
{
}

template <typename T, typename It>
std::pair<std::string, T>
JsonObjectView<T, It>::JsonObjectIterator::operator*() const
{
    return {it->first, json::read<T>(*it->second)};
}

template <typename T, typename It>
auto JsonObjectView<T, It>::JsonObjectIterator::operator++()
    -> JsonObjectIterator&
{
    ++it;
    return *this;
}

template <typename T>
T JsonArray::read(std::size_t index) const
{
    if (index >= elements.size()) {
        throw std::invalid_argument(
            std::format("Array does not have index {}", index));
    }

    return json::read<T>(*elements[index]);
}

std::unique_ptr<JsonElement> parse_element(
    const std::vector<Token>& tokens,
    std::vector<Token>::const_iterator& it);

std::unique_ptr<JsonElement> parse_element(const std::vector<Token>& tokens);

std::unique_ptr<JsonObject> parse_object(
    const std::vector<Token>& tokens,
    std::vector<Token>::const_iterator& it);

std::unique_ptr<JsonObject> parse_object(const std::vector<Token>& tokens);

std::unique_ptr<JsonArray> parse_array(
    const std::vector<Token>& tokens,
    std::vector<Token>::const_iterator& it);

std::unique_ptr<JsonArray> parse_array(const std::vector<Token>& tokens);

template <typename R>
concept Container =
    std::ranges::input_range<R> &&
    std::constructible_from<
        R,
        std::ranges::iterator_t<
            JsonArray::read_view_t<std::ranges::range_value_t<R>>>,
        std::ranges::iterator_t<
            JsonArray::read_view_t<std::ranges::range_value_t<R>>>>;

template <typename R>
concept PairContainer =
    std::ranges::input_range<R> &&
    downward::PairLike<std::ranges::range_value_t<R>> &&
    std::convertible_to<
        std::tuple_element_t<0, std::ranges::range_value_t<R>>,
        std::string> &&
    std::constructible_from<
        R,
        std::ranges::iterator_t<JsonObject::read_view_t<
            std::tuple_element_t<1, std::ranges::range_value_t<R>>>>,
        std::ranges::iterator_t<JsonObject::read_view_t<
            std::tuple_element_t<1, std::ranges::range_value_t<R>>>>>;

template <typename T>
concept JsonObjectReadable = std::constructible_from<T, JsonObject&>;

template <typename T>
    requires(std::is_object_v<T>)
T read(const JsonElement& element)
{
    if constexpr (std::same_as<T, bool>) {
        if (element.id == JsonElement::ElementID::BOOLEAN) {
            return static_cast<const JsonBoolean&>(element).value;
        }

        throw std::invalid_argument("Could not read boolean.");
    } else if constexpr (std::integral<T>) {
        if (element.id == JsonElement::ElementID::INTEGER) {
            return static_cast<T>(
                static_cast<const JsonInteger&>(element).value);
        }

        throw std::invalid_argument("Could not read integer.");
    } else if constexpr (std::floating_point<T>) {
        if (element.id == JsonElement::ElementID::FLOAT) {
            return static_cast<T>(static_cast<const JsonFloat&>(element).value);
        } else if (element.id == JsonElement::ElementID::INTEGER) {
            return static_cast<T>(
                static_cast<const JsonInteger&>(element).value);
        } else if (element.id == JsonElement::ElementID::STRING) {
            if (const auto& json_string =
                    static_cast<const JsonString&>(element);
                json_string.string == "inf" ||
                json_string.string == "infinity") {
                return std::numeric_limits<T>::infinity();
            } else if (
                json_string.string == "-inf" ||
                json_string.string == "-infinity") {
                return -std::numeric_limits<T>::infinity();
            } else if (json_string.string == "NaN") {
                return std::numeric_limits<T>::quiet_NaN();
            } else if (json_string.string == "-NaN") {
                return -std::numeric_limits<T>::quiet_NaN();
            }
        }

        throw std::invalid_argument("Could not read float.");
    } else if constexpr (std::same_as<T, std::string>) {
        if (element.id == JsonElement::ElementID::STRING) {
            return static_cast<T>(
                static_cast<const JsonString&>(element).string);
        }

        throw std::invalid_argument("Could not read string.");
    } else if constexpr (JsonObjectReadable<T>) {
        if (element.id == JsonElement::ElementID::OBJECT) {
            return T(static_cast<const JsonObject&>(element));
        }

        throw std::invalid_argument("Could not read user-defined object.");
    } else if constexpr (PairContainer<T>) {
        if (element.id == JsonElement::ElementID::OBJECT) {
            const auto& object = static_cast<const JsonObject&>(element);
            auto view = object.read_view<
                std::tuple_element_t<1, std::ranges::range_value_t<T>>>();
            return T(view.begin(), view.end());
        }

        throw std::invalid_argument("Could not read user-defined object.");
    } else if constexpr (Container<T>) {
        if (element.id == JsonElement::ElementID::ARRAY) {
            const auto& array = static_cast<const JsonArray&>(element);
            auto view = array.read_view<std::ranges::range_value_t<T>>();
            return T(view.begin(), view.end());
        }

        throw std::invalid_argument("Could not read user-defined object.");
    } else {
        static_assert(false, "Cannot read this type!");
        abort();
    }
}

template <typename T>
    requires(std::is_object_v<T>)
T read(std::istream& is)
{
    const auto tokens = tokenize(is);
    const auto element = parse_element(tokens);
    return read<T>(*element);
}

template <typename T>
    requires(std::is_object_v<T>)
T read(const std::filesystem::path& path)
{
    std::ifstream file(path);
    return read<T>(file);
}

namespace detail {

template <typename T>
void to_json(T&&) = delete;

template <typename T>
concept UnqJsonConvertible = requires(const T& t) {
    { to_json(t) } -> std::convertible_to<std::unique_ptr<JsonElement>>;
};

} // namespace detail

template <typename T>
    requires(std::is_object_v<T>)
struct to_json_t;

template <typename T>
concept JsonConvertible =
    requires(const T& t, to_json_t<std::remove_cvref_t<T>> j) {
        { j(t) } -> std::convertible_to<std::unique_ptr<JsonElement>>;
    };

template <typename T>
constexpr bool enable_json_array = false;

template <typename T>
concept JsonArrayConvertibleRange =
    std::ranges::input_range<T> &&
    JsonConvertible<std::ranges::range_value_t<T>> && enable_json_array<T>;

template <typename T>
constexpr bool enable_json_object = false;

template <typename R>
concept JsonObjectConvertibleRange =
    std::ranges::input_range<R> &&
    std::convertible_to<
        std::ranges::range_value_t<R>,
        std::pair<const std::string, std::unique_ptr<JsonElement>>> &&
    JsonConvertible<std::ranges::range_value_t<R>> && enable_json_object<R>;

template <typename T>
    requires(std::is_object_v<T>)
struct to_json_t {
    auto operator()(const T& value) const
        requires(detail::UnqJsonConvertible<T>)
    {
        return to_json(value);
    }

    auto operator()(T value) const
        requires(
            std::same_as<T, bool> ||
            std::same_as<T, std::vector<bool>::reference> ||
            std::same_as<T, std::vector<bool>::const_reference>)
    {
        return std::make_unique<JsonBoolean>(value);
    }

    auto operator()(T value) const
        requires(std::integral<T> && !std::same_as<T, bool>)
    {
        return std::make_unique<JsonInteger>(value);
    }

    auto operator()(T value) const
        requires(std::floating_point<T>)
    {
        return std::make_unique<JsonFloat>(value);
    }

    auto operator()(const T& value) const
        requires(std::convertible_to<T, std::string>)
    {
        return std::make_unique<JsonString>(std::forward<T>(value));
    }

    auto operator()(const T& value) const
        requires(JsonArrayConvertibleRange<T>)
    {
        return std::make_unique<JsonArray>(
            std::from_range,
            value | std::views::transform([]<typename U>(const U& el) {
                return to_json_t<U>{}(el);
            }));
    }

    auto operator()(const T& value) const
        requires(JsonObjectConvertibleRange<T>)
    {
        return std::make_unique<JsonObject>(
            std::from_range,
            value | std::views::transform(
                        []<typename S, typename U>(std::pair<S, U>& p) {
                            return std::forward_as_tuple(
                                std::get<0>(p),
                                to_json_t<U>{}(std::get<1>(p)));
                        }));
    }
};

template <typename T>
auto to_json(const T& value)
{
    return to_json_t<T>{}(value);
}

template <JsonConvertible... T>
auto make_array(const T&... range)
{
    return std::make_unique<JsonArray>(
        json::to_json(std::forward<T>(range))...);
}

inline auto wrap_pairs()
{
    return std::tuple<>();
}

template <
    std::convertible_to<std::string> S,
    JsonConvertible T,
    typename... Args>
auto wrap_pairs(S&& head_string, const T& head_value, Args&&... tail)
{
    using R = std::invoke_result_t<to_json_t<T>, const T&>;

    auto&& i = wrap_pairs(std::forward<Args>(tail)...);

    return std::apply(
        [&]<typename... Tuples>(
            Tuples&&... args) -> std::tuple<std::pair<S&&, R>, Tuples...> {
            return std::make_tuple<std::pair<S&&, R>, Tuples...>(
                std::pair<S&&, R>(
                    std::forward<S>(head_string),
                    json::to_json(head_value)),
                std::forward<Tuples>(args)...);
        },
        std::move(i));
}

template <typename... Args>
auto make_object(Args&&... args)
{
    return std::apply(
        []<typename... A>(A&&... t) {
            return std::make_unique<JsonObject>(std::forward<A>(t)...);
        },
        wrap_pairs(std::forward<Args>(args)...));
}

template <JsonConvertible T>
void write(std::ostream& os, const T& t)
{
    auto result = json::to_json(t);
    result->print(os, 0);
}

template <JsonConvertible T>
void write(const std::filesystem::path& path, const T& t)
{
    std::ofstream file(path);
    return write(file, t);
}

template <typename T, typename A>
constexpr bool enable_json_array<std::vector<T, A>> = true;

template <typename T, typename A>
constexpr bool enable_json_array<std::list<T, A>> = true;

template <typename T, typename A>
constexpr bool enable_json_array<std::forward_list<T, A>> = true;

template <typename K, typename V, typename C, typename A>
    requires std::convertible_to<K, std::string>
constexpr bool enable_json_object<std::map<K, V, C, A>> = true;

template <typename K, typename V, typename C, typename A>
    requires std::convertible_to<K, std::string>
constexpr bool enable_json_object<std::unordered_map<K, V, C, A>> = true;

static_assert(Container<std::vector<int>>);
static_assert(PairContainer<std::map<std::string, std::string>>);

} // namespace probfd::json

#endif // PROBFD_UTILS_JSON_H
