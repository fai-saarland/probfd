#ifndef MDPS_SOLVERS_STATE_SPACE_INTERFACE_WRAPPERS_H
#define MDPS_SOLVERS_STATE_SPACE_INTERFACE_WRAPPERS_H

#include "probfd/bisimulation/bisimilar_state_space.h"

#include "probfd/engine_interfaces/open_list.h"
#include "probfd/engine_interfaces/transition_sampler.h"

#include "probfd/quotient_system/engine_interfaces.h"
#include "probfd/quotient_system/heuristic_search_interface.h"
#include "probfd/quotient_system/quotient_system.h"

#include "probfd/open_lists/lifo_open_list.h"

#include "probfd/transition_sampler/random_successor_sampler.h"

#include "utils/rng_options.h"

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
    std::shared_ptr<engine_interfaces::TransitionSampler<OperatorID>>> {

    using type = std::shared_ptr<engine_interfaces::TransitionSampler<
        quotient_system::QuotientAction<OperatorID>>>;

    type operator()(
        quotient_system::QuotientSystem<OperatorID>* q,
        std::shared_ptr<engine_interfaces::TransitionSampler<OperatorID>> t)
        const
    {
        return std::make_shared<engine_interfaces::TransitionSampler<
            quotient_system::QuotientAction<OperatorID>>>(q, t);
    }
};

template <>
struct Unwrapper<
    std::false_type,
    std::true_type,
    std::shared_ptr<engine_interfaces::TransitionSampler<
        quotient_system::QuotientAction<OperatorID>>>> {

    using type =
        std::shared_ptr<engine_interfaces::TransitionSampler<OperatorID>>;

    type operator()(std::shared_ptr<engine_interfaces::TransitionSampler<
                        quotient_system::QuotientAction<OperatorID>>> t) const
    {
        return t->real();
    }
};

template <typename Fret>
struct Wrapper<
    std::true_type,
    Fret,
    std::shared_ptr<engine_interfaces::TransitionSampler<OperatorID>>> {
    using type = std::shared_ptr<engine_interfaces::TransitionSampler<
        typename translate_action<Fret, bisimulation::QuotientAction>::type>>;

    type operator()(
        std::shared_ptr<engine_interfaces::TransitionSampler<OperatorID>>) const
    {
        // HACK to access the global rng...
        options::Options opts;
        opts.set<int>("random_seed", -1);
        auto rng = utils::parse_rng_from_options(opts);

        return std::make_shared<transition_sampler::RandomSuccessorSampler<
            typename translate_action<Fret, bisimulation::QuotientAction>::
                type>>(rng);
    }
};

template <typename Fret>
struct Unwrapper<
    std::true_type,
    Fret,
    std::shared_ptr<engine_interfaces::TransitionSampler<
        typename translate_action<Fret, bisimulation::QuotientAction>::type>>> {
    using type = engine_interfaces::TransitionSampler<OperatorID>*;
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
    std::shared_ptr<engine_interfaces::OpenList<OperatorID>>> {

    using type = std::shared_ptr<engine_interfaces::OpenList<
        quotient_system::QuotientAction<OperatorID>>>;

    type operator()(
        quotient_system::QuotientSystem<OperatorID>* q,
        std::shared_ptr<engine_interfaces::OpenList<OperatorID>> t) const
    {
        return std::make_shared<engine_interfaces::OpenList<
            quotient_system::QuotientAction<OperatorID>>>(q, t);
    }
};

template <>
struct Unwrapper<
    std::false_type,
    std::true_type,
    std::shared_ptr<engine_interfaces::OpenList<
        quotient_system::QuotientAction<OperatorID>>>> {

    using type = std::shared_ptr<engine_interfaces::OpenList<OperatorID>>;

    type operator()(std::shared_ptr<engine_interfaces::OpenList<
                        quotient_system::QuotientAction<OperatorID>>> t) const
    {
        return t->real();
    }
};

template <typename Fret>
struct Wrapper<
    std::true_type,
    Fret,
    std::shared_ptr<engine_interfaces::OpenList<OperatorID>>> {
    using type = std::shared_ptr<engine_interfaces::OpenList<
        typename translate_action<Fret, bisimulation::QuotientAction>::type>>;

    type
    operator()(std::shared_ptr<engine_interfaces::OpenList<OperatorID>>) const
    {
        return std::make_shared<open_lists::LifoOpenList<
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
    using type = engine_interfaces::OpenList<OperatorID>*;
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