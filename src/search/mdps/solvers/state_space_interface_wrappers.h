#pragma once

#include "../bisimulation/bisimilar_state_space.h"
#include "../engine_interfaces/open_list.h"
#include "../engine_interfaces/transition_sampler.h"
#include "../probabilistic_operator.h"
#include "../quotient_system/quotient_system.h"
#include "../quotient_system/heuristic_search_interface.h"
#include "../quotient_system/engine_interfaces.h"
#include "../transition_sampler.h"
#include "../open_list.h"

#include <memory>

namespace probabilistic {

template<typename Bisimulation, typename Fret, typename T>
struct Wrapper {
    using type = T;
    type operator()(T t) const { return t; }
};

template<typename Bisimulation, typename Fret, typename T>
struct Unwrapper {
    using type = T;
    type operator()(T t) const { return t; }
};

template<typename Fret, typename Op>
struct translate_action;

template<typename Op>
struct translate_action<std::true_type, Op> {
    using type = quotient_system::QuotientAction<Op>;
};

template<typename Op>
struct translate_action<std::false_type, Op> {
    using type = Op;
};

/****************************************************************************/
/* TransitionSampler */

template<>
struct Wrapper<
    std::false_type,
    std::true_type,
    std::shared_ptr<TransitionSampler<const ProbabilisticOperator*>>> {

    using type = std::shared_ptr<TransitionSampler<
        quotient_system::QuotientAction<const ProbabilisticOperator*>>>;

    type operator()(
        quotient_system::QuotientSystem<const ProbabilisticOperator*>* q,
        std::shared_ptr<TransitionSampler<const ProbabilisticOperator*>> t)
        const
    {
        return std::make_shared<TransitionSampler<
            quotient_system::QuotientAction<const ProbabilisticOperator*>>>(
            q, t.get());
    }
};

template<>
struct Unwrapper<
    std::false_type,
    std::true_type,
    std::shared_ptr<TransitionSampler<
        quotient_system::QuotientAction<const ProbabilisticOperator*>>>> {

    using type = TransitionSampler<const ProbabilisticOperator*>*;

    type operator()(
        std::shared_ptr<TransitionSampler<
            quotient_system::QuotientAction<const ProbabilisticOperator*>>> t)
        const
    {
        return t->real();
    }
};

template<typename Fret>
struct Wrapper<
    std::true_type,
    Fret,
    std::shared_ptr<TransitionSampler<const ProbabilisticOperator*>>> {
    using type = std::shared_ptr<TransitionSampler<
        typename translate_action<Fret, bisimulation::QuotientAction>::type>>;

    type operator()(
        std::shared_ptr<TransitionSampler<const ProbabilisticOperator*>>)
        const
    {
        return std::make_shared<TransitionSampler<typename translate_action<
            Fret,
            bisimulation::QuotientAction>::type>>();
    }
};

template<typename Fret>
struct Unwrapper<
    std::true_type,
    Fret,
    std::shared_ptr<TransitionSampler<
        typename translate_action<Fret, bisimulation::QuotientAction>::type>>> {
    using type = TransitionSampler<const ProbabilisticOperator*>*;
    type operator()(std::shared_ptr<TransitionSampler<typename translate_action<
                        Fret,
                        bisimulation::QuotientAction>::type>>) const
    {
        return nullptr;
    }
};

/****************************************************************************/
/* OpenList */

template<>
struct Wrapper<
    std::false_type,
    std::true_type,
    std::shared_ptr<OpenList<const ProbabilisticOperator*>>> {

    using type = std::shared_ptr<OpenList<
        quotient_system::QuotientAction<const ProbabilisticOperator*>>>;

    type operator()(
        quotient_system::QuotientSystem<const ProbabilisticOperator*>* q,
        std::shared_ptr<OpenList<const ProbabilisticOperator*>> t) const
    {
        return std::make_shared<OpenList<
            quotient_system::QuotientAction<const ProbabilisticOperator*>>>(
            q, t.get());
    }
};

template<>
struct Unwrapper<
    std::false_type,
    std::true_type,
    std::shared_ptr<OpenList<
        quotient_system::QuotientAction<const ProbabilisticOperator*>>>> {

    using type = OpenList<const ProbabilisticOperator*>*;

    type operator()(std::shared_ptr<OpenList<quotient_system::QuotientAction<
                        const ProbabilisticOperator*>>> t) const
    {
        return t->real();
    }
};

template<typename Fret>
struct Wrapper<
    std::true_type,
    Fret,
    std::shared_ptr<OpenList<const ProbabilisticOperator*>>> {
    using type = std::shared_ptr<OpenList<
        typename translate_action<Fret, bisimulation::QuotientAction>::type>>;

    type
    operator()(std::shared_ptr<OpenList<const ProbabilisticOperator*>>) const
    {
        return std::make_shared<OpenList<typename translate_action<
            Fret,
            bisimulation::QuotientAction>::type>>();
    }
};

template<typename Fret>
struct Unwrapper<
    std::true_type,
    Fret,
    std::shared_ptr<OpenList<
        typename translate_action<Fret, bisimulation::QuotientAction>::type>>> {
    using type = OpenList<const ProbabilisticOperator*>*;
    type operator()(std::shared_ptr<OpenList<typename translate_action<
                        Fret,
                        bisimulation::QuotientAction>::type>>) const
    {
        return nullptr;
    }
};

} // namespace probabilistic
