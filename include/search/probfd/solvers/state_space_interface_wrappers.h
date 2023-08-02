#ifndef MDPS_SOLVERS_STATE_SPACE_INTERFACE_WRAPPERS_H
#define MDPS_SOLVERS_STATE_SPACE_INTERFACE_WRAPPERS_H

#include "probfd/bisimulation/bisimilar_state_space.h"

#include "probfd/engine_interfaces/open_list.h"
#include "probfd/engine_interfaces/successor_sampler.h"

#include "probfd/quotients/heuristic_search_interface.h"
#include "probfd/quotients/quotient_system.h"


#include "probfd/open_lists/lifo_open_list.h"

#include "probfd/successor_samplers/random_successor_sampler.h"

#include "downward/utils/rng_options.h"

#include "downward/plugins/options.h"

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
/* SuccessorSampler */

template <>
struct Wrapper<false, true, std::shared_ptr<TaskSuccessorSampler>> {
    using type = std::shared_ptr<engine_interfaces::SuccessorSampler<
        quotients::QuotientAction<OperatorID>>>;

    type operator()(
        quotients::QuotientSystem<State, OperatorID>* q,
        std::shared_ptr<TaskSuccessorSampler> t) const
    {
        return std::make_shared<
            quotients::RepresentativeSuccessorSampler<State>>(q, t);
    }
};

template <bool Fret>
struct Wrapper<true, Fret, std::shared_ptr<TaskSuccessorSampler>> {
    using type = std::shared_ptr<engine_interfaces::SuccessorSampler<
        typename translate_action<Fret, bisimulation::QuotientAction>::type>>;

    type operator()(std::shared_ptr<TaskSuccessorSampler>) const
    {
        // HACK to access the global rng...
        plugins::Options opts;
        opts.set<int>("random_seed", -1);
        auto rng = utils::parse_rng_from_options(opts);

        return std::make_shared<successor_samplers::RandomSuccessorSampler<
            typename translate_action<Fret, bisimulation::QuotientAction>::
                type>>(rng);
    }
};

/****************************************************************************/
/* OpenList */

template <>
struct Wrapper<false, true, std::shared_ptr<TaskOpenList>> {
    using type = std::shared_ptr<
        engine_interfaces::OpenList<quotients::QuotientAction<OperatorID>>>;

    type operator()(
        quotients::QuotientSystem<State, OperatorID>* q,
        std::shared_ptr<TaskOpenList> t) const
    {
        return std::make_shared<quotients::RepresentativeOpenList<State>>(q, t);
    }
};

template <bool Fret>
struct Wrapper<true, Fret, std::shared_ptr<TaskOpenList>> {
    using type = std::shared_ptr<engine_interfaces::OpenList<
        typename translate_action<Fret, bisimulation::QuotientAction>::type>>;

    type operator()(std::shared_ptr<TaskOpenList>) const
    {
        return std::make_shared<open_lists::LifoOpenList<
            typename translate_action<Fret, bisimulation::QuotientAction>::
                type>>();
    }
};

} // namespace solvers
} // namespace probfd

#endif // __STATE_SPACE_INTERFACE_WRAPPERS_H__