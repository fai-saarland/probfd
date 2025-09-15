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
        // std::any_cast returns a copy here, not a reference.
        const std::vector<std::any> any_elements =
            std::any_cast<std::vector<std::any>>(operand);
        std::vector<T> result;
        result.reserve(any_elements.size());
        for (const std::any& element : any_elements) {
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
    explicit Options();
    /*
      TODO: we only need the copy constructor for cases where we need to modify
      the options after parsing (see merge_and_shrink_heuristic.cc for an
      example). This should no longer be necessary once we switch to builders.
      At this time, the constructor can probably be deleted.
    */
    Options(const Options& other) = default;

    template <typename T>
    void set(const std::string& key, T value)
    {
        storage[key] = value;
    }

    template <typename T>
    T get(const std::string& key) const
    {
        const auto it = storage.find(key);
        if (it == storage.end()) {
            ABORT(
                "Attempt to retrieve nonexisting object of name " + key +
                " (type: " + typeid(T).name() + ")");
        }
        try {
            T result = OptionsAnyCaster<T>::cast(it->second);
            return result;
        } catch (const std::bad_any_cast&) {
            ABORT(
                "Invalid conversion while retrieving config options!\n" + key +
                " is not of type " + typeid(T).name() + " but of type " +
                it->second.type().name());
        }
    }

    template <typename T>
    T get(const std::string& key, const T& default_value) const
    {
        if (storage.count(key))
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
    std::vector<T> list = opts.get_list<T>(key);
    if (list.empty()) {
        context.error("List argument '" + key + "' has to be non-empty.");
    }
}
} // namespace downward::cli::plugins

#endif
