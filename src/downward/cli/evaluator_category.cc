#include "downward/cli/plugins/plugin.h"

#include "downward/evaluator.h"

using namespace std;

using namespace downward::cli;

namespace {

class EvaluatorCategoryPlugin
    : public plugins::TypedCategoryPlugin<downward::Evaluator> {
public:
    EvaluatorCategoryPlugin()
        : TypedCategoryPlugin("Heuristic")
    {
        document_synopsis(
            "An evaluator specification is either a newly created evaluator "
            "instance or an evaluator that has been defined previously. "
            "This page describes how one can specify a new evaluator instance. "
            "For re-using evaluators, see "
            "OptionSyntax#Evaluator_Predefinitions.\n\n"
            "If the evaluator is a heuristic, "
            "definitions of //properties// in the descriptions below:\n\n"
            " * **admissible:** h(s) <= h*(s) for all states s\n"
            " * **consistent:** h(s) <= c(s, s') + h(s') for all states s "
            "connected to states s' by an action with cost c(s, s')\n"
            " * **safe:** h(s) = infinity is only true for states "
            "with h*(s) = infinity\n"
            " * **preferred operators:** this heuristic identifies "
            "preferred operators ");
        allow_variable_binding();
    }
};

EvaluatorCategoryPlugin _category_plugin;

} // namespace
