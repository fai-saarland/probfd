#ifndef DOWNWARD_CLI_TASKS_COST_TASK_TRANSFORMATION_FEATURE_H
#define DOWNWARD_CLI_TASKS_COST_TASK_TRANSFORMATION_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
}

namespace downward::cli::tasks {

language::parser::InternalFunctionDefinitionBase&
add_cost_task_transformation_features(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif