#ifndef OPEN_LISTS_TIEBREAKING_OPEN_LIST_H
#define OPEN_LISTS_TIEBREAKING_OPEN_LIST_H

#include "open_list.h"
#include "open_list_factory.h"
#include "../state_id.h"
#include "../evaluator.h"

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
class TieBreakingOpenList : public OpenList<Entry> {
    typedef std::deque<Entry> Bucket;

    std::map<const std::vector<int>, Bucket> buckets;
    int size;

    std::vector<std::shared_ptr<Evaluator> > evaluators;
    std::vector<int> last_evaluated_value;
    bool last_preferred;
    bool dead_end;
    bool first_is_dead_end;
    bool dead_end_reliable;
    bool allow_unsafe_pruning; // don't insert if main evaluator
    // says dead end, even if not reliably

    const std::vector<int> &get_value(); // currently not used
    int dimension() const;
protected:
    Evaluator *get_evaluator() {return this; }

public:
    TieBreakingOpenList(const options::Options &opts);
    TieBreakingOpenList(const std::vector<std::shared_ptr<Evaluator> > &evals,
                        bool preferred_only, bool unsafe_pruning);
    ~TieBreakingOpenList();

    // open list interface
    virtual int insert(const Entry &entry) override;
    virtual Entry remove_min(std::vector<int> *key = 0) override;
    virtual bool empty() const override;
    virtual void clear() override;

    // tuple evaluator interface
    virtual void evaluate(int g, bool preferred) override;
    virtual bool is_dead_end() const override;
    virtual bool dead_end_is_reliable() const override;
    virtual void get_involved_heuristics(std::set<Heuristic *> &hset);
};

class TieBreakingOpenListFactory : public FallthroughOpenListFactory<TieBreakingOpenList> {
public:
    using FallthroughOpenListFactory::FallthroughOpenListFactory;
    TieBreakingOpenListFactory(const std::vector<std::shared_ptr<Evaluator> > &evals,
                    bool preferred_only, bool unsafe_pruning);
};

#endif
