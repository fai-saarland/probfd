#ifndef OPEN_LISTS_PARETO_OPEN_LIST_H
#define OPEN_LISTS_PARETO_OPEN_LIST_H

#include "open_list.h"
#include "open_list_factory.h"
#include "../evaluator.h"
#include "../utils/hash.h"
#include "../state_id.h"

#include <deque>
#include <set>
#include <vector>
#include <utility>
#include <memory>

namespace options {
class Options;
class OptionParser;
}

template<class Entry>
class ParetoOpenList : public OpenList<Entry> {
    typedef std::deque<Entry> Bucket;
    typedef std::vector<int> KeyType;
    //typedef std::map<const KeyType, Bucket> BucketMap;
    typedef typename utils::HashMap<const KeyType, Bucket> BucketMap;
    typedef std::set<KeyType> KeySet; // no hash_set (see insert method)

    BucketMap buckets;
    KeySet nondominated;
    bool state_uniform_selection;
    std::vector<std::shared_ptr<Evaluator> > evaluators;
    std::vector<int> last_evaluated_value;

    bool dominates(const KeyType &v1, const KeyType &v2);
    bool is_nondominated(const KeyType &vec,
                         KeySet &domination_candidates);
    void remove_key(const KeyType key);
    bool last_preferred;

    bool dead_end;
    bool dead_end_reliable;
protected:
    virtual Evaluator *get_evaluator() override {return this; }

public:
    ParetoOpenList(const std::vector<std::shared_ptr<Evaluator> > &evals,
                   bool preferred_only, bool state_uniform_selection_);
    ParetoOpenList(const options::Options &opts);
    ~ParetoOpenList();

    // open list interface
    virtual int insert(const Entry &entry) override;
    virtual Entry remove_min(std::vector<int> *key = 0) override;
    virtual bool empty() const override {return nondominated.empty(); }
    virtual void clear() override;

    // tuple evaluator interface
    virtual void evaluate(int g, bool preferred) override;
    virtual bool is_dead_end() const override;
    virtual bool dead_end_is_reliable() const override;
    virtual void get_involved_heuristics(std::set<Heuristic *> &hset) override;
};

class ParetoOpenListFactory : public FallthroughOpenListFactory<ParetoOpenList> {
public:
    using FallthroughOpenListFactory::FallthroughOpenListFactory;
    ParetoOpenListFactory(const std::vector<std::shared_ptr<Evaluator> > &evals,
                   bool preferred_only, bool state_uniform_selection_);

};

#endif
