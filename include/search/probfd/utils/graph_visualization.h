#ifndef PROBFD_UTILS_GRAPH_VISUALIZATION_H
#define PROBFD_UTILS_GRAPH_VISUALIZATION_H

#include "probfd/storage/per_state_storage.h"

#include "probfd/distribution.h"
#include "probfd/evaluator.h"
#include "probfd/mdp.h"

#include <cassert>
#include <deque>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <ranges>
#include <sstream>
#include <vector>

/// This namespace contains code used for dumping search spaces as dot graphs.
namespace probfd::graphviz {

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

        void set_attribute(std::string attribute, std::string value)
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
        [[nodiscard]]
        int get_rank() const
        {
            return rank_;
        }
    };

private:
    class StateNode : public Node {
        friend class GraphBuilder;
        using Node::Node;
    };

    class DummyNode : public Node {
        friend class GraphBuilder;
        using Node::Node;
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
        : GraphBuilder(initial, get_default_node_name())
    {
    }

    GraphBuilder(StateID initial, std::string name)
        : initial_(initial)
    {
        insert_state_node(initial, 0, std::move(name));
    }

    Node& get_node(StateID id) { return *id_to_nodes_[id]; }

    std::pair<Node*, bool> insert_node(StateID id)
    {
        return insert_state_node(id, 0, get_default_node_name());
    }

    Node* create_dummy_node(int rank)
    {
        return create_dummy_node(rank, get_default_dummy_node_name());
    }

    Node* create_dummy_node(int rank, std::string name)
    {
        auto& r =
            dummynodes_.emplace_back(new DummyNode(std::move(name), rank));
        ranked_nodes_[rank].push_back(r.get());
        return r.get();
    }

    Node* create_state_node(StateID id, int rank, std::string name)
    {
        auto& r = nodes_.emplace_back(new StateNode(std::move(name), rank));
        id_to_nodes_[id] = r.get();
        r->rank_ = rank;

        ranked_nodes_[rank].push_back(r.get());

        return r.get();
    }

    std::pair<Node*, bool> insert_state_node(StateID id, int rank)
    {
        return insert_state_node(id, rank, get_default_node_name());
    }

    std::pair<Node*, bool>
    insert_state_node(StateID id, int rank, std::string name)
    {
        auto it = id_to_nodes_.find(id);

        if (it != id_to_nodes_.end()) {
            return std::make_pair(it->second, false);
        }

        return std::make_pair(
            create_state_node(id, rank, std::move(name)),
            true);
    }

    Configurable& create_edge(Node& source, Node& target)
    {
        return create_edge(source, target, get_default_edge_name());
    }

    Configurable&
    create_edge(Node& source_node, Node& target_node, std::string name)
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

        for (const auto& group : std::views::values(ranked_nodes_)) {
            out << "    { ";
            emit_attribute(out, "rank", "same");
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
                emit_attribute_list(out, node->attributes_);
            }
            out << ";\n";
        }

        out << "\n    # Intermediate Nodes\n";

        for (const auto& node : dummynodes_) {
            out << "    ";
            out << node->name_;
            if (!node->attributes_.empty()) {
                out << " ";
                emit_attribute_list(out, node->attributes_);
            }
            out << ";\n";
        }

        out << "\n    # Edges\n";

        for (const auto& edge : edges_) {
            out << "    ";
            emit_edge(out, edge->source_, edge->target_);
            if (!edge->attributes_.empty()) {
                out << " ";
                emit_attribute_list(out, edge->attributes_);
            }
            out << ";\n";
        }

        out << "}\n";
    }

private:
    static void emit_attribute_list(
        std::ostream& out,
        const std::map<std::string, std::string>& attributes)
    {
        out << "[";

        auto it = attributes.begin();
        auto end = attributes.end();

        assert(it != end);

        emit_attribute(out, it->first, it->second);
        while (++it != end) {
            out << ", ";
            emit_attribute(out, it->first, it->second);
        }

        out << "]";
    }

    static void emit_attribute(
        std::ostream& out,
        const std::string& attribute,
        const std::string& value)
    {
        out << attribute << "=\"" << value << "\"";
    }

    static void emit_edge(std::ostream& out, Node& source, Node& target)
    {
        out << source.name_ << " -> " << target.name_;
    }

    [[nodiscard]]
    std::string get_default_node_name() const
    {
        return "node_" + std::to_string(nodes_.size());
    }

    [[nodiscard]]
    std::string get_default_dummy_node_name() const
    {
        return "intermediate_node_" + std::to_string(dummynodes_.size());
    }

    [[nodiscard]]
    std::string get_default_edge_name() const
    {
        return "edge_" + std::to_string(edges_.size());
    }
};
} // namespace internal

template <typename State, typename Action>
void dump_state_space_dot_graph(
    std::ostream& out,
    const State& initial_state,
    MDP<State, Action>* mdp,
    Evaluator<State>* prune = nullptr,
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

    StateID istateid = mdp->get_state_id(initial_state);
    internal::GraphBuilder builder(istateid);
    std::stringstream ss;
    ss << std::setprecision(3);

    std::deque<SearchInfo> open;
    open.emplace_back(istateid, initial_state, &builder.get_node(istateid));

    do {
        auto& s = open.front();

        const State& state = s.state;
        auto* node = s.node;

        node->set_attribute("label", sstr(state));
        node->set_attribute("shape", "circle");

        const auto term = mdp->get_termination_info(state);
        bool expand = expand_terminal || !term.is_goal_state();

        if (term.is_goal_state()) {
            node->set_attribute("peripheries", std::to_string(2));
        } else if (
            expand && prune != nullptr &&
            prune->evaluate(state) == term.get_cost()) {
            expand = false;
            node->set_attribute("peripheries", std::to_string(3));
        }

        open.pop_front();

        if (!expand) {
            continue;
        }

        std::vector<Action> aops;
        std::vector<Distribution<StateID>> all_successors;
        mdp->generate_all_transitions(state, aops, all_successors);

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
            const auto a_cost = mdp->get_action_cost(act);
            if (a_cost != 0_vt) {
                ss << a_cost << "\\n";
            }
            ss << astr(act);
            std::string label_text = ss.str();
            ss.str("");

            if (successors.is_dirac()) {
                const auto succ_id = successors.begin()->item;
                auto [succ_node, inserted] = builder.insert_node(succ_id);

                auto& direct_edge = builder.create_edge(*node, *succ_node);
                direct_edge.set_attribute("arrowhead", "vee");
                direct_edge.set_attribute("label", label_text);

                if (inserted) {
                    open.emplace_back(
                        succ_id,
                        mdp->get_state(succ_id),
                        succ_node);
                }

                continue;
            }

            Distribution<internal::GraphBuilder::Node*> successor_nodes;
            int my_rank = node->get_rank();
            int max_rank = 0;

            for (const auto& [succ_id, prob] : successors) {
                auto [succ_node, inserted] =
                    builder.insert_state_node(succ_id, my_rank + 2);
                max_rank = std::max(max_rank, succ_node->get_rank());
                successor_nodes.add_probability(succ_node, prob);

                if (inserted) {
                    open.emplace_back(
                        succ_id,
                        mdp->get_state(succ_id),
                        succ_node);
                }
            }

            auto* intermediate = builder.create_dummy_node(
                my_rank < max_rank ? my_rank + 1 : my_rank - 1);
            intermediate->set_attribute("xlabel", astr(act));
            // intermediate->set_attribute("tooltip", astr(act));
            intermediate->set_attribute("shape", "point");
            intermediate->set_attribute("style", "filled");
            intermediate->set_attribute("fillcolor", "black");

            auto& interm_edge = builder.create_edge(*node, *intermediate);
            interm_edge.set_attribute("arrowhead", "none");
            interm_edge.set_attribute("label", label_text);

            for (const auto& [succ_node, prob] : successor_nodes) {
                ss << prob;
                std::string prob_text = ss.str();
                ss.str("");

                auto& edge = builder.create_edge(*intermediate, *succ_node);
                edge.set_attribute("arrowhead", "vee");
                edge.set_attribute("label", prob_text);
            }
        }

        aops.clear();
    } while (!open.empty());

    builder.emit(out);
}

} // namespace probfd::graphviz

#endif // PROBFD_UTILS_GRAPH_VISUALIZATION_H