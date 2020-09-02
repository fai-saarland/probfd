#include "critical_path_heuristic_refinement.h"

#include "../option_parser.h"
#include "../plugin.h"
#include "../strips.h"
#include "../utils/system.h"
#include "critical_path_heuristic.h"
#include "critical_path_heuristic_nogoods.h"
#include "critical_path_online_cart_heuristic.h"

#include <cmath>
#include <iostream>

namespace critical_path_heuristic {

CriticalPathHeuristicRefinement::CriticalPathHeuristicRefinement(
    const options::Options& opts)
    : nogoods_(std::dynamic_pointer_cast<CriticalPathHeuristicNoGoods>(
        opts.get<std::shared_ptr<Heuristic>>("heuristic")))
    , carts_(std::dynamic_pointer_cast<CriticalPathOnlineCARTHeuristic>(
          opts.get<std::shared_ptr<Heuristic>>("heuristic")))
    , hc_([](std::shared_ptr<CriticalPathHeuristicNoGoods> ng,
             std::shared_ptr<CriticalPathOnlineCARTHeuristic> cart,
             std::shared_ptr<CriticalPathHeuristic> hc) {
        if (ng != nullptr) {
            return ng->get_underlying_heuristic();
        } else if (cart != nullptr) {
            return cart->get_underlying_heuristic();
        } else {
            return hc;
        }
    }(nogoods_,
          carts_,
          std::dynamic_pointer_cast<CriticalPathHeuristic>(
              opts.get<std::shared_ptr<Heuristic>>("heuristic"))))
    , task_(hc_->get_task())
    , overall_counter_limit_([](double limit, unsigned counters) {
        if (limit < 0) {
            return static_cast<unsigned>(-1);
        } else if (limit < 1) {
            std::cerr << "max_counter_ratio cannot be smaller than 1"
                      << std::endl;
            ::utils::exit_with(::utils::ExitCode::SEARCH_CRITICAL_ERROR);
        } else {
            double result = std::floor(limit * counters + 0.5);
            return (unsigned)result;
        }
    }(opts.get<double>("max_counter_ratio"), hc_->num_base_counters()))
    , overall_conjunction_limit_([](double limit, unsigned conjs) {
        if (limit < 0) {
            return static_cast<unsigned>(-1);
        } else if (limit < 1) {
            std::cerr << "max_conjunction_ratio cannot be smaller than 1"
                      << std::endl;
            ::utils::exit_with(::utils::ExitCode::SEARCH_CRITICAL_ERROR);
        } else {
            double result = std::floor(limit * conjs + 0.5);
            return (unsigned)result;
        }
    }(opts.get<double>("max_conjunction_ratio"), hc_->get_task()->num_facts()))
    , stop_listening_on_failure_(opts.get<bool>("stop_on_failure"))
{
    num_refinements_ = 0;
    num_refinements_wo_changes_ = 0;
}

void
CriticalPathHeuristicRefinement::add_options_to_parser(
    options::OptionParser& parser)
{
    parser.add_option<std::shared_ptr<Heuristic>>("heuristic");
    parser.add_option<double>("max_counter_ratio", "", "-1");
    parser.add_option<double>("max_conjunction_ratio", "", "-1");
    parser.add_option<bool>("stop_on_failure", "", "true");
}

bool
CriticalPathHeuristicRefinement::is_size_limit_reached() const
{
    return hc_->num_conjunctions() >= overall_conjunction_limit_
        || hc_->num_counters() >= overall_counter_limit_;
}

state_component::StateComponentListener::Status
CriticalPathHeuristicRefinement::on_new_component(
    state_component::StateComponent& component,
    state_component::StateComponent& neighbors)
{
    if (is_size_limit_reached()) {
        return Status::StopListening;
    }
    num_refinements_++;
    unsigned before = hc_->num_conjunctions();
    bool success = refine_conjunction_set(component, neighbors);
    if (success && before == hc_->num_conjunctions()) {
        num_refinements_wo_changes_++;
    }
    if (is_size_limit_reached()) {
        return Status::StopListening;
    }
#ifndef NDEBUG
    if (success) {
        component.reset();
        hc_->evaluate(component.current());
        assert(hc_->is_dead_end());
    }
#endif
    if (success) {
        if (nogoods_ != nullptr) {
            component.reset();
            nogoods_->update_nogoods(component.current());
        } else if (carts_ != nullptr) {
            carts_->extract_and_insert_new_trace();
        }
        return Status::Mark;
    }
    return (
        stop_listening_on_failure_ ? Status::StopListening
                                   : Status::KeepListening);
}

void
CriticalPathHeuristicRefinement::statistics() const
{
    std::cout << "**** Critical-Path Heuristic " << name() << " Refinement ****"
              << std::endl;
    std::cout << "Refinements: " << num_refinements_ << std::endl;
    std::cout << "Refinements w/o change: " << num_refinements_wo_changes_
              << std::endl;
    hc_->print_statistics();
    if (nogoods_ != nullptr) {
        nogoods_->print_statistics();
    } else if (carts_ != nullptr) {
        carts_->print_statistics();
    }
}

} // namespace critical_path_heuristic
