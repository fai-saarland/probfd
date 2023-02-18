#ifndef MDPS_SOLVERS_STATE_SPACE_INTERFACE_WRAPPERS_H
#define MDPS_SOLVERS_STATE_SPACE_INTERFACE_WRAPPERS_H

#include "probfd/bisimulation/bisimilar_state_space.h"

#include "probfd/engine_interfaces/open_list.h"
#include "probfd/engine_interfaces/transition_sampler.h"

#include "probfd/quotients/engine_interfaces.h"
#include "probfd/quotients/heuristic_search_interface.h"
#include "probfd/quotients/quotient_system.h"

#include "probfd/open_lists/lifo_open_list.h"

#include "probfd/transition_samplers/random_successor_sampler.h"

#include "utils/rng_options.h"

#include <memory>

namespace probfd {
namespace solvers {

template <bool Bisimulation, bool Fret, typename T>
struct Wrapper {
    using type = T;
    type operator()(T t) const { return t; }
};

template <bool Fret, typename Op>
struct translate_action;

template <typename Op>
struct translate_action<true, Op> {
    using type = quotients::QuotientAction<Op>;
};

template <typename Op>
struct translate_action<false, Op> {
    using type = Op;
};

/****************************************************************************/
/* TransitionSampler */

template <>
struct Wrapper<
    false,
    true,
    std::shared_ptr<engine_interfaces::TransitionSampler<OperatorID>>> {

    using type = std::shared_ptr<engine_interfaces::TransitionSampler<
        quotients::QuotientAction<OperatorID>>>;

    type operator()(
        quotients::QuotientSystem<OperatorID>* q,
        std::shared_ptr<engine_interfaces::TransitionSampler<OperatorID>> t)
        const
    {
        return std::make_shared<quotients::RepresentativeTransitionSampler>(
            q,
            t);
    }
};

template <bool Fret>
struct Wrapper<
    true,
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

        return std::make_shared<transition_samplers::RandomSuccessorSampler<
            typename translate_action<Fret, bisimulation::QuotientAction>::
                type>>(rng);
    }
};

/****************************************************************************/
/* OpenList */

template <>
struct Wrapper<
    false,
    true,
    std::shared_ptr<engine_interfaces::OpenList<OperatorID>>> {

    using type = std::shared_ptr<
        engine_interfaces::OpenList<quotients::QuotientAction<OperatorID>>>;

    type operator()(
        quotients::QuotientSystem<OperatorID>* q,
        std::shared_ptr<engine_interfaces::OpenList<OperatorID>> t) const
    {
        return std::make_shared<quotients::RepresentativeOpenList>(q, t);
    }
};

template <bool Fret>
struct Wrapper<
    true,
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

} // namespace solvers
} // namespace probfd

#endif // __STATE_SPACE_INTERFACE_WRAPPERS_H__