"""This module contains a function for simplifying tasks in
finite-domain representation (SASTask). Usage:

    simplify.filter_unreachable_propositions(sas_task)

simplifies `sas_task` in-place. If simplification detects that the
task is unsolvable, the function raises `simplify.Impossible`. If it
detects that is has an empty goal, the function raises
`simplify.TriviallySolvable`.

The simplification procedure generates DTGs for the task and then
removes facts that are unreachable from the initial state in a DTG.
Note that such unreachable facts can exist even though we perform a
relaxed reachability analysis before grounding (and DTG reachability
is weaker than relaxed reachability) because the previous relaxed
reachability does not take into account any mutex information, while
PDDL-to-SAS conversion gets rid of certain operators that cannot be
applicable given the mutex information.

Despite the name, the method touches more than the set of facts. For
example, operators that have preconditions on pruned facts are
removed, too. (See also the docstring of
filter_unreachable_propositions.)
"""

from collections import defaultdict
from itertools import count

import sas_tasks

DEBUG = False

# TODO:
# This is all quite hackish and would be easier if the translator were
# restructured so that more information is immediately available for
# the propositions, and if propositions had more structure. Directly
# working with int pairs is awkward.


class DomainTransitionGraph:
    """Domain transition graphs.

    Attributes:
    - init (int): the initial state value of the DTG variable
    - size (int): the number of values in the domain
    - arcs (defaultdict: int -> set(int)): the DTG arcs (unlabeled)

    There are no transition labels or goal values.

    The intention is that nodes are represented as ints in {1, ...,
    domain_size}, but this is not enforced.

    For derived variables, the "fallback value" that is produced by
    negation by failure should be used for `init`, so that it is
    always considered reachable.
    """

    def __init__(self, init, size):
        """Create a DTG with no arcs."""
        self.init = init
        self.size = size
        self.arcs = defaultdict(set)

    def add_arc(self, u, v):
        """Add an arc from u to v."""
        self.arcs[u].add(v)

    def reachable(self):
        """Return the values reachable from the initial value.
        Represented as a set(int)."""
        queue = [self.init]
        reachable = set(queue)
        while queue:
            node = queue.pop()
            new_neighbors = self.arcs.get(node, set()) - reachable
            reachable |= new_neighbors
            queue.extend(new_neighbors)
        return reachable

    def dump(self):
        """Dump the DTG."""
        print("DTG size:", self.size)
        print("DTG init value:", self.init)
        print("DTG arcs:")
        for source, destinations in sorted(self.arcs.items()):
            for destination in sorted(destinations):
                print("  %d => %d" % (source, destination))


def build_dtgs(task):
    """Build DTGs for all variables of the SASTask `task`.
    Return a list(DomainTransitionGraph), one for each variable.

    For derived variables, we do not consider the axiom bodies, i.e.,
    we treat each axiom as if it were an operator with no
    preconditions. In the case where the only derived variables used
    are binary and all rules change the value from the default value
    to the non-default value, this results in the correct DTG.
    Otherwise, at worst it results in an overapproximation, which
    would not threaten correctness."""

    init_vals = task.get_initial_state()
    sizes = task.get_variable_domains()
    dtgs = [DomainTransitionGraph(init, size) for (init, size) in zip(init_vals, sizes)]

    def add_arc(var_no, pre_spec, post):
        """Add a DTG arc for var_no induced by transition pre_spec -> post.
        pre_spec may be -1, in which case arcs from every value
        other than post are added."""
        if pre_spec == -1:
            pre_values = set(range(sizes[var_no])).difference([post])
        else:
            pre_values = [pre_spec]
        for pre in pre_values:
            dtgs[var_no].add_arc(pre, post)

    def get_effective_pre(var_no, conditions, effect_conditions):
        """Return combined information on the conditions on `var_no`
        from operator conditions and effect conditions.

        - conditions: dict(int -> int) containing the combined
          operator prevail and preconditions
        - effect_conditions: list(pair(int, int)) containing the
          effect conditions

        Result:
        - -1   if there is no condition on var_no
        - val  if there is a unique condition var_no=val
        - None if there are contradictory conditions on var_no"""

        result = conditions.get(var_no, -1)
        for cond_var_no, cond_val in effect_conditions:
            if cond_var_no == var_no:
                if result == -1:
                    # This is the first condition on var_no.
                    result = cond_val
                elif cond_val != result:
                    # We have contradictory conditions on var_no.
                    return None
        return result

    for op in task.get_outcomes():
        conditions = dict(op.get_applicability_conditions())
        for var_no, _, post, cond in op.pre_post:
            effective_pre = get_effective_pre(var_no, conditions, cond)
            if effective_pre is not None:
                add_arc(var_no, effective_pre, post)

    for axiom in task.get_axioms():
        var_no, val = axiom.effect
        add_arc(var_no, -1, val)

    return dtgs


class VarValueRenaming:

    FALSE = object()
    TRUE = object()

    def __init__(self):
        self.new_var_nos = []  # indexed by old var_no
        self.new_values = []  # indexed by old var_no and old value
        self.new_sizes = []  # indexed by new var_no
        self.new_var_count = 0
        self.num_removed_values = 0

    def dump(self):
        old_var_count = len(self.new_var_nos)
        print("variable count: %d => %d" % (old_var_count, self.new_var_count))
        print("number of removed values: %d" % self.num_removed_values)
        print("variable conversions:")
        for old_var_no, (new_var_no, new_values) in enumerate(
            zip(self.new_var_nos, self.new_values)
        ):
            old_size = len(new_values)
            if new_var_no is None:
                print("variable %d [size %d] => removed" % (old_var_no, old_size))
            else:
                new_size = self.new_sizes[new_var_no]
                print(
                    "variable %d [size %d] => %d [size %d]"
                    % (old_var_no, old_size, new_var_no, new_size)
                )
            for old_value, new_value in enumerate(new_values):
                if new_value is VarValueRenaming.FALSE:
                    new_value = "always false"
                elif new_value is VarValueRenaming.TRUE:
                    new_value = "always true"
                print("    value %d => %s" % (old_value, new_value))

    def register_variable(self, old_domain_size, init_value, new_domain):
        assert 1 <= len(new_domain) <= old_domain_size
        assert init_value in new_domain
        if len(new_domain) == 1:
            # Remove this variable completely.
            new_values_for_var = [VarValueRenaming.FALSE] * old_domain_size
            new_values_for_var[init_value] = VarValueRenaming.TRUE
            self.new_var_nos.append(None)
            self.new_values.append(new_values_for_var)
            self.num_removed_values += old_domain_size
        else:
            new_value_counter = count()
            new_values_for_var = []
            for value in range(old_domain_size):
                if value in new_domain:
                    new_values_for_var.append(next(new_value_counter))
                else:
                    self.num_removed_values += 1
                    new_values_for_var.append(VarValueRenaming.FALSE)
            new_size = next(new_value_counter)
            assert new_size == len(new_domain)

            self.new_var_nos.append(self.new_var_count)
            self.new_values.append(new_values_for_var)
            self.new_sizes.append(new_size)
            self.new_var_count += 1

    def get_new_variable_domains(self):
        return self.new_sizes

    def get_new_domain_size(self, old_var: int):
        return self.new_sizes[self.new_var_nos[old_var]]

    def translate_variable(self, old_var: int):
        return self.new_var_nos[old_var]

    def translate_value(self, old_var: int, old_val: int):
        return self.new_values[old_var][old_val]


def build_renaming(dtgs):
    renaming = VarValueRenaming()
    for dtg in dtgs:
        renaming.register_variable(dtg.size, dtg.init, dtg.reachable())
    return renaming


def filter_unreachable_propositions(sas_task):
    """We remove unreachable propositions and then prune variables
    with only one value.

    Examples of things that are pruned:
    - Constant propositions that are not detected in instantiate.py
      because instantiate.py only reasons at the predicate level, and some
      predicates such as "at" in Depot are constant for some objects
      (hoists), but not others (trucks).

      Example: "at(hoist1, distributor0)" and the associated variable
      in depots-01.

    - "none of those" values that are unreachable.

      Example: at(truck1, ?x) = <none of those> in depots-01.

    - Certain values that are relaxed reachable but detected as
      unreachable after SAS instantiation because the only operators
      that set them have inconsistent preconditions.

      Example: on(crate0, crate0) in depots-01.
    """

    if DEBUG:
        sas_task.validate()
    dtgs = build_dtgs(sas_task)
    renaming = build_renaming(dtgs)
    # apply_to_task may raise Impossible if the goal is detected as
    # unreachable or TriviallySolvable if it has no goal. We let the
    # exceptions propagate to the caller.
    sas_task.apply_variable_renaming(renaming)
    print("%d propositions removed" % renaming.num_removed_values)
    if DEBUG:
        sas_task.validate()
