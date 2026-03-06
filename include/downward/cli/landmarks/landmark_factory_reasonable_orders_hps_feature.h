#ifndef DOWNWARD_LANDMARKS_LANDMARK_FACTORY_REASONABLE_ORDERS_HPS_FEATURE_H
#define DOWNWARD_LANDMARKS_LANDMARK_FACTORY_REASONABLE_ORDERS_HPS_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
}

namespace downward::cli::landmarks {

language::parser::InternalFunctionDefinitionBase&
add_landmark_factory_reasonable_orders_hps_feature(
    language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif