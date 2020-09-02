#pragma once

#include <iterator>
#include <vector>

class PartialState {
public:
    class const_iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value = std::pair<int, int>;
        using difference_type = int;
        using pointer = const value*;
        using reference = value;

        const_iterator(const const_iterator&) = default;
        const_iterator(const_iterator&&) = default;
        ~const_iterator() = default;

        const_iterator& operator=(const const_iterator&) = default;
        const_iterator& operator=(const_iterator&&) = default;
        const_iterator& operator++();
        const_iterator operator++(int);
        bool operator==(const const_iterator& other) const;
        bool operator!=(const const_iterator& other) const;
        reference operator*() const;

    private:
        friend class PartialState;
        explicit const_iterator(const PartialState* state, unsigned var);
        explicit const_iterator(const PartialState* state);
        const PartialState* state_;
        unsigned var_;
    };

    explicit PartialState(std::vector<int>* values);
    explicit PartialState(const std::vector<int>& values);
    explicit PartialState(std::vector<int>&& values);
    explicit PartialState(unsigned num_vars);
    ~PartialState();

    int operator[](int var) const;
    int& operator[](int var);
    bool is_defined(int var) const;

    const_iterator begin() const;
    const_iterator end() const;

    void clear();

private:
    friend class const_iterator;
    std::vector<int>* values_;
    bool delete_ptr_;
};
