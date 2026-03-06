#ifndef DOWNWARD_LANDMARKS_LANDMARK_FACTORY_CATEGORY_H
#define DOWNWARD_LANDMARKS_LANDMARK_FACTORY_CATEGORY_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace downward::cli::landmarks {

void add_landmark_factory_category(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif