#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/engines/ao_star.h"

#include "probfd/utils/logging.h"

#include "probfd/engine_interfaces/transition_sampler.h"
#include "probfd/transition_sampler/task_transition_sampler_factory.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace solvers {

using namespace engine_interfaces;

template <typename Bisimulation>
class AOStarSolver : public MDPHeuristicSearch<Bisimulation, std::false_type> {
public:
    template <typename T>
    using WrappedType =
        typename MDPHeuristicSearch<Bisimulation, std::false_type>::
            template WrappedType<T>;

    explicit AOStarSolver(const options::Options& opts)
        : MDPHeuristicSearch<Bisimulation, std::false_type>(opts)
        , successor_sampler_(this->template wrap<>(
              opts.get<std::shared_ptr<TaskTransitionSamplerFactory>>(
                      "successor_sampler")
                  ->create_sampler(
                      this->get_state_id_map(),
                      this->get_action_id_map())))
    {
    }

    virtual std::string get_heuristic_search_name() const override
    {
        return "aostar";
    }

    virtual engines::MDPEngineInterface<State>* create_engine() override
    {
        return this->template heuristic_search_engine_factory<
            engines::ao_star::AOStar>(successor_sampler_.get());
    }

protected:
    virtual void print_additional_statistics() const override
    {
        auto sampler = this->template unwrap<>(successor_sampler_);
        if (sampler != nullptr) {
            sampler->print_statistics(logging::out);
        }
        MDPHeuristicSearch<Bisimulation, std::false_type>::
            print_additional_statistics();
    }

    WrappedType<std::shared_ptr<TaskTransitionSampler>> successor_sampler_;
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

} // namespace solvers
} // namespace probfd
