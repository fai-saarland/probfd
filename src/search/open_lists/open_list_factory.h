#ifndef OPEN_LIST_FACTORY_H
#define OPEN_LIST_FACTORY_H

#include "../global_operator.h"
#include "../option_parser.h"
#include "../state_id.h"
#include "open_list.h"

#include <utility>

class OpenListFactory {
public:
    OpenListFactory();
    virtual ~OpenListFactory();
    virtual std::shared_ptr<OpenList<StateID> > create_state_open_list() = 0;
    virtual std::shared_ptr<
        OpenList<std::pair<StateID, std::pair<int, const GlobalOperator*> > > >
    create_ehc_open_list() = 0;
    virtual std::shared_ptr<
        OpenList<std::pair<StateID, const GlobalOperator*> > >
    create_lazy_open_list() = 0;
};

template <template <typename> class OpenListType>
class FallthroughOpenListFactory : public OpenListFactory {
protected:
    options::Options opts;
    template <typename T>
    std::shared_ptr<OpenList<T> > create()
    {
        return std::make_shared<OpenListType<T> >(opts);
    }

public:
    FallthroughOpenListFactory(const options::Options& opts)
        : opts(opts)
    {
    }

    virtual std::shared_ptr<OpenList<StateID> >
    create_state_open_list() override
    {
        return create<StateID>();
    }

    virtual std::shared_ptr<
        OpenList<std::pair<StateID, std::pair<int, const GlobalOperator*> > > >
    create_ehc_open_list() override
    {
        return create<
            std::pair<StateID, std::pair<int, const GlobalOperator*> > >();
    }

    virtual std::shared_ptr<
        OpenList<std::pair<StateID, const GlobalOperator*> > >
    create_lazy_open_list() override
    {
        return create<std::pair<StateID, const GlobalOperator*> >();
    }
};

#endif
