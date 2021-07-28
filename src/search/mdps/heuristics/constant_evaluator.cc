#include "constant_evaluator.h"

#include "../../option_parser.h"
#include "../../plugin.h"
#include "../analysis_objectives/analysis_objective.h"
#include "../globals.h"

#include <memory>

namespace probabilistic {

ConstantEvaluator::ConstantEvaluator(value_type::value_t value)
    : value_(value)
{
}

EvaluationResult ConstantEvaluator::evaluate(const GlobalState&)
{
    return EvaluationResult(false, value_);
}

static std::shared_ptr<GlobalStateEvaluator>
_parse(options::OptionParser& parser)
{
    parser.add_option<double>("value", "", options::OptionParser::NONE);
    options::Options opts = parser.parse();
    if (parser.dry_run()) {
        return nullptr;
    }
    if (opts.contains("value")) {
        return std::make_shared<ConstantEvaluator>(
            value_type::from_double(opts.get<double>("value")));
    } else {
        return std::make_shared<ConstantEvaluator>(g_analysis_objective->max());
    }
}

static Plugin<GlobalStateEvaluator> _plugin("const", _parse);

} // namespace probabilistic
