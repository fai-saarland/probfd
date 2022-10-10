#ifndef MDPS_SOLVERS_STATE_SPACE_INTERFACE_WRAPPERS_H
#define MDPS_SOLVERS_STATE_SPACE_INTERFACE_WRAPPERS_H

#include "../bisimulation/bisimilar_state_space.h"
#include "../engine_interfaces/open_list.h"
#include "../engine_interfaces/transition_sampler.h"
#include "../open_list.h"
#include "../probabilistic_operator.h"
#include "../quotient_system/engine_interfaces.h"
#include "../quotient_system/heuristic_search_interface.h"
#include "../quotient_system/quotient_system.h"
#include "../transition_sampler.h"

#include <memory>

namespace probfd {
namespace solvers {

template <typename Bisimulation, typename Fret, typename T>
struct Wrapper {
    using type = T;
    type operator()(T t) const { return t; }
};

template <typename Bisimulation, typename Fret, typename T>
struct Unwrapper {
    using type = T;
    type operator()(T t) const { return t; }
};

template <typename Fret, typename Op>
struct translate_action;

template <typename Op>
struct translate_action<std::true_type, Op> {
    using type = quotient_system::QuotientAction<Op>;
};

template <typename Op>
struct translate_action<std::false_type, Op> {
    using type = Op;
};

/****************************************************************************/
/* TransitionSampler */

template <>
struct Wrapper<
    std::false_type,
    std::true_type,
    std::shared_ptr<
        engine_interfaces::TransitionSampler<const ProbabilisticOperator*>>> {

    using type = std::shared_ptr<engine_interfaces::TransitionSampler<
        quotient_system::QuotientAction<const ProbabilisticOperator*>>>;

    type operator()(
        quotient_system::QuotientSystem<const ProbabilisticOperator*>* q,
        std::shared_ptr<
            engine_interfaces::TransitionSampler<const ProbabilisticOperator*>>
            t) const
    {
        return std::make_shared<engine_interfaces::TransitionSampler<
            quotient_system::QuotientAction<const ProbabilisticOperator*>>>(
            q,
            t.get());
    }
};

template <>
struct Unwrapper<
    std::false_type,
    std::true_type,
    std::shared_ptr<engine_interfaces::TransitionSampler<
        quotient_system::QuotientAction<const ProbabilisticOperator*>>>> {

    using type =
        engine_interfaces::TransitionSampler<const ProbabilisticOperator*>*;

    type operator()(
        std::shared_ptr<engine_interfaces::TransitionSampler<
            quotient_system::QuotientAction<const ProbabilisticOperator*>>> t)
        const
    {
        return t->real();
    }
};

template <typename Fret>
struct Wrapper<
    std::true_type,
    Fret,
    std::shared_ptr<
        engine_interfaces::TransitionSampler<const ProbabilisticOperator*>>> {
    using type = std::shared_ptr<engine_interfaces::TransitionSampler<
        typename translate_action<Fret, bisimulation::QuotientAction>::type>>;

    type operator()(std::shared_ptr<engine_interfaces::TransitionSampler<
                        const ProbabilisticOperator*>>) const
    {
        return std::make_shared<engine_interfaces::TransitionSampler<
            typename translate_action<Fret, bisimulation::QuotientAction>::
                type>>();
    }
};

template <typename Fret>
struct Unwrapper<
    std::true_type,
    Fret,
    std::shared_ptr<engine_interfaces::TransitionSampler<
        typename translate_action<Fret, bisimulation::QuotientAction>::type>>> {
    using type =
        engine_interfaces::TransitionSampler<const ProbabilisticOperator*>*;
    type operator()(
        std::shared_ptr<engine_interfaces::TransitionSampler<
            typename translate_action<Fret, bisimulation::QuotientAction>::
                type>>) const
    {
        return nullptr;
    }
};

/****************************************************************************/
/* OpenList */

template <>
struct Wrapper<
    std::false_type,
    std::true_type,
    std::shared_ptr<
        engine_interfaces::OpenList<const ProbabilisticOperator*>>> {

    using type = std::shared_ptr<engine_interfaces::OpenList<
        quotient_system::QuotientAction<const ProbabilisticOperator*>>>;

    type operator()(
        quotient_system::QuotientSystem<const ProbabilisticOperator*>* q,
        std::shared_ptr<
            engine_interfaces::OpenList<const ProbabilisticOperator*>> t) const
    {
        return std::make_shared<engine_interfaces::OpenList<
            quotient_system::QuotientAction<const ProbabilisticOperator*>>>(
            q,
            t.get());
    }
};

template <>
struct Unwrapper<
    std::false_type,
    std::true_type,
    std::shared_ptr<engine_interfaces::OpenList<
        quotient_system::QuotientAction<const ProbabilisticOperator*>>>> {

    using type = engine_interfaces::OpenList<const ProbabilisticOperator*>*;

    type operator()(
        std::shared_ptr<engine_interfaces::OpenList<
            quotient_system::QuotientAction<const ProbabilisticOperator*>>> t)
        const
    {
        return t->real();
    }
};

template <typename Fret>
struct Wrapper<
    std::true_type,
    Fret,
    std::shared_ptr<
        engine_interfaces::OpenList<const ProbabilisticOperator*>>> {
    using type = std::shared_ptr<engine_interfaces::OpenList<
        typename translate_action<Fret, bisimulation::QuotientAction>::type>>;

    type
    operator()(std::shared_ptr<
               engine_interfaces::OpenList<const ProbabilisticOperator*>>) const
    {
        return std::make_shared<engine_interfaces::OpenList<
            typename translate_action<Fret, bisimulation::QuotientAction>::
                type>>();
    }
};

template <typename Fret>
struct Unwrapper<
    std::true_type,
    Fret,
    std::shared_ptr<engine_interfaces::OpenList<
        typename translate_action<Fret, bisimulation::QuotientAction>::type>>> {
    using type = engine_interfaces::OpenList<const ProbabilisticOperator*>*;
    type operator()(
        std::shared_ptr<engine_interfaces::OpenList<typename translate_action<
            Fret,
            bisimulation::QuotientAction>::type>>) const
    {
        return nullptr;
    }
};

} // namespace solvers
} // namespace probfd

#endif // __STATE_SPACE_INTERFACE_WRAPPERS_H__