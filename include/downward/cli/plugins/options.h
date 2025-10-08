#ifndef PLUGINS_OPTIONS_H
#define PLUGINS_OPTIONS_H

#include "downward/utils/logging.h"
#include "downward/utils/system.h"

#include <any>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

namespace downward::cli::plugins {
/*
  Elements in the storage of the class Options are stored as std::any. Normally,
  the type stored inside the std::any objects matches the type we eventually use
  to extract the element but there are two exceptions.

  Lists are stored as vector<std::any> inside the options, so a normal
  std::any_cast will not work for them. We instead have to create a new vector
  and copy the casted elements into it. Since the new vector has to live
  somewhere, we do not support this for pointer to such vectors. Those will also
  not be created by the parser.

  Enums are stored as ints, so we have to extract them as such and cast the
  int into the enum types afterwards.
*/
template <typename ValueType>
struct OptionsAnyCaster {
    static ValueType cast(const std::any& operand)
    {
        return std::any_cast<ValueType>(operand);
    }
};

template <typename ValueType>
    requires std::is_enum_v<ValueType>
struct OptionsAnyCaster<ValueType> {
    static ValueType cast(const std::any& operand)
    {
        // Enums set within the code (options.set()) are already the right
        // ValueType...
        if (operand.type() == typeid(ValueType)) {
            return std::any_cast<ValueType>(operand);
        }
        // ... otherwise (Enums set over the command line) they are ints.
        return static_cast<ValueType>(std::any_cast<int>(operand));
    }
};

template <>
struct OptionsAnyCaster<std::chrono::duration<double>> {
    static std::chrono::duration<double> cast(const std::any& operand)
    {
        return static_cast<std::chrono::duration<double>>(
            std::any_cast<double>(operand));
    }
};

template <typename T>
struct OptionsAnyCaster<std::vector<T>> {
    static std::vector<T> cast(const std::any& operand)
    {
        if (operand.type() == typeid(std::vector<T>)) {
            return std::any_cast<std::vector<T>>(operand);
        }

        const std::vector<std::any>* any_elements =
            std::any_cast<std::vector<std::any>>(&operand);
        std::vector<T> result;
        result.reserve(any_elements->size());
        for (const std::any& element : *any_elements) {
            result.push_back(OptionsAnyCaster<T>::cast(element));
        }
        return result;
    }
};

// Wrapper for unordered_map<string, std::any>.
class Options {
    std::unordered_map<std::string, std::any> storage;
    std::string unparsed_config;

public:
    Options() = default;

    Options(const Options& other) = delete;
    Options& operator=(const Options& other) = delete;

    template <typename T>
    void set(const std::string& key, T value)
    {
        storage[key] = value;
    }

    template <typename T>
    std::shared_ptr<T> get_shared(const std::string& key) const
    {
        return get<std::shared_ptr<T>>(key);
    }

    template <typename T>
    std::shared_ptr<T>
    get_shared(const std::string& key, const std::shared_ptr<T>& default_value)
        const
    {
        return get<std::shared_ptr<T>>(key, default_value);
    }

    template <typename T>
    std::shared_ptr<T> get_shared(const std::string& key, std::nullptr_t) const
    {
        return get<std::shared_ptr<T>>(key, nullptr);
    }

    template <typename T>
    T get(const std::string& key) const
    {
        const auto it = storage.find(key);
        if (it == storage.end()) {
            throw downward::utils::CriticalError(
                "Attempt to retrieve non-existing object of name {} (type: {})",
                key,
                typeid(T).name());
        }
        try {
            T result = OptionsAnyCaster<T>::cast(it->second);
            return result;
        } catch (const std::bad_any_cast&) {
            throw downward::utils::CriticalError(
                "Invalid conversion while retrieving config options!\n"
                "{} is not of type {} but of type {}",
                key,
                typeid(T).name(),
                it->second.type().name());
        }
    }

    template <typename T>
    T get(const std::string& key, const T& default_value) const
    {
        if (storage.contains(key))
            return get<T>(key);
        else
            return default_value;
    }

    template <typename T>
    std::vector<T> get_list(const std::string& key) const
    {
        return get<std::vector<T>>(key);
    }

    bool contains(const std::string& key) const;
    const std::string& get_unparsed_config() const;
    void set_unparsed_config(const std::string& config);
};

template <typename T>
void verify_list_non_empty(
    const downward::utils::Context& context,
    const Options& opts,
    const std::string& key)
{
    if (std::vector<T> list = opts.get_list<T>(key); list.empty()) {
        context.error("List argument '{}' has to be non-empty.", key);
    }
}
} // namespace downward::cli::plugins

#endif
