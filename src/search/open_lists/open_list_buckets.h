#ifndef OPEN_LISTS_OPEN_LIST_BUCKETS_H
#define OPEN_LISTS_OPEN_LIST_BUCKETS_H

#include "open_list.h"
#include "open_list_factory.h"
#include "../evaluator.h"
#include "../state_id.h"

#include <deque>
#include <utility>
#include <vector>
#include <memory>

namespace options {
class Options;
class OptionParser;
}

template<class Entry>
class BucketOpenList : public OpenList<Entry> {
    typedef std::deque<Entry> Bucket;
    std::vector<Bucket> buckets;
    mutable int lowest_bucket;
    int size;

    std::shared_ptr<Evaluator> evaluator;
    int last_evaluated_value;
    bool last_preferred;
    bool dead_end;
    bool dead_end_reliable;
protected:
    virtual Evaluator* get_evaluator() override {return evaluator.get(); }

public:
    BucketOpenList(const options::Options &opts);
    ~BucketOpenList();

    virtual int insert(const Entry &entry) override;
    virtual Entry remove_min(std::vector<int> *key = 0) override;
    virtual bool empty() const override;
    virtual void clear() override;

    virtual void evaluate(int g, bool preferred) override;
    virtual bool is_dead_end() const override;
    virtual bool dead_end_is_reliable() const override;
    virtual void get_involved_heuristics(std::set<Heuristic *> &hset) override;
};

class BucketOpenListFactory : public FallthroughOpenListFactory<BucketOpenList> {
public:
    using FallthroughOpenListFactory::FallthroughOpenListFactory;
};

#endif
