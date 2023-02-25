#include "probfd/engine_interfaces/open_list.h"
#include "probfd/engines/trap_aware_dfhs.h"
#include "probfd/solvers/mdp_heuristic_search.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace solvers {
namespace {

using namespace engine_interfaces;
using namespace engines::trap_aware_dfhs;

class TrapAwareDFHSSolver : public MDPHeuristicSearch<false, true> {
    template <typename T>
    using WrappedType =
        typename MDPHeuristicSearch<false, true>::WrappedType<T>;

    template <typename State, typename Action, bool Interval>
    using Engine = TADepthFirstHeuristicSearch<State, Action, Interval>;

    WrappedType<std::shared_ptr<TaskOpenList>> open_list_;
    const bool forward_updates_;
    const BacktrackingUpdateType backward_updates_;
    const bool cutoff_tip_;
    const bool cutoff_inconsistent_;
    const bool terminate_exploration_;
    const bool value_iteration_;
    const bool reexpand_traps_;

public:
    explicit TrapAwareDFHSSolver(const options::Options& opts)
        : MDPHeuristicSearch<false, true>(opts)
        , open_list_(
              opts.contains("open_list")
                  ? this->wrap(
                        opts.get<std::shared_ptr<TaskOpenList>>("open_list"))
                  : nullptr)
        , forward_updates_(opts.get<bool>("fwup"))
        , backward_updates_(opts.get<BacktrackingUpdateType>("bwup"))
        , cutoff_tip_(opts.get<bool>("cutoff_tip"))
        , cutoff_inconsistent_(opts.get<bool>("cutoff_inconsistent"))
        , terminate_exploration_(opts.get<bool>("terminate_exploration"))
        , value_iteration_(opts.get<bool>("vi"))
        , reexpand_traps_(opts.get<bool>("reexpand_traps"))
    {
    }

    static void add_options_to_parser(options::OptionParser& parser)
    {
        MDPHeuristicSearchBase::add_options_to_parser(parser);
        parser.document_synopsis(
            "Trap-aware depth-first heuristic search family. Supports all MDPs "
            "(even non-SSPs) without FRET loop.",
            "");
        parser.add_option<bool>(
            "fwup",
            "Value updates on the way down of exploration.",
            "true");
        {
            std::vector<std::string> bwups(
                {"disabled", "ondemand", "single", "convergence"});
            parser.add_enum_option<BacktrackingUpdateType>(
                "bwup",
                bwups,
                "Value updates on the way back of exploration: disabled, "
                "ondemand (if some value changed during forward updates), "
                "single (single update), convergence (run VI)",
                "ondemand");
        }
        parser.add_option<bool>(
            "cutoff_tip",
            "Do not follow tip states during policy exploration.",
            "true");
        parser.add_option<bool>(
            "cutoff_inconsistent",
            "Do not expand states whose values have changed during the forward "
            "updates.",
            "true");
        parser.add_option<bool>(
            "terminate_exploration",
            "Stop the exploration of the policy as soon as a state expansion "
            "was cutoff.",
            "false");
        parser.add_option<bool>(
            "vi",
            "Run VI to check for termination. If set to false falling back to "
            "solved-labeling mechanism.",
            "false");

        parser.add_option<bool>(
            "reexpand_traps",
            "Immediately re-expand the collapsed trap state.",
            "true");
        parser.add_option<std::shared_ptr<TaskOpenList>>(
            "open_list",
            "Ordering in which successors are considered during policy "
            "exploration.",
            options::OptionParser::NONE);
    }

    virtual std::string get_engine_name() const override { return "tadfhs"; }

    virtual std::string get_heuristic_search_name() const override
    {
        return "";
    }

    virtual engines::MDPEngineInterface<State, OperatorID>*
    create_engine() override
    {
        return this->template create_quotient_heuristic_search_engine<Engine>(
            forward_updates_,
            backward_updates_,
            !cutoff_tip_,
            cutoff_inconsistent_,
            terminate_exploration_,
            value_iteration_,
            !value_iteration_,
            reexpand_traps_,
            open_list_.get());
    }

protected:
    virtual void print_additional_statistics() const override
    {
        MDPHeuristicSearch<false, true>::print_additional_statistics();
    }
};

struct ILAOOptions {
    void operator()(options::Options& opts) const
    {
        opts.set<std::string>("name", "ilao");
        opts.set<bool>("labeling", false);
        opts.set<bool>("fwup", false);
        opts.set<BacktrackingUpdateType>(
            "bwup",
            BacktrackingUpdateType::Single);
        opts.set<bool>("cutoff_inconsistent", false);
        opts.set<bool>("partial_exploration", false);
        opts.set<bool>("expand_tip", false);
        opts.set<bool>("vi", true);
    }
};

struct LILAOOptions {
    void operator()(options::Options& opts) const
    {
        opts.set<std::string>("name", "lilao");
        opts.set<bool>("labeling", true);
        opts.set<bool>("fwup", false);
        opts.set<BacktrackingUpdateType>(
            "bwup",
            BacktrackingUpdateType::Single);
        opts.set<bool>("cutoff_inconsistent", false);
        opts.set<bool>("partial_exploration", false);
        opts.set<bool>("expand_tip", false);
        opts.set<bool>("vi", false);
    }
};

struct HDPOptions {
    void operator()(options::Options& opts) const
    {
        opts.set<std::string>("name", "hdp");
        opts.set<bool>("labeling", true);
        opts.set<bool>("fwup", true);
        opts.set<BacktrackingUpdateType>(
            "bwup",
            BacktrackingUpdateType::OnDemand);
        opts.set<bool>("cutoff_inconsistent", true);
        opts.set<bool>("partial_exploration", false);
        opts.set<bool>("vi", false);
        opts.set<bool>("expand_tip", true);
    }
};

template <typename OptionsPostprocessing>
static std::shared_ptr<SolverInterface>
parse_with_default(options::OptionParser& parser)
{
    TrapAwareDFHSSolver::add_options_to_parser(parser);
    options::Options opts = parser.parse();
    OptionsPostprocessing()(opts);

    if (!parser.dry_run()) {
        return std::make_shared<TrapAwareDFHSSolver>(opts);
    }

    return nullptr;
}

static Plugin<SolverInterface>
    _plugin("tadfhs", options::parse<SolverInterface, TrapAwareDFHSSolver>);

static Plugin<SolverInterface>
    _plugin2("tailao", parse_with_default<ILAOOptions>);

static Plugin<SolverInterface>
    _plugin3("talilao", parse_with_default<LILAOOptions>);

static Plugin<SolverInterface>
    _plugin4("tahdp", parse_with_default<HDPOptions>);

}
} // namespace solvers
} // namespace probfd
