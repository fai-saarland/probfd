#ifndef DOWNWARD_CLI_TASKS_IDENTITY_TASK_TRANSFORMATION_FEATURE_H
#define DOWNWARD_CLI_TASKS_IDENTITY_TASK_TRANSFORMATION_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
}

namespace downward::cli::tasks {

language::parser::InternalFunctionDefinitionBase&
add_identity_task_transformation_features(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif