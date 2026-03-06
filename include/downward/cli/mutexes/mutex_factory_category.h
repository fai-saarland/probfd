#ifndef DOWNWARD_CLI_MUTEXES_MUTEX_FACTORY_CATEGORY_H
#define DOWNWARD_CLI_MUTEXES_MUTEX_FACTORY_CATEGORY_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace downward::cli::mutexes {

void add_mutex_factory_category(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif