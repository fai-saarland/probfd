#ifndef VARIABLE_SPACE_H
#define VARIABLE_SPACE_H

#include <string>

namespace downward {
struct FactPair;
}

namespace downward {

class VariableSpace {
public:
    virtual ~VariableSpace() = default;

    virtual int get_num_variables() const = 0;
    virtual int get_variable_domain_size(int var) const = 0;

    virtual std::string get_variable_name(int var) const = 0;
    virtual std::string get_fact_name(const FactPair& fact) const = 0;
};

} // namespace downward

#endif
