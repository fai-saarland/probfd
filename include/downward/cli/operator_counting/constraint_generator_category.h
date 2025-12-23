#ifndef DOWNWARD_CLI_OPERATOR_COUNTING_CONSTRAINT_GENERATOR_CATEGORY_H
#define DOWNWARD_CLI_OPERATOR_COUNTING_CONSTRAINT_GENERATOR_CATEGORY_H

namespace language::plugins {
class Namespace;
}

namespace downward::cli::operator_counting {

void add_constraint_generator_category(language::plugins::Namespace& nspace);

}

#endif