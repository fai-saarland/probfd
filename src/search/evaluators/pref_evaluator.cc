#include "evaluators/pref_evaluator.h"

#include "option_parser.h"
#include "plugin.h"

PrefEvaluator::PrefEvaluator() {
}

PrefEvaluator::~PrefEvaluator() {
}

void PrefEvaluator::evaluate(int, bool preferred) {
    value_preferred = preferred;
}

bool PrefEvaluator::is_dead_end() const {
    return false;
}

bool PrefEvaluator::dead_end_is_reliable() const {
    return true;
}

int PrefEvaluator::get_value() const {
    if (value_preferred)
        return 0;
    else
        return 1;
}

static std::shared_ptr<Evaluator> _parse(options::OptionParser &parser) {
    parser.document_synopsis("Preference evaluator",
                             "Returns 0 if preferred is true and 1 otherwise.");
    parser.parse();
    if (parser.dry_run())
        return 0;
    else
        return std::make_shared<PrefEvaluator>();
}

static Plugin<Evaluator> _plugin("pref", _parse);
