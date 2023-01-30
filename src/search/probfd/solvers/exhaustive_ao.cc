#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/engines/exhaustive_ao.h"

#include "probfd/utils/logging.h"

#include "probfd/open_list.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace solvers {

using namespace engine_interfaces;

template <typename Bisimulation>
class ExhaustiveAOSolver
    : public MDPHeuristicSearch<Bisimulation, std::false_type> {
public:
    template <typename T>
    using WrappedType =
        typename MDPHeuristicSearch<Bisimulation, std::false_type>::
            template WrappedType<T>;

    explicit ExhaustiveAOSolver(const options::Options& opts)
        : MDPHeuristicSearch<Bisimulation, std::false_type>(opts)
        , open_list_(this->wrap(
              opts.get<std::shared_ptr<GlobalStateOpenList>>("open_list")))
    {
    }

    virtual std::string get_heuristic_search_name() const override
    {
        return "exhaustive_ao";
    }

    virtual engines::MDPEngineInterface<State>* create_engine() override
    {
        return this->template heuristic_search_engine_factory<
            engines::exhaustive_ao::ExhaustiveAOSearch>(open_list_.get());
    }

protected:
    WrappedType<std::shared_ptr<GlobalStateOpenList>> open_list_;
};

struct ExhaustiveAOOptions {
    void operator()(options::OptionParser& parser) const
    {
        MDPHeuristicSearchBase::add_options_to_parser(parser);
        parser.add_option<std::shared_ptr<GlobalStateOpenList>>(
            "open_list",
            "",
            "lifo_open_list");
    }
};

static Plugin<SolverInterface> _plugin(
    "exhaustive_ao",
    parse_mdp_heuristic_search_solver<ExhaustiveAOSolver, ExhaustiveAOOptions>);

} // namespace solvers
} // namespace probfd
