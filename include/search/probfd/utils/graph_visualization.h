#ifndef PROBFD_UTILS_GRAPH_VISUALIZATION_H
#define PROBFD_UTILS_GRAPH_VISUALIZATION_H

#include "probfd/engine_interfaces/reward_function.h"
#include "probfd/engine_interfaces/state_evaluator.h"
#include "probfd/engine_interfaces/state_id_map.h"
#include "probfd/engine_interfaces/transition_generator.h"

#include "probfd/storage/per_state_storage.h"

#include <cassert>
#include <deque>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <vector>

namespace probfd {

/// This namespace contains code used for dumping search spaces as dot graphs.
namespace graphviz {

namespace internal {
class GraphBuilder {
public:
    class Configurable {
        friend class GraphBuilder;
        std::string name_;
        std::map<std::string, std::string> attributes_;

    public:
        explicit Configurable(std::string name)
            : name_(std::move(name))
        {
        }

        void setAttribute(std::string attribute, std::string value)
        {
            attributes_.emplace(std::move(attribute), std::move(value));
        }
    };

    class Node : public Configurable {
        friend class GraphBuilder;
        using Configurable::Configurable;
        int rank_;

        Node(std::string name, int rank)
            : Configurable(std::move(name))
            , rank_(rank)
        {
        }

    public:
        int getRank() const { return rank_; }
    };

private:
    class StateNode : public Node {
        friend class GraphBuilder;
        StateID id_;

        StateNode(std::string name, int rank, StateID id)
            : Node(std::move(name), rank)
            , id_(id)
        {
        }
    };

    class DummyNode : public Node {
        friend class GraphBuilder;
        DummyNode(std::string name, int rank)
            : Node(std::move(name), rank)
        {
        }
    };

    class Edge : public Configurable {
        friend class GraphBuilder;
        Node& source_;
        Node& target_;

        Edge(std::string name, Node& source, Node& target)
            : Configurable(std::move(name))
            , source_(source)
            , target_(target)
        {
        }
    };

    std::vector<std::unique_ptr<DummyNode>> dummynodes_;
    std::vector<std::unique_ptr<StateNode>> nodes_;
    std::vector<std::unique_ptr<Edge>> edges_;

    std::map<StateID, StateNode*> id_to_nodes_;
    StateID initial_;

    std::map<int, std::vector<Node*>> ranked_nodes_;

public:
    explicit GraphBuilder(StateID initial)
        : GraphBuilder(initial, getDefaultNodeName())
    {
    }

    GraphBuilder(StateID initial, std::string name)
        : initial_(initial)
    {
        insertStateNode(initial, 0, std::move(name));
    }

    Node& getNode(StateID id) { return *id_to_nodes_[id]; }

    std::pair<Node*, bool> insertNode(StateID id)
    {
        return insertStateNode(id, 0, getDefaultNodeName());
    }

    Node* CreateDummyNode(int rank)
    {
        return CreateDummyNode(rank, getDefaultDummyNodeName());
    }

    Node* CreateDummyNode(int rank, std::string name)
    {
        auto& r =
            dummynodes_.emplace_back(new DummyNode(std::move(name), rank));
        ranked_nodes_[rank].push_back(r.get());
        return r.get();
    }

    Node* CreateStateNode(StateID id, int rank)
    {
        return CreateStateNode(id, rank, getDefaultNodeName());
    }

    Node* CreateStateNode(StateID id, int rank, std::string name)
    {
        auto& r = nodes_.emplace_back(new StateNode(std::move(name), rank, id));
        id_to_nodes_[id] = r.get();
        r->rank_ = rank;

        ranked_nodes_[rank].push_back(r.get());

        return r.get();
    }

    std::pair<Node*, bool> insertStateNode(StateID id, int rank)
    {
        return insertStateNode(id, rank, getDefaultNodeName());
    }

    std::pair<Node*, bool>
    insertStateNode(StateID id, int rank, std::string name)
    {
        auto it = id_to_nodes_.find(id);

        if (it != id_to_nodes_.end()) {
            return std::make_pair(it->second, false);
        }

        return std::make_pair(CreateStateNode(id, rank, std::move(name)), true);
    }

    Configurable& createEdge(Node& source, Node& target)
    {
        return createEdge(source, target, getDefaultEdgeName());
    }

    Configurable&
    createEdge(Node& source_node, Node& target_node, std::string name)
    {
        return *edges_.emplace_back(
            new Edge(std::move(name), source_node, target_node));
    }

    void emit(std::ostream& out)
    {
        out << "digraph {\n"
            << "    # Node and edge settings\n"
            << "    node [fontsize=8];\n"
            << "    edge [fontsize=8, arrowsize=0.4];\n\n"
            << "    # Initial Arrow\n"
            << "    initial_arrow_origin [label=\"\", shape=none];\n"
            << "    initial_arrow_origin -> " << id_to_nodes_[initial_]->name_
            << " [arrowhead=vee];\n";

        out << "\n    # Node Ranking\n";

        for (const auto& group : utils::values(ranked_nodes_)) {
            out << "    { ";
            emitAttribute(out, "rank", "same");
            out << "; ";

            for (const auto* node : group) {
                out << node->name_ << "; ";
            }

            out << "}\n";
        }

        out << "\n    # State Nodes\n";

        for (const auto& node : nodes_) {
            out << "    ";
            out << node->name_;
            if (!node->attributes_.empty()) {
                out << " ";
                emitAttributeList(out, node->attributes_);
            }
            out << ";\n";
        }

        out << "\n    # Intermediate Nodes\n";

        for (const auto& node : dummynodes_) {
            out << "    ";
            out << node->name_;
            if (!node->attributes_.empty()) {
                out << " ";
                emitAttributeList(out, node->attributes_);
            }
            out << ";\n";
        }

        out << "\n    # Edges\n";

        for (const auto& edge : edges_) {
            out << "    ";
            emitEdge(out, edge->source_, edge->target_);
            if (!edge->attributes_.empty()) {
                out << " ";
                emitAttributeList(out, edge->attributes_);
            }
            out << ";\n";
        }

        out << "}\n";
    }

private:
    void emitAttributeList(
        std::ostream& out,
        const std::map<std::string, std::string>& attributes)
    {
        out << "[";

        auto it = attributes.begin();
        auto end = attributes.end();

        assert(it != end);

        emitAttribute(out, it->first, it->second);
        while (++it != end) {
            out << ", ";
            emitAttribute(out, it->first, it->second);
        }

        out << "]";
    }

    void emitAttribute(
        std::ostream& out,
        const std::string& attribute,
        const std::string& value)
    {
        out << attribute << "=\"" << value << "\"";
    }

    void emitEdge(std::ostream& out, Node& source, Node& target)
    {
        out << source.name_ << " -> " << target.name_;
    }

    std::string getDefaultNodeName() const
    {
        return "node_" + std::to_string(nodes_.size());
    }

    std::string getDefaultDummyNodeName() const
    {
        return "intermediate_node_" + std::to_string(dummynodes_.size());
    }

    std::string getDefaultEdgeName() const
    {
        return "edge_" + std::to_string(edges_.size());
    }
};
} // namespace internal

template <typename State, typename Action>
void dump_state_space_dot_graph(
    std::ostream& out,
    const State& initial_state,
    engine_interfaces::StateIDMap<State>* state_id_map,
    engine_interfaces::TransitionGenerator<Action>* transition_gen,
    engine_interfaces::RewardFunction<State, Action>* reward_fn,
    engine_interfaces::StateEvaluator<State>* prune = nullptr,
    std::function<std::string(const State&)> sstr =
        [](const State&) { return ""; },
    std::function<std::string(const Action&)> astr =
        [](const Action&) { return ""; },
    bool expand_terminal = false)
{
    struct SearchInfo {
        StateID state_id;
        State state;
        internal::GraphBuilder::Node* node;

        SearchInfo(
            StateID state_id,
            State state,
            internal::GraphBuilder::Node* node)
            : state_id(state_id)
            , state(state)
            , node(node)
        {
        }
    };

    StateID istateid = state_id_map->get_state_id(initial_state);
    internal::GraphBuilder builder(istateid);
    std::stringstream ss;
    ss << std::setprecision(3);

    std::deque<SearchInfo> open;
    open.emplace_back(istateid, initial_state, &builder.getNode(istateid));

    do {
        auto& s = open.front();

        const State& state = s.state;
        const StateID state_id = s.state_id;
        auto* node = s.node;

        node->setAttribute("label", sstr(state));
        node->setAttribute("shape", "circle");

        const auto rew = reward_fn->get_termination_info(state);
        bool expand = expand_terminal || !rew.is_goal_state();

        if (rew.is_goal_state()) {
            node->setAttribute("peripheries", std::to_string(2));
        } else if (
            expand && prune != nullptr &&
            prune->evaluate(state).is_unsolvable()) {
            expand = false;
            node->setAttribute("peripheries", std::to_string(3));
        }

        open.pop_front();

        if (!expand) {
            continue;
        }

        std::vector<Action> aops;
        std::vector<Distribution<StateID>> all_successors;
        transition_gen->generate_all_transitions(
            state_id,
            aops,
            all_successors);

        for (auto& dist : all_successors) {
            dist.make_unique();
        }

        std::vector<std::pair<Action, Distribution<StateID>>> transitions;

        for (std::size_t i = 0; i != aops.size(); ++i) {
            transitions.emplace_back(aops[i], all_successors[i]);
        }

        auto less = [](const auto& left, const auto& right) {
            return left.second < right.second;
        };

        auto equals = [](const auto& left, const auto& right) {
            return left.second == right.second;
        };

        std::sort(transitions.begin(), transitions.end(), less);
        transitions.erase(
            std::unique(transitions.begin(), transitions.end(), equals),
            transitions.end());

        for (const auto& [act, successors] : transitions) {
            const auto a_reward = reward_fn->get_action_reward(state_id, act);
            if (a_reward != value_type::zero) {
                ss << a_reward << "\\n";
            }
            ss << astr(act);
            std::string label_text = ss.str();
            ss.str("");

            if (successors.is_dirac()) {
                const auto succ_id = successors.begin()->element;
                auto [succ_node, inserted] = builder.insertNode(succ_id);

                auto& direct_edge = builder.createEdge(*node, *succ_node);
                direct_edge.setAttribute("arrowhead", "vee");
                direct_edge.setAttribute("label", label_text);

                if (inserted) {
                    open.emplace_back(
                        succ_id,
                        state_id_map->get_state(succ_id),
                        succ_node);
                }

                continue;
            }

            Distribution<internal::GraphBuilder::Node*> successor_nodes;
            int my_rank = node->getRank();
            int max_rank = 0;

            for (const auto& [succ_id, prob] : successors) {
                auto [succ_node, inserted] =
                    builder.insertStateNode(succ_id, my_rank + 2);
                max_rank = std::max(max_rank, succ_node->getRank());
                successor_nodes.add(succ_node, prob);

                if (inserted) {
                    open.emplace_back(
                        succ_id,
                        state_id_map->get_state(succ_id),
                        succ_node);
                }
            }

            auto* intermediate = builder.CreateDummyNode(
                my_rank < max_rank ? my_rank + 1 : my_rank - 1);
            intermediate->setAttribute("xlabel", astr(act));
            // intermediate->setAttribute("tooltip", astr(act));
            intermediate->setAttribute("shape", "point");
            intermediate->setAttribute("style", "filled");
            intermediate->setAttribute("fillcolor", "black");

            auto& interm_edge = builder.createEdge(*node, *intermediate);
            interm_edge.setAttribute("arrowhead", "none");
            interm_edge.setAttribute("label", label_text);

            for (const auto& [succ_node, prob] : successor_nodes) {
                ss << prob;
                std::string prob_text = ss.str();
                ss.str("");

                auto& edge = builder.createEdge(*intermediate, *succ_node);
                edge.setAttribute("arrowhead", "vee");
                edge.setAttribute("label", prob_text);
            }
        }

        aops.clear();
    } while (!open.empty());

    builder.emit(out);
}

} // namespace graphviz
} // namespace probfd

#endif // __GRAPH_VISUALIZATION_H__