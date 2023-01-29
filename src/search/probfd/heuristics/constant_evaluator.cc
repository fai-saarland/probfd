#include "probfd/heuristics/constant_evaluator.h"

#include "probfd/cost_model.h"

#include "option_parser.h"
#include "plugin.h"

#include <memory>

namespace probfd {
namespace heuristics {

static std::shared_ptr<TaskStateEvaluator>
_parse(options::OptionParser& parser)
{
    parser.add_option<double>("value", "", options::OptionParser::NONE);
    options::Options opts = parser.parse();
    if (parser.dry_run()) {
        return nullptr;
    }
    if (opts.contains("value")) {
        return std::make_shared<ConstantEvaluator<State>>(
            double_to_value(opts.get<double>("value")));
    } else {
        return std::make_shared<ConstantEvaluator<State>>(
            g_cost_model->optimal_value_bound().lower);
    }
}

static Plugin<TaskStateEvaluator> _plugin("const_eval", _parse);

} // namespace heuristics
} // namespace probfd
