#include "heuristics/ipc_max_heuristic.h"

#include "option_parser.h"
#include "plugin.h"

#include <memory>
#include <limits>
#include <string>
#include <vector>

using namespace std;


IPCMaxHeuristic::IPCMaxHeuristic(const options::Options &opts)
    : Heuristic(opts), evaluators(opts.get_list<std::shared_ptr<Heuristic> >("heuristics")) {
}

IPCMaxHeuristic::~IPCMaxHeuristic() {
}

int IPCMaxHeuristic::compute_heuristic(const GlobalState &state) {
    dead_end = false;
    dead_end_reliable = false;
    value = 0;
    for (size_t i = 0; i < evaluators.size(); ++i) {
        evaluators[i]->evaluate(state);

        if (evaluators[i]->is_dead_end()) {
            value = numeric_limits<int>::max();
            dead_end = true;
            if (evaluators[i]->dead_end_is_reliable()) {
                dead_end_reliable = true;
                value = -1;
                break;
            }
        } else {
            value = max(value, evaluators[i]->get_value());
        }
    }
    return value;
}

bool IPCMaxHeuristic::reach_state(const GlobalState &parent_state, const GlobalOperator &op,
                                  const GlobalState &state) {
    bool result = false;
    for (size_t i = 0; i < evaluators.size(); ++i) {
        if (evaluators[i]->reach_state(parent_state, op, state)) {
            result = true;
            // Don't break: we must call reached_state everywhere.
        }
    }
    return result;
}

static std::shared_ptr<Heuristic> _parse(options::OptionParser &parser) {
    parser.document_synopsis("IPC-Max Heuristic", "");
    parser.add_list_option<std::shared_ptr<Heuristic> >("heuristics");
    Heuristic::add_options_to_parser(parser);
    options::Options opts = parser.parse();

    opts.verify_list_non_empty<std::shared_ptr<Heuristic> >("heuristics");

    if (parser.dry_run())
        return 0;
    else
        return std::make_shared<IPCMaxHeuristic>(opts);
}

static Plugin<Heuristic> plugin("max", _parse);
