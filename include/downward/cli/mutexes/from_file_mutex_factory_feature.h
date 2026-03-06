#ifndef DOWNWARD_CLI_MUTEXES_FROM_FILE_MUTEX_FACTORY_FEATURE_H
#define DOWNWARD_CLI_MUTEXES_FROM_FILE_MUTEX_FACTORY_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace downward::cli::mutexes {

language::parser::InternalFunctionDefinitionBase&
add_from_file_mutex_factory_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif