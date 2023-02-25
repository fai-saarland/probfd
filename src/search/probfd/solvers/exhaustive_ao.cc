#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/engines/exhaustive_ao.h"

#include "probfd/open_lists/task_open_list_factory.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace solvers {
namespace {

using namespace engine_interfaces;

template <bool Bisimulation>
class ExhaustiveAOSolver : public MDPHeuristicSearch<Bisimulation, false> {
public:
    template <typename T>
    using WrappedType =
        typename MDPHeuristicSearch<Bisimulation, false>::template WrappedType<
            T>;

    explicit ExhaustiveAOSolver(const options::Options& opts)
        : MDPHeuristicSearch<Bisimulation, false>(opts)
        , open_list_(this->wrap(
              opts.get<std::shared_ptr<TaskOpenListFactory>>("open_list")
                  ->create_open_list(
                      &this->state_id_map_,
                      &this->action_id_map_)))
    {
    }

    virtual std::string get_heuristic_search_name() const override
    {
        return "exhaustive_ao";
    }

    virtual engines::MDPEngineInterface<State, OperatorID>*
    create_engine() override
    {
        return this->template create_heuristic_search_engine<
            engines::exhaustive_ao::ExhaustiveAOSearch>(open_list_.get());
    }

protected:
    WrappedType<std::shared_ptr<TaskOpenList>> open_list_;
};

struct ExhaustiveAOOptions {
    void operator()(options::OptionParser& parser) const
    {
        MDPHeuristicSearchBase::add_options_to_parser(parser);
        parser.add_option<std::shared_ptr<TaskOpenListFactory>>(
            "open_list",
            "",
            "lifo_open_list_factory");
    }
};

static Plugin<SolverInterface> _plugin(
    "exhaustive_ao",
    parse_mdp_heuristic_search_solver<ExhaustiveAOSolver, ExhaustiveAOOptions>);

} // namespace
} // namespace solvers
} // namespace probfd
