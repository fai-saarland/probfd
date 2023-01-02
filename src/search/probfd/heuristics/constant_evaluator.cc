#include "probfd/heuristics/constant_evaluator.h"

#include "probfd/analysis_objectives/analysis_objective.h"

#include "option_parser.h"
#include "plugin.h"

#include <memory>

namespace probfd {
namespace heuristics {

static std::shared_ptr<GlobalStateEvaluator>
_parse(options::OptionParser& parser)
{
    parser.add_option<double>("value", "", options::OptionParser::NONE);
    options::Options opts = parser.parse();
    if (parser.dry_run()) {
        return nullptr;
    }
    if (opts.contains("value")) {
        return std::make_shared<ConstantEvaluator<State>>(
            value_type::from_double(opts.get<double>("value")));
    } else {
        return std::make_shared<ConstantEvaluator<State>>(
            g_analysis_objective->reward_bound().upper);
    }
}

static Plugin<GlobalStateEvaluator> _plugin("const_eval", _parse);

} // namespace heuristics
} // namespace probfd
