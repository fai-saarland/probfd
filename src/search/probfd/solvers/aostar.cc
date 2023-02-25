#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/engines/ao_star.h"

#include "probfd/engine_interfaces/transition_sampler.h"
#include "probfd/transition_samplers/task_transition_sampler_factory.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace solvers {
namespace {

using namespace engine_interfaces;

template <bool Bisimulation>
class AOStarSolver : public MDPHeuristicSearch<Bisimulation, false> {
    template <typename T>
    using WrappedType =
        typename MDPHeuristicSearch<Bisimulation, false>::template WrappedType<
            T>;

    WrappedType<std::shared_ptr<TaskTransitionSampler>> successor_sampler_;

public:
    explicit AOStarSolver(const options::Options& opts)
        : MDPHeuristicSearch<Bisimulation, false>(opts)
        , successor_sampler_(this->template wrap<>(
              opts.get<std::shared_ptr<TaskTransitionSamplerFactory>>(
                      "successor_sampler")
                  ->create_sampler(&this->state_space_)))
    {
    }

    virtual std::string get_heuristic_search_name() const override
    {
        return "aostar";
    }

    virtual engines::MDPEngineInterface<State, OperatorID>*
    create_engine() override
    {
        return this->template create_heuristic_search_engine<
            engines::ao_search::ao_star::AOStar>(successor_sampler_.get());
    }

protected:
    virtual void print_additional_statistics() const override
    {
        successor_sampler_->print_statistics(std::cout);
        MDPHeuristicSearch<Bisimulation, false>::print_additional_statistics();
    }
};

struct AOStarOptions {
    void operator()(options::OptionParser& parser) const
    {
        parser.add_option<std::shared_ptr<TaskTransitionSamplerFactory>>(
            "successor_sampler",
            "",
            "arbitrary_successor_selector_factory");
    }
};

static Plugin<SolverInterface> _plugin(
    "aostar",
    parse_mdp_heuristic_search_solver<AOStarSolver, AOStarOptions>);

} // namespace
} // namespace solvers
} // namespace probfd
