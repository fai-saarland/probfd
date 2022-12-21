#include "legacy/global_state.h"

#include "legacy/globals.h"

#include "legacy/state_registry.h"

#include <algorithm>
#include <cassert>
#include <iostream>

namespace legacy {

using namespace std;

GlobalState::GlobalState(
    const PackedStateBin* buffer_,
    const legacy::StateRegistry& registry_,
    legacy::StateID id_)
    : buffer(buffer_)
    , registry(&registry_)
    , state_packer(registry_.get_state_packer())
    , id(id_)
{
    assert(buffer);
    assert(id != legacy::StateID::no_state);
}

GlobalState::GlobalState(
    const PackedStateBin* buffer,
    const int_packer::IntPacker* state_packer)
    : buffer(buffer)
    , registry(nullptr)
    , state_packer(state_packer)
    , id(legacy::StateID::no_state)
{
}

GlobalState::~GlobalState() { }

int
GlobalState::operator[](size_t index) const
{
    return state_packer->get(buffer, index);
}

bool
GlobalState::operator==(const GlobalState& other) const
{
    if (registry == nullptr) {
        return other.registry == nullptr;
    }
    if (other.registry == nullptr) {
        return false;
    }
    assert(registry == other.registry);
    return id == other.id;
}

void
GlobalState::dump_pddl() const
{
    for (size_t i = 0; i < g_variable_domain.size(); ++i) {
        const string& fact_name = g_fact_names[i][(*this)[i]];
        if (fact_name != "<none of those>")
            cout << fact_name << endl;
    }
}

void
GlobalState::dump_fdr() const
{
    for (size_t i = 0; i < g_variable_domain.size(); ++i)
        cout << "  #" << i << " [" << g_variable_name[i] << "] -> "
             << (*this)[i] << endl;
}

std::ostream& operator<<(std::ostream& out, const GlobalState& state)
{
    out << "GlobalState(" << state.get_id() << ")";
    return out;
}

}