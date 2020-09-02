#include "sum_evaluator.h"

#include <limits>
#include <cassert>

#include "option_parser.h"
#include "plugin.h"

using namespace std;

SumEvaluator::SumEvaluator(const options::Options &opts)
    : CombiningEvaluator(opts.get_list<std::shared_ptr<Evaluator>>("evals")) {
}

SumEvaluator::SumEvaluator(const std::vector<std::shared_ptr<Evaluator>> &evals)
    : CombiningEvaluator(evals) {
}

SumEvaluator::~SumEvaluator() {
}

int SumEvaluator::combine_values(const vector<int> &values) {
    int result = 0;
    for (size_t i = 0; i < values.size(); ++i) {
        assert(values[i] >= 0);
        result += values[i];
        assert(result >= 0); // Check against overflow.
    }
    return result;
}



static std::shared_ptr<Evaluator> _parse(options::OptionParser &parser) {
    parser.document_synopsis("Sum evaluator",
                             "Calculates the sum of the sub-evaluators.");

    parser.add_list_option<std::shared_ptr<Evaluator>>("evals",
                                              "at least one scalar evaluator");
    options::Options opts = parser.parse();

    opts.verify_list_non_empty<std::shared_ptr<Evaluator>>("evals");

    if (parser.dry_run())
        return nullptr;
    else
        return std::make_shared<SumEvaluator>(opts);
}

static Plugin<Evaluator> _plugin("sum", _parse);
