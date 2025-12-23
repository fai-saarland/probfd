#ifndef DOWNWARD_EVALUATORS_SUBCATEGORY_H
#define DOWNWARD_EVALUATORS_SUBCATEGORY_H

namespace language::plugins {
class DocumentationTopic;
class Registry;
}

namespace downward::cli::evaluators {

language::plugins::DocumentationTopic& add_evaluator_subcategory(
    language::plugins::Registry& registry);

}

#endif