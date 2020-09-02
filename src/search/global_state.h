#ifndef GLOBAL_STATE_H
#define GLOBAL_STATE_H

#include "algorithms/int_packer.h"
#include "state_id.h"

#include <cstddef>
#include <iostream>
#include <vector>

class GlobalOperator;
class StateRegistry;

typedef int_packer::IntPacker::Bin PackedStateBin;

// For documentation on classes relevant to storing and working with registered
// states see the file state_registry.h.
class GlobalState {
    friend class StateRegistry;
    template<class Entry>
    friend class PerStateInformation;
    // Values for vars are maintained in a packed state and accessed on demand.
    const PackedStateBin* buffer;
    // registry isn't a reference because we want to support operator=
    const StateRegistry* registry;
    const int_packer::IntPacker* state_packer;
    StateID id;
    // Only used by the state registry.
    GlobalState(
        const PackedStateBin* buffer_,
        const StateRegistry& registry_,
        StateID id_);

    const PackedStateBin* get_packed_buffer() const { return buffer; }

    const StateRegistry& get_registry() const { return *registry; }

    // No implementation to prevent default construction
    GlobalState();

public:
    explicit GlobalState(
        const PackedStateBin* buffer,
        const int_packer::IntPacker* state_packer);
    ~GlobalState();

    StateID get_id() const { return id; }

    int operator[](std::size_t index) const;
    bool operator==(const GlobalState& other) const;

    void dump_pddl() const;
    void dump_fdr() const;
};

std::ostream& operator<<(std::ostream& out, const GlobalState& state);

namespace std {
template<>
struct hash<GlobalState> {
    size_t operator()(const GlobalState& state) const
    {
        return state.get_id().hash();
    }
};
} // namespace std

#endif
