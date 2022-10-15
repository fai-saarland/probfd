#ifndef MDPS_OPEN_LISTS_KEY_BASED_OPEN_LIST_H
#define MDPS_OPEN_LISTS_KEY_BASED_OPEN_LIST_H

#include "probfd/open_list.h"

#include <deque>
#include <map>
#include <vector>

namespace probfd {
namespace open_lists {

struct LifoPop {
    template <typename C>
    StateID operator()(C& c) const
    {
        auto it = c.rbegin();
        const StateID res = *it;
        c.erase(--(it.base()));
        return res;
    }
};

struct FifoPop {
    template <typename C>
    StateID operator()(C& c) const
    {
        auto it = c.begin();
        const StateID res = *it;
        c.erase(it);
        return res;
    }
};

template <
    typename Key,
    typename Pop = LifoPop,
    typename Comparison = typename std::less<Key>>
class KeyBasedOpenList : public GlobalStateOpenList {
private:
    using Bucket = std::vector<StateID>;
    // using Buckets = std::vector<Bucket>;
    using BucketMap = std::map<Key, Bucket, Comparison>;
    using BucketRef = typename BucketMap::iterator;

public:
    explicit KeyBasedOpenList(const Comparison& comparison = Comparison())
        : size_(0)
        , bucket_map_(comparison)
    {
    }

    virtual ~KeyBasedOpenList() = default;

    virtual StateID pop() override
    {
        BucketRef b = bucket_map_.begin();
        const StateID res = bucket_pop(b);
        if (bucket_empty(b)) {
            remove(b);
        }
        --size_;
        return res;
    }

    virtual unsigned size() const override { return size_; }

    virtual void clear() override
    {
        size_ = 0;
        bucket_map_.clear();
    }

    virtual void push(const StateID& state_id) override
    {
        Key k = get_key(state_id);
        push(k, state_id);
    }

    virtual void push(
        const StateID& parent,
        const ProbabilisticOperator* op,
        const value_type::value_t& prob,
        const StateID& state_id) override
    {
        Key k = get_key(parent, op, prob, state_id);
        push(k, state_id);
    }

protected:
    virtual Key get_key(const StateID& state_id) = 0;

    virtual Key get_key(
        const StateID&,
        const ProbabilisticOperator*,
        const value_type::value_t&,
        const StateID& state_id)
    {
        return this->get_key(state_id);
    }

private:
    void push(const Key& key, const StateID& stateid)
    {
        BucketRef b = bucket(key);
        bucket_push(b, stateid);
        ++size_;
    }

    inline BucketRef bucket(const Key& key)
    {
        return bucket_map_.emplace(key, Bucket()).first;
    }

    inline void remove(BucketRef ref) { bucket_map_.erase(ref); }

    inline void bucket_push(BucketRef ref, const StateID& stateid)
    {
        ref->second.push_back(stateid);
    }

    inline StateID bucket_pop(BucketRef ref) { return Pop()(ref->second); }

    inline bool bucket_empty(BucketRef ref) { return ref->second.empty(); }

private:
    unsigned size_;
    BucketMap bucket_map_;
};

} // namespace open_lists
} // namespace probfd

#endif // __KEY_BASED_OPEN_LIST_H__