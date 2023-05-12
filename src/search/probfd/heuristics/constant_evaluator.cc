#include "probfd/heuristics/constant_evaluator.h"

#include "probfd/cost_model.h"

#include "option_parser.h"
#include "plugin.h"

#include <memory>

namespace probfd {
namespace heuristics {

static std::shared_ptr<TaskEvaluator> _parse(options::OptionParser& parser)
{
    parser.add_option<double>("value", "", options::OptionParser::NONE);
    options::Options opts = parser.parse();
    if (parser.dry_run()) {
        return nullptr;
    }
    return std::make_shared<ConstantEvaluator<State>>(
        double_to_value(opts.get<double>("value")));
}

static std::shared_ptr<TaskEvaluator>
_parse_blind(options::OptionParser& parser)
{
    if (parser.dry_run()) {
        return nullptr;
    }
    return std::make_shared<BlindEvaluator<State>>();
}

static Plugin<TaskEvaluator> _plugin("const_eval", _parse);
static Plugin<TaskEvaluator> _plugin2("blind_eval", _parse_blind);

} // namespace heuristics
} // namespace probfd
