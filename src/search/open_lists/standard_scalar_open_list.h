#ifndef OPEN_LISTS_STANDARD_SCALAR_OPEN_LIST_H
#define OPEN_LISTS_STANDARD_SCALAR_OPEN_LIST_H

#include "open_list.h"
#include "open_list_factory.h"
#include "../state_id.h"

#include <deque>
#include <map>
#include <vector>
#include <utility>
#include <memory>

namespace options {
class Options;
class OptionParser;
}

template<class Entry>
class StandardScalarOpenList : public OpenList<Entry> {
    typedef std::deque<Entry> Bucket;

    std::map<int, Bucket> buckets;
    int size;

    std::shared_ptr<Evaluator> evaluator;
    int last_evaluated_value;
    int last_preferred;
    bool dead_end;
    bool dead_end_reliable;
protected:
    virtual Evaluator *get_evaluator() override {return evaluator.get(); }

public:
    StandardScalarOpenList(const options::Options &opts);
    StandardScalarOpenList(std::shared_ptr<Evaluator> eval,
                           bool preferred_only);
    ~StandardScalarOpenList();

    virtual int insert(const Entry &entry) override;
    virtual Entry remove_min(std::vector<int> *key = 0) override;
    virtual bool empty() const override;
    virtual void clear() override;

    virtual void evaluate(int g, bool preferred) override;
    virtual bool is_dead_end() const override;
    virtual bool dead_end_is_reliable() const override;
    virtual void get_involved_heuristics(std::set<Heuristic *> &hset) override;
};

class StandardScalarOpenListFactory : public FallthroughOpenListFactory<StandardScalarOpenList> {
public:
    using FallthroughOpenListFactory::FallthroughOpenListFactory;
    StandardScalarOpenListFactory(std::shared_ptr<Evaluator> eval,
                           bool preferred_only);
};

#endif
