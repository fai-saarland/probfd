import heapq
from collections import defaultdict, deque
from itertools import chain

import sccs

DEBUG = False


class CausalGraph:
    """Weighted causal graph used for defining a variable order.

    The causal graph only contains pre->eff edges (in contrast to the
    variant that also has eff<->eff edges).

    The variable order is defined such that removing all edges v->v'
    with v>v' induces an acyclic subgraph of the causal graph. This
    corresponds to the pruning of the causal graph as described in the
    JAIR 2006 Fast Downward paper for the causal graph heuristic. The
    greedy method is based on weighting the edges of the causal graph.
    In this implementation these weights slightly differ from the
    description in the JAIR paper to reproduce the behaviour of the
    original implementation in the preprocessor component of the
    planner.
    """

    def __init__(self, sas_task):
        self.weighted_graph = defaultdict(lambda: defaultdict(int))
        ## var_no -> (var_no -> number)
        self.predecessor_graph = defaultdict(set)
        self.ordering = []

        self.weight_graph_from_ops(sas_task.get_outcomes())
        self.weight_graph_from_axioms(sas_task.get_axioms())

        self.num_variables = len(sas_task.get_variable_domains())
        self.goal_map = dict(sas_task.get_goal())

    def get_ordering(self):
        if not self.ordering:
            sccs = self.get_strongly_connected_components()
            self.calculate_topological_pseudo_sort(sccs)
        return self.ordering

    def weight_graph_from_ops(self, operators):
        ### A source variable can be processed several times. This was
        ### probably not intended originally but in experiments (cf.
        ### issue26) it performed better than the (clearer) weighting
        ### described in the Fast Downward paper (which would require
        ### a more complicated implementation).
        for op in operators:
            source_vars = [var for (var, value) in op.prevail]
            for var, pre, _, _ in op.pre_post:
                if pre != -1:
                    source_vars.append(var)

            for target, _, _, cond in op.pre_post:
                for source in chain(source_vars, (var for var, _ in cond)):
                    if source != target:
                        self.weighted_graph[source][target] += 1
                        self.predecessor_graph[target].add(source)

    def weight_graph_from_axioms(self, axioms):
        for ax in axioms:
            target = ax.effect[0]
            for source, _ in ax.condition:
                if source != target:
                    self.weighted_graph[source][target] += 1
                    self.predecessor_graph[target].add(source)

    def get_strongly_connected_components(self):
        unweighted_graph = [[] for _ in range(self.num_variables)]
        assert len(self.weighted_graph) <= self.num_variables
        for source, target_weights in self.weighted_graph.items():
            unweighted_graph[source] = sorted(target_weights.keys())
        return sccs.get_sccs_adjacency_list(unweighted_graph)

    def calculate_topological_pseudo_sort(self, sccs):
        for scc in sccs:
            if len(scc) > 1:
                # component needs to be turned into acyclic subgraph

                # Compute subgraph induced by scc
                subgraph = defaultdict(list)
                for var in scc:
                    # for each variable in component only list edges inside
                    # component.
                    subgraph_edges = subgraph[var]
                    for target, cost in sorted(self.weighted_graph[var].items()):
                        if target in scc:
                            if target in self.goal_map:
                                subgraph_edges.append((target, 100000 + cost))
                            subgraph_edges.append((target, cost))

                self.ordering.extend(MaxDAG(subgraph, scc).get_result())
            else:
                self.ordering.append(scc[0])

    def calculate_important_vars(self, goal):
        # Note for future refactoring: it is perhaps more idiomatic
        # and efficient to use a set rather than a defaultdict(bool).
        necessary = defaultdict(bool)
        for var, _ in goal:
            if not necessary[var]:
                necessary[var] = True
                self.dfs(var, necessary)
        return necessary

    def dfs(self, node, necessary):
        stack = [pred for pred in self.predecessor_graph[node]]
        while stack:
            n = stack.pop()
            if not necessary[n]:
                necessary[n] = True
                stack.extend(pred for pred in self.predecessor_graph[n])


class MaxDAG:
    """Defines a variable ordering for a SCC of the (weighted) causal
    graph.

    Conceptually, the greedy algorithm successively picks a node with
    minimal cummulated weight of incoming arcs and removes its
    incident edges from the graph until only a single node remains
    (cf. computation of total order of vertices when pruning the
    causal graph in the Fast Downward JAIR 2006 paper).
    """

    def __init__(self, graph, input_order):
        self.weighted_graph = graph
        # input_order is only used to get the same tie-breaking as
        # with the old preprocessor
        self.input_order = input_order

    def get_result(self):
        incoming_weights = defaultdict(int)
        for weighted_edges in self.weighted_graph.values():
            for target, weight in weighted_edges:
                incoming_weights[target] += weight

        weight_to_nodes = defaultdict(deque)
        for node in self.input_order:
            weight = incoming_weights[node]
            weight_to_nodes[weight].append(node)
        weights = list(weight_to_nodes.keys())
        heapq.heapify(weights)

        done = set()
        result = []
        while weights:
            min_key = weights[0]
            min_elem = None
            entries = weight_to_nodes[min_key]
            while entries and (
                min_elem is None
                or min_elem in done
                or min_key > incoming_weights[min_elem]
            ):
                min_elem = entries.popleft()
            if not entries:
                del weight_to_nodes[min_key]
                heapq.heappop(weights)  # remove min_key from heap
            if min_elem is None or min_elem in done:
                # since we use lazy deletion from the heap weights,
                # there can be weights with a "done" entry in
                # weight_to_nodes
                continue
            done.add(min_elem)
            result.append(min_elem)
            for target, weight in self.weighted_graph[min_elem]:
                if target not in done:
                    weight = weight % 100000
                    if weight == 0:
                        continue
                    old_in_weight = incoming_weights[target]
                    new_in_weight = old_in_weight - weight
                    incoming_weights[target] = new_in_weight

                    # add new entry to weight_to_nodes
                    if new_in_weight not in weight_to_nodes:
                        heapq.heappush(weights, new_in_weight)
                    weight_to_nodes[new_in_weight].append(target)
        return result


class VariableOrder:
    FALSE = object()
    TRUE = object()

    """Apply a given variable order to a SAS task."""

    def __init__(self, ordering, domains):
        """Ordering is a list of variable numbers in the desired order.

        If a variable does not occur in the ordering, it is removed
        from the task.
        """
        self.ordering = ordering
        self.new_var = {v: i for i, v in enumerate(ordering)}
        self.domains = [domains[v] for v in self.ordering]

    def get_new_variable_domains(self):
        return self.domains

    def translate_variable(self, old_var: int):
        return self.new_var[old_var]

    def translate_value(self, old_var: int, old_val: int):
        return old_val if self.new_var[old_var] != None else VariableOrder.TRUE


def find_and_apply_variable_order(
    sas_task, reorder_vars=True, filter_unimportant_vars=True
):
    if reorder_vars or filter_unimportant_vars:
        cg = CausalGraph(sas_task)
        if reorder_vars:
            order = cg.get_ordering()
        else:
            order = list(range(len(sas_task.variables.ranges)))
        if filter_unimportant_vars:
            necessary = cg.calculate_important_vars(sas_task.get_goal())
            print("%s of %s variables necessary." % (len(necessary), len(order)))
            order = [var for var in order if necessary[var]]
        sas_task.apply_variable_renaming(
            VariableOrder(order, sas_task.get_variable_domains())
        )
