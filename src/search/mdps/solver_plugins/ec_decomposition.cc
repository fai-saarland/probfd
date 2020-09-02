#include "../../option_parser.h"
#include "../../plugin.h"
#include "../../utils/system.h"
#include "../../utils/timer.h"
#include "../algorithms/end_component_decomposition.h"
#include "../algorithms/types_storage.h"
#include "../logging.h"
#include "mdp_solver.h"

#include <cassert>
#include <sstream>

namespace probabilistic {
namespace solvers {

template<typename T>
inline bool is_state_contained(T& t, const StateID& id);

template<>
inline bool
is_state_contained(
    algorithms::storage::StateIDHashSet& set,
    const StateID& state_id)
{
    return set.count(state_id);
}

template<>
bool
is_state_contained(
    algorithms::storage::PerStateStorage<bool>& set,
    const StateID& state_id)
{
    return set[state_id];
}

template<typename Store>
class GlobalStateInSetEvaluator : public GlobalStateEvaluator {
public:
    template<typename... Args>
    GlobalStateInSetEvaluator(
        value_type::value_t in,
        GlobalStateEvaluator* eval,
        Args... args)
        : store_(args...)
        , in_(in)
        , eval_(eval)
    {
    }

    Store store_;

protected:
    virtual EvaluationResult evaluate(const GlobalState& state)
    {
        EvaluationResult res = eval_->operator()(state);
        if (res) {
            return res;
        }
        if (is_state_contained(store_, state.get_id())) {
            return EvaluationResult(true, in_);
        } else {
            return res;
        }
    }

    const value_type::value_t in_;
    GlobalStateEvaluator* eval_;
};

class ECDecomposition : public MDPSolver {
public:
    ECDecomposition(const options::Options& opts)
        : MDPSolver(opts)
        , inner_(std::dynamic_pointer_cast<MDPSolver>(
              opts.get<std::shared_ptr<SolverInterface>>("inner")))
        , prune_(
              opts.contains("prune")
                  ? opts.get<std::shared_ptr<GlobalStateEvaluator>>("prune")
                  : nullptr)
        , zero_(opts.get<bool>("zero"))
        , one_(opts.contains("one_reward"))
        , one_reward_(one_ ? opts.get<double>("one_reward") : 0)
    {
        if (inner_ == nullptr) {
            g_err << "ECDecomposition: Invalid inner MDP solver" << std::endl;
            utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
        }
    }

    static void add_options_to_parser(options::OptionParser& parser)
    {
        parser.add_option<std::shared_ptr<SolverInterface>>("inner");
        parser.add_option<std::shared_ptr<GlobalStateEvaluator>>(
            "prune", "", options::OptionParser::NONE);
        parser.add_option<bool>("zero", "", "false");
        parser.add_option<double>(
            "one_reward", "", options::OptionParser::NONE);
        MDPSolver::add_options_to_parser(parser);
    }

    virtual std::string name() const override
    {
        std::ostringstream out;
        out << "ec_decomposition"
            << "(" << inner_->name() << ")";
        return out.str();
    }

    virtual IMDPEngine<State>* create_engine(
        ProgressReport* report,
        algorithms::StateIDMap<State>* state_id_map,
        algorithms::ApplicableActionsGenerator<State, Action>* aops_generator,
        algorithms::TransitionGenerator<State, Action>* transition_generator,
        value_type::value_t minval,
        value_type::value_t maxval,
        algorithms::StateEvaluationFunction<State>* state_reward,
        algorithms::TransitionRewardFunction<State, Action>* transition_reward)
        override
    {
        return new ECDecompositionEngine(
            inner_,
            prune_,
            report,
            state_id_map,
            aops_generator,
            transition_generator,
            minval,
            maxval,
            state_reward,
            transition_reward,
            zero_,
            one_,
            one_reward_);
    }

    virtual bool maintains_dual_bounds() const override
    {
        return inner_->maintains_dual_bounds();
    }

private:
    std::shared_ptr<MDPSolver> inner_;
    std::shared_ptr<GlobalStateEvaluator> prune_;
    const bool zero_;
    const bool one_;
    const value_type::value_t one_reward_;

    class ECDecompositionEngine : public IMDPEngine<State> {
    public:
        ECDecompositionEngine(
            std::shared_ptr<MDPSolver> inner,
            std::shared_ptr<GlobalStateEvaluator> prune,
            ProgressReport* report,
            algorithms::StateIDMap<State>* state_id_map,
            algorithms::ApplicableActionsGenerator<State, Action>* aops_gen,
            algorithms::TransitionGenerator<State, Action>* transition_gen,
            value_type::value_t minval,
            value_type::value_t maxval,
            algorithms::StateEvaluationFunction<State>* state_reward,
            algorithms::TransitionRewardFunction<State, Action>*
                transition_reward,
            bool zero,
            bool one,
            value_type::value_t one_reward)
            : inner_(inner)
            , prune_(prune)
            , report_(report)
            , state_id_map_(state_id_map)
            , aops_gen_(aops_gen)
            , transition_gen_(transition_gen)
            , minval_(minval)
            , maxval_(maxval)
            , state_reward_(state_reward)
            , transition_reward_(transition_reward)
            , zero_(zero)
            , one_(one)
            , one_reward_(one_reward)
        {
        }

        virtual AnalysisResult solve(const State& state) override
        {
            g_log << "running end component decomposition ..." << std::endl;
            algorithms::end_component_decomposition::EndComponentDecomposition<
                GlobalState,
                const ProbabilisticOperator*>
                ec(state_id_map_,
                   prune_.get(),
                   state_reward_,
                   aops_gen_,
                   transition_gen_);

            IPrintable* stats = nullptr;
            algorithms::quotient_system::
                QuotientSystem<GlobalState, const ProbabilisticOperator*>* qs =
                    nullptr;

            using BoolStore = algorithms::storage::PerStateStorage<bool>;
            using MyEval = GlobalStateInSetEvaluator<BoolStore>;
            std::unique_ptr<algorithms::StateEvaluationFunction<State>>
                state_reward_aux0 = nullptr;
            std::unique_ptr<algorithms::StateEvaluationFunction<State>>
                state_reward_aux1 = nullptr;
            algorithms::StateEvaluationFunction<State>* state_reward =
                state_reward_;

            bool is_one = false;
            bool is_dead = false;

            if (zero_ && one_) {
                MyEval* ones = new MyEval(one_reward_, state_reward_, false);
                MyEval* dead_ends = new MyEval(minval_, ones, false);
                auto res = ec.compute_quotient_system<BoolStore&, BoolStore&>(
                    state, dead_ends->store_, ones->store_);
                // g_log << dead_ends.size() << " states are dead ends!"
                //       << std::endl;
                stats = res.first;
                qs = res.second;
                state_reward_aux0 =
                    std::unique_ptr<algorithms::StateEvaluationFunction<State>>(
                        ones);
                state_reward_aux1 =
                    std::unique_ptr<algorithms::StateEvaluationFunction<State>>(
                        dead_ends);
                state_reward = dead_ends;
                if (is_state_contained(
                        ones->store_, qs->get_representative_id(state))) {
                    is_one = true;
                }
                if (is_state_contained(
                        dead_ends->store_, qs->get_representative_id(state))) {
                    is_dead = true;
                }
            } else if (zero_) {
                MyEval* dead_ends = new MyEval(minval_, state_reward_, false);
                auto res = ec.compute_quotient_system<BoolStore&>(
                    state, dead_ends->store_);
                // g_log << dead_ends.size() << " states are dead ends!"
                //       << std::endl;
                stats = res.first;
                qs = res.second;
                state_reward_aux1 =
                    std::unique_ptr<algorithms::StateEvaluationFunction<State>>(
                        dead_ends);
                state_reward = dead_ends;
                if (is_state_contained(
                        dead_ends->store_, qs->get_representative_id(state))) {
                    is_dead = true;
                }
            } else {
                auto res = ec.compute_quotient_system(state);
                stats = res.first;
                qs = res.second;
            }

            g_log << "quotient system constructed!" << std::endl;

            if (is_one || is_dead) {
                if (is_one) {
                    g_log << "initial state has been identified as 1 goal "
                             "probability state!"
                          << std::endl;
                } else if (is_dead) {
                    g_log << "initial state has been identified as dead end!"
                          << std::endl;
                }
                g_log << "skipping execution of " << inner_->name()
                      << std::endl;
                return AnalysisResult(is_dead ? minval_ : one_reward_, stats);
            } else {
                stats->print(g_log);

                utils::Timer t;
                QuotientSystemFunctionFactory* qsf =
                    new QuotientSystemFunctionFactory(qs);
                IMDPEngine<QState>* inner = inner_->create_engine(
                    *qsf,
                    qs,
                    report_,
                    qsf->get_id_map(),
                    qsf->get_aops_gen(),
                    qsf->get_transition_gen(),
                    minval_,
                    maxval_,
                    qsf->create(state_reward),
                    qsf->create(transition_reward_));
                if (inner == nullptr) {
                    g_err
                        << inner_->name()
                        << " doesn't support the analysis of quotient systems!"
                        << std::endl;
                    utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
                }

                g_log << "running " << inner_->name() << " on quotient system"
                      << std::endl
                      << std::flush;
                AnalysisResult res =
                    inner->solve(qs->get_quotient_state(state));
                delete (inner);
                delete (qs);
                g_log << inner_->name() << " terminated after " << t
                      << std::endl;

                AnalysisResult ress(
                    res.result, new IPrintables({ stats, res.statistics }));
                res.statistics = nullptr;
                return ress;
            }
        }

    private:
        std::shared_ptr<MDPSolver> inner_;
        std::shared_ptr<GlobalStateEvaluator> prune_;
        ProgressReport* report_;
        algorithms::StateIDMap<State>* state_id_map_;
        algorithms::ApplicableActionsGenerator<State, Action>* aops_gen_;
        algorithms::TransitionGenerator<State, Action>* transition_gen_;
        value_type::value_t minval_;
        value_type::value_t maxval_;
        algorithms::StateEvaluationFunction<State>* state_reward_;
        algorithms::TransitionRewardFunction<State, Action>* transition_reward_;
        const bool zero_;
        const bool one_;
        const value_type::value_t one_reward_;
    };
};

static Plugin<SolverInterface> _plugin(
    "ec_decomposition",
    options::parse<SolverInterface, ECDecomposition>);

} // namespace solvers
} // namespace probabilistic
