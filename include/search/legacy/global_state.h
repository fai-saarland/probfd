#ifndef LEGACY_GLOBAL_STATE_H
#define LEGACY_GLOBAL_STATE_H

#include "algorithms/int_packer.h"
#include "legacy/state_id.h"

#include <cstddef>
#include <iostream>
#include <vector>

namespace legacy {
class GlobalOperator;
class StateRegistry;

typedef int_packer::IntPacker::Bin PackedStateBin;

// For documentation on classes relevant to storing and working with registered
// states see the file state_registry.h.
class GlobalState {
    friend class legacy::StateRegistry;
    template <class Entry>
    friend class PerStateInformation;
    // Values for vars are maintained in a packed state and accessed on demand.
    const PackedStateBin* buffer;
    // registry isn't a reference because we want to support operator=
    const legacy::StateRegistry* registry;
    const int_packer::IntPacker* state_packer;
    legacy::StateID id;
    // Only used by the state registry.
    GlobalState(
        const PackedStateBin* buffer_,
        const legacy::StateRegistry& registry_,
        legacy::StateID id_);

    const PackedStateBin* get_packed_buffer() const { return buffer; }

    // No implementation to prevent default construction
    GlobalState();

public:
    explicit GlobalState(
        const PackedStateBin* buffer,
        const int_packer::IntPacker* state_packer);
    ~GlobalState();

    legacy::StateID get_id() const { return id; }

    int operator[](std::size_t index) const;
    bool operator==(const GlobalState& other) const;

    const legacy::StateRegistry& get_registry() const { return *registry; }

    void dump_pddl() const;
    void dump_fdr() const;
};
} // namespace legacy

std::ostream& operator<<(std::ostream& out, const legacy::GlobalState& state);

namespace std {
template <>
struct hash<legacy::GlobalState> {
    size_t operator()(const legacy::GlobalState& state) const
    {
        return state.get_id().hash();
    }
};
} // namespace std

#endif
