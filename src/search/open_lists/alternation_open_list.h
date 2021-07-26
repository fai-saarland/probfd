#ifndef OPEN_LISTS_ALTERNATION_OPEN_LIST_H
#define OPEN_LISTS_ALTERNATION_OPEN_LIST_H

#include "open_list.h"
#include "open_list_factory.h"
#include "../evaluator.h"

#include <vector>
#include <memory>

namespace options {
class Options;
class OptionParser;
}

template<class Entry>
class AlternationOpenList : public OpenList<Entry> {
    std::vector<std::shared_ptr<OpenList<Entry> > > open_lists;
    std::vector<int> priorities;

    int size;
    bool dead_end;
    bool dead_end_reliable;
    // roughly speaking, boosting is how often the boosted queue should be
    // preferred when removing an entry
    int boosting;
    int last_used_list;

protected:
    virtual Evaluator *get_evaluator() override {return this; }

public:
    AlternationOpenList(const options::Options &opts);
    AlternationOpenList(const std::vector<std::shared_ptr<OpenList<Entry> > > &sublists,
                        int boost_influence);
    ~AlternationOpenList();

    // OpenList interface
    virtual int insert(const Entry &entry) override;
    virtual Entry remove_min(std::vector<int> *key = 0) override;
    virtual bool empty() const override;
    virtual void clear() override;

    // Evaluator interface
    virtual void evaluate(int g, bool preferred) override;
    virtual bool is_dead_end() const override;
    virtual bool dead_end_is_reliable() const override;
    virtual void get_involved_heuristics(std::set<Heuristic *> &hset) override;

    int boost_preferred() override;
    void boost_last_used_list() override;
};

class AlternationOpenListFactory : public OpenListFactory {
    options::Options opts;
public:
    AlternationOpenListFactory(const options::Options& opts);
    AlternationOpenListFactory(const std::vector<std::shared_ptr<OpenListFactory > > &sublists,
                        int boost_influence);
    virtual std::shared_ptr<OpenList<StateID> > create_state_open_list() override;
    virtual std::shared_ptr<
        OpenList<std::pair<StateID, std::pair<int, const GlobalOperator*> > > >
    create_ehc_open_list() override;
    virtual std::shared_ptr<
        OpenList<std::pair<StateID, const GlobalOperator*> > >
    create_lazy_open_list() override;
};

#endif
