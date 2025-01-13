#ifndef TRANSFORMATIONS_DOMAIN_ABSTRACTION_H
#define TRANSFORMATIONS_DOMAIN_ABSTRACTION_H

#include "downward/task_transformation.h"

struct FactPair;

class DomainAbstraction : public StateMapping {
    const std::vector<std::vector<int>> value_map;

public:
    explicit DomainAbstraction(std::vector<std::vector<int>> value_map);

    virtual void
    convert_ancestor_state_values(std::vector<int>& values) const override;

    int get_abstract_value(const FactPair& fact) const;

    FactPair get_abstract_fact(const FactPair& fact) const;
};

#endif
