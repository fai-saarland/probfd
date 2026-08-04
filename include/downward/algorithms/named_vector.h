#ifndef ALGORITHMS_NAMED_VECTOR_H
#define ALGORITHMS_NAMED_VECTOR_H

#include <cassert>
#include <string>
#include <vector>

namespace downward::named_vector {
/*
  NamedVector is a vector-like collection with optional names
  associated with each element. It is intended for attaching names
  to objects in vectors for debugging purposes and is optimized
  to have minimal overhead when there are no names. Any name which is
  not specified is the empty string. Accessing a name with
  an invalid index will result in an error in debug mode.
 */
template <typename T>
class NamedVector {
    std::vector<T> elements;
    std::vector<std::string> names;

public:
    template <typename... Args>
    T& emplace_back(Args&&... args)
    {
        return elements.emplace_back(std::forward<Args>(args)...);
    }

    void push_back(const T& element)
    {
        elements.push_back(element);
    }

    void push_back(T&& element)
    {
        elements.push_back(std::move(element));
    }

    T& operator[](std::size_t index)
    {
        return elements[index];
    }

    const T& operator[](std::size_t index) const
    {
        return elements[index];
    }

    bool has_names() const
    {
        return !names.empty();
    }

    void set_name(std::size_t index, const std::string& name)
    {
        if (index >= names.size()) {
            if (name.empty()) {
                // All unspecified names are empty by default.
                return;
            }

            names.resize(index + 1, "");
        }

        names[index] = name;
    }

    const std::string& get_name(std::size_t index) const
    {
        if (index < names.size()) {
            return names[index];
        }

        /*
          All unspecified names are empty by default. We use a static
          string here to avoid returning a reference to a local object.
        */
        static std::string empty;
        return empty;
    }

    std::size_t size() const
    {
        return elements.size();
    }

    bool empty() const
    {
        return elements.empty();
    }

    std::vector<T>::reference back()
    {
        return elements.back();
    }

    std::vector<T>::iterator begin()
    {
        return elements.begin();
    }

    std::vector<T>::iterator end()
    {
        return elements.end();
    }

    std::vector<T>::const_iterator begin() const
    {
        return elements.begin();
    }

    std::vector<T>::const_iterator end() const
    {
        return elements.end();
    }

    void clear()
    {
        elements.clear();
        names.clear();
    }

    void reserve(std::size_t capacity)
    {
        /* No space is reserved in the names vector because it is kept
           at minimal size and space is only used when necessary. */
        elements.reserve(capacity);
    }

    void resize(std::size_t count)
    {
        /* The names vector is not resized because it is kept
           at minimal size and only resized when necessary. */
        elements.resize(count);
    }

    void resize(std::size_t count, const T& value)
    {
        /* The names vector is not resized because it is kept
           at minimal size and only resized when necessary. */
        elements.resize(count, value);
    }
};
} // namespace downward::named_vector

#endif
