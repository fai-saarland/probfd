#pragma once

#include <algorithm>
#include <cassert>
#include <iterator>
#include <type_traits>
#include <vector>

namespace ub_tree {

namespace defaults {

template<typename V>
struct allocator {
    template<typename... T>
    V operator()(const T&...) const
    {
        return V();
    }
};

template<typename V>
using value_store = std::vector<V>;

} // namespace defaults

template<
    typename K,
    typename V,
    typename Allocator = defaults::allocator<V>,
    typename InternalValueStorage = defaults::value_store<V>>
class UBTree {
public:
    using key_type = K;
    using key_set = std::vector<key_type>;
    using value_type = V;

    class iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = V;
        using difference_type = int;
        using point = value_type*;
        using reference = value_type&;

        iterator(const iterator&) = default;
        iterator(iterator&&) = default;
        iterator& operator=(const iterator&) = default;
        iterator& operator=(iterator&&) = default;

        iterator& operator++()
        {
            index_++;
            return *this;
        }

        iterator operator++(int i)
        {
            iterator result(*this);
            index_ += i;
            return result;
        }

        reference operator*() const { return tree_->values_[index_]; }

        bool operator==(const iterator& other) const
        {
            return tree_ == other.tree_ && index_ == other.index_;
        }

        bool operator!=(const iterator& other) const
        {
            return !(*this == other);
        }

    private:
        friend class UBTree<K, V, Allocator, InternalValueStorage>;
        explicit iterator(
            UBTree<K, V, Allocator, InternalValueStorage>* tree,
            unsigned index)
            : tree_(tree)
            , index_(index)
        {
        }

        UBTree<K, V, Allocator, InternalValueStorage>* tree_;
        unsigned index_;
    };

    class const_iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = V;
        using difference_type = int;
        using point = const value_type*;
        using reference = const value_type&;

        const_iterator(const const_iterator&) = default;
        const_iterator(const_iterator&&) = default;
        const_iterator& operator=(const const_iterator&) = default;
        const_iterator& operator=(const_iterator&&) = default;

        const_iterator& operator++()
        {
            index_++;
            return *this;
        }

        const_iterator operator++(int i)
        {
            const_iterator result(*this);
            index_ += i;
            return result;
        }

        reference operator*() const { return tree_->values_[index_]; }

        bool operator==(const const_iterator& other) const
        {
            return tree_ == other.tree_ && index_ == other.index_;
        }

        bool operator!=(const const_iterator& other) const
        {
            return !(*this == other);
        }

    private:
        friend class UBTree<K, V, Allocator, InternalValueStorage>;
        explicit const_iterator(
            const UBTree<K, V, Allocator, InternalValueStorage>* tree,
            unsigned index)
            : tree_(tree)
            , index_(index)
        {
        }

        const UBTree<K, V, Allocator, InternalValueStorage>* tree_;
        unsigned index_;
    };

    UBTree(Allocator allocator = Allocator());
    ~UBTree() = default;

    UBTree(const UBTree&) = delete;
    UBTree(UBTree&&) = delete;
    UBTree& operator=(const UBTree&) = delete;
    UBTree& operator=(UBTree&&) = delete;

    iterator begin();
    const_iterator begin() const;
    iterator end();
    const_iterator end() const;

    unsigned size() const;
    bool contains(const key_set& keys) const;
    bool contains_subset(const key_set& keys) const;

    V& operator[](const key_set& keys);
    V& operator[](unsigned idx);
    const V& operator[](unsigned idx) const;

    bool insert(const key_set& key_set, const V& value);

    void clear();

    template<typename F>
    void apply_to_subsets(const key_set& keys, F fun);

    template<typename F>
    void apply_to_supersets(const key_set& keys, F fun);

    bool contains_disjoint_key(const key_set& keys) const;

private:
    friend class iterator;
    friend class const_iterator;

    struct Node;
    struct RootNode {
        RootNode()
            : value_idx(-1)
        {
        }

        ~RootNode() = default;
        RootNode(const RootNode&) = default;
        RootNode(RootNode&&) = default;
        RootNode& operator=(const RootNode&) = default;
        RootNode& operator=(RootNode&&) = default;

        Node& insert_or_get(const K& k)
        {
            auto pos =
                std::lower_bound(successors.begin(), successors.end(), k);
            if (pos == successors.end() || k < pos->key) {
                pos = successors.insert(pos, Node(k));
            }
            return *pos;
        }

        Node& insert_or_get_trace(const key_set& keys)
        {
            assert(!keys.empty());
            Node* node = &(insert_or_get(keys[0]));
            for (unsigned i = 1; i < keys.size(); i++) {
                node = &(node->insert_or_get(keys[i]));
            }
            return *node;
        }

        const Node* get(const K& k) const
        {
            auto pos =
                std::lower_bound(successors.begin(), successors.end(), k);
            if (pos == successors.end() || k < pos->key) {
                return nullptr;
            }
            return &(*pos);
        }

        const Node* trace(const key_set& keys) const
        {
            assert(!keys.empty());
            const Node* node = get(keys[0]);
            for (unsigned i = 1; node != nullptr && i < keys.size(); i++) {
                node = node->get(keys[i]);
            }
            return node;
        }

        bool contains_disjoint_key(const key_set& keys, unsigned idx) const
        {
            if (value_idx >= 0) {
                return true;
            }
            for (unsigned succ = 0; succ < successors.size(); succ++) {
                while (idx < keys.size() && keys[idx] < successors[succ].key) {
                    ++idx;
                }
                if (idx == keys.size()
                    || (successors[succ].key < keys[idx]
                        && successors[succ].contains_disjoint_key(keys, idx))) {
                    return true;
                }
            }
            return false;
        }

        bool contains_subset(const key_set& keys, unsigned key_idx) const
        {
            if (value_idx >= 0) {
                return true;
            }
            auto it = successors.begin();
            for (; key_idx < keys.size(); ++key_idx) {
                const K& k = keys[key_idx];
                it = std::lower_bound(it, successors.end(), k);
                if (it == successors.end()) {
                    break;
                }
                if (!(k < it->key) && it->contains_subset(keys, key_idx + 1)) {
                    return true;
                }
            }
            return false;
        }

        template<typename Fun, typename... Args>
        static bool
        forward_call(const std::true_type, Fun& fun, const Args&... args)
        {
            return fun(args...);
        }

        template<typename Fun, typename... Args>
        static bool
        forward_call(const std::false_type, Fun& fun, const Args&... args)
        {
            fun(args...);
            return false;
        }

        template<typename Fun>
        bool apply_to_subsets(
            const key_set& keys,
            Fun& fun,
            unsigned key_idx,
            key_set& subset) const
        {
            using return_type =
                typename std::result_of<Fun(const key_set&, unsigned)>::type;
            static auto early = std::is_convertible<return_type, bool>();
            if (value_idx >= 0) {
                if (forward_call(fun(early, subset, value_idx))) {
                    return true;
                }
            }
            auto it = successors.begin();
            for (; key_idx < keys.size(); ++key_idx) {
                const K& k = keys[key_idx];
                it = std::lower_bound(it, successors.end(), k);
                if (it == successors.end()) {
                    break;
                }
                if (!(k < it->key)) {
                    subset.push_back(k);
                    if (it->apply_to_key_subsets(
                            keys, fun, key_idx + 1, subset)) {
                        return true;
                        subset.pop_back();
                    }
                    subset.pop_back();
                }
            }
            return false;
        }

        template<typename Fun>
        bool apply_to_supersets(const key_set& keys, Fun& fun, unsigned key_idx)
            const
        {
            if (key_idx == keys.size()) {
                if (apply(fun)) {
                    return true;
                }
            } else {
                auto end = std::lower_bound(
                    successors.begin(), successors.end(), keys[key_idx]);
                for (auto it = successors.begin(); it != end; ++it) {
                    if (it->apply_to_supersets(keys, fun, key_idx)) {
                        return true;
                    }
                }
                if (end != successors.end() && !(keys[key_idx] < end->key)) {
                    if (end->apply_to_supersets(keys, fun, key_idx + 1)) {
                        return true;
                    }
                }
            }
            return false;
        }

        template<typename Fun>
        bool apply(Fun& fun) const
        {
            using return_type = typename std::result_of<Fun(unsigned)>::type;
            static auto early = std::is_convertible<return_type, bool>();
            if (value_idx >= 0) {
                if (forward_call(early, fun, value_idx)) {
                    return true;
                }
            }
            for (unsigned i = 0; i < successors.size(); i++) {
                if (successors[i].apply(fun)) {
                    return true;
                }
            }
            return false;
        }

        int value_idx;
        std::vector<Node> successors;
    };
    struct Node : public RootNode {
        explicit Node(const K& key)
            : RootNode()
            , key(key)
        {
        }
        bool operator<(const Node& other) const { return key < other.key; }
        bool operator<(const K& k) const { return key < k; }

        K key;
    };

    unsigned size_;
    RootNode root_;
    Allocator value_allocator_;
    InternalValueStorage values_;
};

template<
    typename K,
    typename V,
    typename Allocator,
    typename InternalValueStorage>
UBTree<K, V, Allocator, InternalValueStorage>::UBTree(Allocator allocator)
    : size_(0)
    , value_allocator_(allocator)
{
}

template<
    typename K,
    typename V,
    typename Allocator,
    typename InternalValueStorage>
typename UBTree<K, V, Allocator, InternalValueStorage>::iterator
UBTree<K, V, Allocator, InternalValueStorage>::begin()
{
    return iterator(this, 0);
}

template<
    typename K,
    typename V,
    typename Allocator,
    typename InternalValueStorage>
typename UBTree<K, V, Allocator, InternalValueStorage>::const_iterator
UBTree<K, V, Allocator, InternalValueStorage>::begin() const
{
    return const_iterator(this, 0);
}

template<
    typename K,
    typename V,
    typename Allocator,
    typename InternalValueStorage>
typename UBTree<K, V, Allocator, InternalValueStorage>::iterator
UBTree<K, V, Allocator, InternalValueStorage>::end()
{
    return iterator(this, size_);
}

template<
    typename K,
    typename V,
    typename Allocator,
    typename InternalValueStorage>
typename UBTree<K, V, Allocator, InternalValueStorage>::const_iterator
UBTree<K, V, Allocator, InternalValueStorage>::end() const
{
    return const_iterator(this, size_);
}

template<
    typename K,
    typename V,
    typename Allocator,
    typename InternalValueStorage>
unsigned
UBTree<K, V, Allocator, InternalValueStorage>::size() const
{
    return size_;
}

template<
    typename K,
    typename V,
    typename Allocator,
    typename InternalValueStorage>
bool
UBTree<K, V, Allocator, InternalValueStorage>::contains(
    const key_set& keys) const
{
    if (keys.empty()) {
        return root_.value_idx >= 0;
    }
    const Node* node = root_.trace(keys);
    return node != nullptr && node->value_idx >= 0;
}

template<
    typename K,
    typename V,
    typename Allocator,
    typename InternalValueStorage>
bool
UBTree<K, V, Allocator, InternalValueStorage>::contains_subset(
    const key_set& keys) const
{
    if (keys.empty()) {
        return root_.value_idx >= 0;
    }
    return root_.contains_subset(keys, 0);
}

template<
    typename K,
    typename V,
    typename Allocator,
    typename InternalValueStorage>
V&
UBTree<K, V, Allocator, InternalValueStorage>::operator[](const key_set& keys)
{
    RootNode* node = nullptr;
    if (keys.empty()) {
        node = &root_;
    } else {
        node = &(root_.insert_or_get_trace(keys));
    }
    if (node->value_idx < 0) {
        size_++;
        node->value_idx = values_.size();
        values_.push_back(value_allocator_(keys));
    }
    return values_[node->value_idx];
}

template<
    typename K,
    typename V,
    typename Allocator,
    typename InternalValueStorage>
bool
UBTree<K, V, Allocator, InternalValueStorage>::insert(
    const key_set& keys,
    const V& value)
{

    RootNode* node = nullptr;
    if (keys.empty()) {
        node = &root_;
    } else {
        node = &(root_.insert_or_get_trace(keys));
    }
    if (node->value_idx < 0) {
        size_++;
        node->value_idx = values_.size();
        values_.push_back(value);
        return true;
    }
    return false;
}

template<
    typename K,
    typename V,
    typename Allocator,
    typename InternalValueStorage>
template<typename F>
void
UBTree<K, V, Allocator, InternalValueStorage>::apply_to_subsets(
    const key_set& keys,
    F fun)
{
    auto replace_idx = [this, fun](const key_set& keys, int idx) {
        assert(idx >= 0);
        fun(keys, values_[idx]);
    };
    std::vector<int> subset;
    root_.apply_to_subsets(keys, replace_idx, 0, subset);
}

template<
    typename K,
    typename V,
    typename Allocator,
    typename InternalValueStorage>
template<typename F>
void
UBTree<K, V, Allocator, InternalValueStorage>::apply_to_supersets(
    const key_set& keys,
    F fun)
{
    auto replace_idx = [this, fun](int idx) {
        assert(idx >= 0);
        fun(values_[idx]);
    };
    root_.apply_to_supersets(keys, replace_idx, 0);
}

template<
    typename K,
    typename V,
    typename Allocator,
    typename InternalValueStorage>
V&
UBTree<K, V, Allocator, InternalValueStorage>::operator[](unsigned idx)
{
    return values_[idx];
}

template<
    typename K,
    typename V,
    typename Allocator,
    typename InternalValueStorage>
const V&
UBTree<K, V, Allocator, InternalValueStorage>::operator[](unsigned idx) const
{
    return values_[idx];
}

template<
    typename K,
    typename V,
    typename Allocator,
    typename InternalValueStorage>
bool
UBTree<K, V, Allocator, InternalValueStorage>::contains_disjoint_key(
    const key_set& keys) const
{
    if (keys.empty()) {
        return values_.size() > 0;
    }
    return root_.contains_disjoint_key(keys, 0);
}

template<
    typename K,
    typename V,
    typename Allocator,
    typename InternalValueStorage>
void
UBTree<K, V, Allocator, InternalValueStorage>::clear()
{
    size_ = 0;
    values_.clear();
    root_.successors.clear();
    root_.value_idx = -1;
}

} // namespace ub_tree
