import itertools
from fractions import Fraction
from typing import List, Tuple

from pddl import Metric

SAS_FILE_VERSION = "1"

DEBUG = False

VarValPair = Tuple[int, int]


class SASTask:
    """Probabilistic planning task in finite-domain representation.

    The user is responsible for making sure that the data fits a
    number of structural restrictions. For example, conditions should
    generally be sorted and mention each variable at most once. See
    the validate methods for details."""

    def __init__(self,
                 variables: "SASVariables",
                 mutexes: List["SASMutexGroup"],
                 init: "SASInit",
                 goal: "SASGoal",
                 operators: List["SASOperator"],
                 axioms: List["SASAxiom"],
                 metric: Metric,
                 rewards: bool) -> None:
        self.variables = variables
        self.mutexes = mutexes
        self.init = init
        self.goal = goal
        self.operators = sorted(operators, key=lambda op: (
            op.name, op.outcomes))
        self.axioms = sorted(axioms, key=lambda axiom: (
            axiom.condition, axiom.effect))
        self.metric = metric
        self.rewards = rewards
        if DEBUG:
            self.validate()

    def validate(self):
        """Fail an assertion if the task is invalid.

        A task is valid if all its components are valid. Valid tasks
        are almost in a kind of "canonical form", but not quite. For
        example, operators and axioms are permitted to be listed in
        any order, even though it would be possible to require some
        kind of canonical sorting.

        Note that we require that all derived variables are binary.
        This is stricter than what later parts of the planner are
        supposed to handle, but some parts of the translator rely on
        this. We might want to consider making this a general
        requirement throughout the planner.

        Note also that there is *no* general rule on what the init (=
        fallback) value of a derived variable is. For example, in
        PSR-Large #1, it can be either 0 or 1. While it is "usually"
        1, code should not rely on this.
        """
        self.variables.validate()
        for mutex in self.mutexes:
            mutex.validate(self.variables)
        self.init.validate(self.variables)
        self.goal.validate(self.variables)
        for op in self.operators:
            op.validate(self.variables)
        for axiom in self.axioms:
            axiom.validate(self.variables, self.init)
        assert self.metric in [Metric.NONE, Metric.MINIMIZE, Metric.MAXIMIZE]

    def dump(self):
        print("variables:")
        self.variables.dump()
        print("%d mutex groups:" % len(self.mutexes))
        for mutex in self.mutexes:
            print("group:")
            mutex.dump()
        print("init:")
        self.init.dump()
        print("goal:")
        self.goal.dump()
        print("%d operators:" % len(self.operators))
        for operator in self.operators:
            operator.dump()
        print("%d axioms:" % len(self.axioms))
        for axiom in self.axioms:
            axiom.dump()
        print("metric: %s" % self.metric)

    def output(self, stream):
        print("begin_version", file=stream)
        print(SAS_FILE_VERSION, file=stream)
        print("end_version", file=stream)
        print("begin_metric", file=stream)
        print(int(self.metric), file=stream)
        print(int(self.rewards), file=stream)
        print("end_metric", file=stream)
        self.variables.output(stream)
        print(len(self.mutexes), file=stream)
        for mutex in self.mutexes:
            mutex.output(stream)
        self.init.output(stream)
        self.goal.output(stream)
        print(len(self.operators), file=stream)
        for op in self.operators:
            op.output(stream)
        print(len(self.axioms), file=stream)
        for axiom in self.axioms:
            axiom.output(stream)

    def get_encoding_size(self):
        task_size = 0
        task_size += self.variables.get_encoding_size()
        for mutex in self.mutexes:
            task_size += mutex.get_encoding_size()
        task_size += self.goal.get_encoding_size()
        for op in self.operators:
            task_size += op.get_encoding_size()
        for axiom in self.axioms:
            task_size += axiom.get_encoding_size()
        return task_size


class SASVariables:
    def __init__(self, ranges: List[int], axiom_layers: List[int],
                 value_names: List[List[str]]) -> None:
        self.ranges = ranges
        self.axiom_layers = axiom_layers
        self.value_names = value_names

    def validate(self):
        """Validate variables.

        All variables must have range at least 2, and derived
        variables must have range exactly 2. See comment on derived
        variables in the docstring of SASTask.validate.
        """
        assert len(self.ranges) == len(self.axiom_layers) == len(
            self.value_names)
        for (var_range, layer, var_value_names) in zip(
                self.ranges, self.axiom_layers, self.value_names):
            assert var_range == len(var_value_names)
            assert var_range >= 2
            assert layer == -1 or layer >= 0
            if layer != -1:
                assert var_range == 2

    def validate_fact(self, fact):
        """Assert that fact is a valid (var, value) pair."""
        var, value = fact
        assert 0 <= var < len(self.ranges)
        assert 0 <= value < self.ranges[var]

    def validate_condition(self, condition):
        """Assert that the condition (list of facts) is sorted, mentions each
        variable at most once, and only consists of valid facts."""
        last_var = -1
        for (var, value) in condition:
            self.validate_fact((var, value))
            assert var > last_var
            last_var = var

    def dump(self):
        for var, (rang, axiom_layer) in enumerate(
                zip(self.ranges, self.axiom_layers)):
            if axiom_layer != -1:
                axiom_str = " [axiom layer %d]" % axiom_layer
            else:
                axiom_str = ""
            print("v%d in {%s}%s" % (var, list(range(rang)), axiom_str))

    def output(self, stream):
        print(len(self.ranges), file=stream)
        for var, (rang, axiom_layer, values) in enumerate(zip(
                self.ranges, self.axiom_layers, self.value_names)):
            print("begin_variable", file=stream)
            print("var%d" % var, file=stream)
            print(axiom_layer, file=stream)
            print(rang, file=stream)
            assert rang == len(values), (rang, values)
            for value in values:
                print(value, file=stream)
            print("end_variable", file=stream)

    def get_encoding_size(self):
        # A variable with range k has encoding size k + 1 to also give the
        # variable itself some weight.
        return len(self.ranges) + sum(self.ranges)


class SASMutexGroup:
    def __init__(self, facts: List[VarValPair]):
        self.facts = sorted(facts)

    def validate(self, variables):
        """Assert that the facts in the mutex group are sorted and unique
        and that they are all valid."""
        for fact in self.facts:
            variables.validate_fact(fact)
        assert self.facts == sorted(set(self.facts))

    def dump(self):
        for var, val in self.facts:
            print("v%d: %d" % (var, val))

    def output(self, stream):
        print("begin_mutex_group", file=stream)
        print(len(self.facts), file=stream)
        for var, val in self.facts:
            print(var, val, file=stream)
        print("end_mutex_group", file=stream)

    def get_encoding_size(self):
        return len(self.facts)


class SASInit:
    def __init__(self, values):
        self.values = values

    def validate(self, variables):
        """Validate initial state.

        Assert that the initial state contains the correct number of
        values and that all values are in range.
        """

        assert len(self.values) == len(variables.ranges)
        for fact in enumerate(self.values):
            variables.validate_fact(fact)

    def dump(self):
        for var, val in enumerate(self.values):
            print("v%d: %d" % (var, val))

    def output(self, stream):
        print("begin_state", file=stream)
        for val in self.values:
            print(val, file=stream)
        print("end_state", file=stream)


class SASGoal:
    def __init__(self, pairs: List[Tuple[int, int]]) -> None:
        self.pairs = sorted(pairs)

    def validate(self, variables):
        """Assert that the goal is nonempty and a valid condition."""
        assert self.pairs
        variables.validate_condition(self.pairs)

    def dump(self):
        for var, val in self.pairs:
            print("v%d: %d" % (var, val))

    def output(self, stream):
        print("begin_goal", file=stream)
        print(len(self.pairs), file=stream)
        for var, val in self.pairs:
            print(var, val, file=stream)
        print("end_goal", file=stream)

    def get_encoding_size(self):
        return len(self.pairs)


class SASOutcome:
    def __init__(self,
                 probability: Fraction,
                 prevail: List[VarValPair],
                 cond_eff: List[Tuple[VarValPair, List[VarValPair]]]) -> None:
        self.probability = probability
        self.prevail = sorted(prevail)
        self.cond_eff = self._canonical_cond_eff(cond_eff)

    def _canonical_cond_eff(self, cond_eff):
        # Return a sorted and uniquified version of pre_post. We would
        # like to just use sorted(set(pre_post)), but this fails because
        # the effect conditions are a list and hence not hashable.
        def tuplify(entry):
            (var, post), cond = entry
            return var, post, tuple(cond)

        def listify(entry):
            var, post, cond = entry
            return (var, post), list(cond)

        cond_eff = map(tuplify, cond_eff)
        cond_eff = sorted(set(cond_eff))
        cond_eff = list(map(listify, cond_eff))
        return cond_eff

    def __le__(self, other):
        return (self.probability, self.prevail, self.cond_eff) < (
            other.probability, other.prevail, other.cond_eff)

    def validate(self, variables, precondition):
        """Validate the operator.

        Assert that
        1. Prevail conditions are valid conditions (i.e., sorted and
           all referring to different variables)
        2. The pre_post list is sorted by (var, pre, post, cond), and the
           same (var, pre, post, cond) 4-tuple is not repeated.
        3. Effect conditions are valid conditions and do not contain variables
           from the pre- or prevail conditions.
        4. Variables occurring in pre_post rules do not have a prevail
           condition.
        5. Preconditions in pre_post are -1 or valid facts.
        6. Effects are valid facts.
        7. Effect variables are non-derived.
        8. If a variable has multiple pre_post rules, then pre is
           identical in all these rules.
        9. There is at least one effect.

        Odd things that are *not* illegal:
        - The effect in a pre_post rule may be identical to the
          precondition or to an effect condition of that effect.

        TODO/open question:
        - It is currently not very clear what the semantics of operators
          should be when effects "conflict", i.e., when multiple effects
          trigger and want to set a given variable to two different
          values. In the case where both are unconditional effects, we
          should make sure that our representation doesn't actually
          contain two such effects, but when at least one of them is
          conditional, things are not so easy.

          To make our life simpler when generating SAS+ tasks from
          PDDL tasks, it probably makes most sense to generalize the
          PDDL rule in this case: there is a value order where certain
          values "win" over others in this situation. It probably
          makes sense to say the "highest" values should win in this
          case, because that's consistent with the PDDL rules if we
          say false = 0 and true = 1, and also with our sort order of
          effects it means we get the right result if we just apply
          effects in sequence.

          But whatever we end up deciding, we need to be clear about it,
          document it and make sure that all of our code knows the rules
          and follows them.
        """

        variables.validate_condition(self.prevail)
        assert self.cond_eff == self._canonical_cond_eff(self.cond_eff)
        prevail_vars = {var for (var, value) in self.prevail}
        pre_values = {}
        for (var, post), cond in self.cond_eff:
            pre = precondition[var]
            variables.validate_condition(cond)
            assert var not in prevail_vars
            if pre != -1:
                variables.validate_fact((var, pre))
            variables.validate_fact((var, post))
            assert variables.axiom_layers[var] == -1
            if var in pre_values:
                assert pre_values[var] == pre
            else:
                pre_values[var] = pre
        for _, cond in self.cond_eff:
            for cvar, cval in cond:
                assert cvar not in pre_values or pre_values[cvar] == -1
                assert cvar not in prevail_vars
        assert self.cond_eff

    def get_encoding_size(self):
        size = 1 + len(self.prevail)
        for _, cond in self.cond_eff:
            size += 1 + len(cond)
        return size

    def dump(self):
        print(f"Probability: {self.probability}")
        print("  Prevail:")
        for var in self.prevail:
            print("    v%d" % var)
        print("  Conditional Effects:")
        for (var, post), cond in self.cond_eff:
            if cond:
                cond_str = " [%s]" % ", ".join(
                    ["%d: %d" % tuple(c) for c in cond])
            else:
                cond_str = ""
            print("    v%d -> %d%s" % (var, post, cond_str))

    def output(self, stream):
        print(self.probability, file=stream)
        # print(len(self.prevail), file=stream)
        # for var in self.prevail:
        #    print(var, file=stream)
        print(len(self.cond_eff), file=stream)
        for (var, post), cond in self.cond_eff:
            print(len(cond), end=' ', file=stream)
            for cvar, cval in cond:
                print(cvar, cval, end=' ', file=stream)
            print(var, post, file=stream)


class SASOperator:
    def __init__(self, name: str, precondition: List[VarValPair],
                 outcomes: List[SASOutcome], cost: Fraction) -> None:
        self.name = name
        self.precondition = sorted(precondition)
        self.outcomes = outcomes
        self.cost = cost

    def validate(self, variables):
        """Validate the operator.

        Assert that
        1. Precondition makes sense.
        2. Probabilities sum up to one and are non-negative.
        3. Costs are non-negative.
        4. Every outcome is valid w.r.t. the precondition.
        """

        variables.validate_condition(self.precondition)

        assert self.outcomes

        assert 0 < self.outcomes[0].probability <= 1
        total_prob = self.outcomes[0].probability

        for outcome in itertools.islice(self.outcomes, 1):
            assert 0 < outcome.probability <= 1
            total_prob += outcome.probability

        assert total_prob == 1

        assert self.cost >= 0

        for outcome in self.outcomes:
            outcome.validate(variables, self.precondition)

    def dump(self):
        print(self.name)
        print("Precondition:")
        for var, val in self.precondition:
            print(f"  v{var} = {val}")
        for outcome in self.outcomes:
            outcome.dump()

    def output(self, stream):
        print("begin_operator", file=stream)
        print(self.name[1:-1], file=stream)
        print(len(self.precondition), file=stream)
        for var, val in self.precondition:
            print(f"{var} {val}", file=stream)
        print(len(self.outcomes), file=stream)
        for outcome in self.outcomes:
            outcome.output(stream)
        print(self.cost, file=stream)
        print("end_operator", file=stream)

    def get_encoding_size(self):
        return 1 + sum(map(lambda obj: obj.get_encoding_size(), self.outcomes))


class SASAxiom:
    def __init__(self, condition: List[VarValPair], effect: VarValPair) -> None:
        self.condition = sorted(condition)
        self.effect = effect
        assert self.effect[1] in (0, 1)

        for _, val in condition:
            assert val >= 0, condition

    def validate(self, variables, init):

        """Validate the axiom.

        Assert that the axiom condition is a valid condition, that the
        effect is a valid fact, that the effect variable is a derived
        variable, and that the layering condition is satisfied.

        See the docstring of SASTask.validate for information on the
        restriction on derived variables. The layering condition boils
        down to:

        1. Axioms always set the "non-init" value of the derived
           variable.
        2. Derived variables in the condition must have a lower of
           equal layer to derived variables appearing in the effect.
        3. Conditions with equal layer are only allowed when the
           condition uses the "non-init" value of that variable.

        TODO/bug: rule #1 is currently disabled because we currently
        have axioms that violate it. This is likely due to the
        "extended domain transition graphs" described in the Fast
        Downward paper, Section 5.1. However, we want to eventually
        changes this. See issue454. For cases where rule #1 is violated,
        "non-init" should be "init" in rule #3.
        """

        variables.validate_condition(self.condition)
        variables.validate_fact(self.effect)
        eff_var, eff_value = self.effect
        eff_layer = variables.axiom_layers[eff_var]
        assert eff_layer >= 0
        eff_init_value = init.values[eff_var]
        ## The following rule is currently commented out because of
        ## the TODO/bug mentioned in the docstring.
        # assert eff_value != eff_init_value
        for cond_var, cond_value in self.condition:
            cond_layer = variables.axiom_layers[cond_var]
            if cond_layer != -1:
                assert cond_layer <= eff_layer
                if cond_layer == eff_layer:
                    cond_init_value = init.values[cond_var]
                    ## Once the TODO/bug above is addressed, the
                    ## following four lines can be simplified because
                    ## we are guaranteed to land in the "if" branch.
                    if eff_value != eff_init_value:
                        assert cond_value != cond_init_value
                    else:
                        assert cond_value == cond_init_value

    def dump(self):
        print("Condition:")
        for var, val in self.condition:
            print("  v%d: %d" % (var, val))
        print("Effect:")
        var, val = self.effect
        print("  v%d: %d" % (var, val))

    def output(self, stream):
        print("begin_rule", file=stream)
        print(len(self.condition), file=stream)
        for var, val in self.condition:
            print(var, val, file=stream)
        var, val = self.effect
        print(var, 1 - val, val, file=stream)
        print("end_rule", file=stream)

    def get_encoding_size(self):
        return 1 + len(self.condition)
