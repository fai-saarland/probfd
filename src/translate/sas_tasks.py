import itertools
from collections import defaultdict
from collections.abc import Iterator
from fractions import Fraction

SAS_FILE_VERSION = "3"
SAS_FILE_VERSION_PROBABILISTIC = "3P"

DEBUG = False


def _ifnnone(a, b):
    return a if a != None else b


class Impossible(Exception):
    pass


class SimplifiesToTrivial(Exception):
    pass


class _RenamerUtils:
    """
    Adds additional helper functions to the core renamer interface. The renamer objects is required to have the following attributes:

            translate_variable(old_var: int) -> int (or None)
            get_new_variable_domains() -> list[int]

        Will be used to map old variable ids to new ones.

        Additionally, if values are supposed to be pruned / renamed, the following functions must be available:

            TRUE: constant
            FALSE: constant

        Both must compare unequal to all valid variable values.

            translate_value(old_var: int, old_val: int) -> int or TRUE or FALSE
            get_new_domain_size(old_var: int) -> int
    """

    def __init__(self, renamer):
        self.renamer = renamer
        self.TRUE = getattr(renamer, "TRUE", None)
        self.FALSE = getattr(renamer, "FALSE", None)

    def get_new_variable_domains(self):
        return self.renamer.get_new_variable_domains()

    def translate_variable(self, v: int) -> int:
        return self.renamer.translate_variable(v)

    def translate_value(self, v: int, vv: int) -> int:
        try:
            return self.renamer.translate_value(v, vv)
        except AttributeError:
            return vv

    def apply_to_variable_vector(self, old_vec, fn=lambda var_id, elem: elem):
        res = [None] * len(self.renamer.get_new_variable_domains())
        for old_var, entry in enumerate(old_vec):
            new_var = self.translate_variable(old_var)
            if new_var != None:
                res[new_var] = fn(old_var, entry)
        return res

    def apply_to_value_vector(
        self, old_var_id, old_vec, fn=lambda var_id, val_id, elem: elem
    ):
        try:
            res = [None] * self.renamer.get_new_domain_size(old_var_id)
            for old_val, entry in enumerate(old_vec):
                new_val = self.translate_value(old_var_id, old_val)
                if new_val != self.FALSE and new_val != self.TRUE:
                    res[new_val] = fn(old_var_id, old_val, entry)
            return res
        except AttributeError:
            return old_vec

    def translate_pair(self, fact_pair: tuple[int, int]) -> tuple[int, int]:
        (var_no, value) = fact_pair
        new_var_no = self.translate_variable(var_no)
        new_value = self.translate_value(var_no, value)
        return new_var_no, new_value

    def translate_pairs(
        self, pairs: list[tuple[int, int]]
    ) -> Iterator[tuple[int, int]]:
        for pair in pairs:
            new_var_no, new_value = self.translate_pair(pair)
            if new_value is self.FALSE:
                pass
            elif new_value is not self.TRUE:
                assert new_var_no is not None
                yield (new_var_no, new_value)

    def translate_pair_conjunction(
        self, pairs: list[tuple[int, int]]
    ) -> Iterator[tuple[int, int]]:
        for pair in pairs:
            new_var_no, new_value = self.translate_pair(pair)
            if new_value is self.FALSE:
                raise Impossible
            elif new_value is not self.TRUE:
                assert new_var_no is not None
                yield (new_var_no, new_value)


class SASVariables:
    def __init__(self, ranges, axiom_layers, value_names):
        self.ranges = ranges
        self.axiom_layers = axiom_layers
        self.value_names = value_names

    def apply_variable_renaming(self, renaming):
        self.ranges = list(renaming.get_new_variable_domains())
        self.axiom_layers = renaming.apply_to_variable_vector(self.axiom_layers)
        self.value_names = renaming.apply_to_variable_vector(
            self.value_names,
            lambda var_id, value_names: renaming.apply_to_value_vector(
                var_id, value_names
            ),
        )

    def validate(self):
        """Validate variables.

        All variables must have range at least 2, and derived
        variables must have range exactly 2. See comment on derived
        variables in the docstring of SASTask.validate.
        """
        assert len(self.ranges) == len(self.axiom_layers) == len(self.value_names)
        for (var_range, layer, var_value_names) in zip(
            self.ranges, self.axiom_layers, self.value_names
        ):
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
        for var, (rang, axiom_layer) in enumerate(zip(self.ranges, self.axiom_layers)):
            if axiom_layer != -1:
                axiom_str = " [axiom layer %d]" % axiom_layer
            else:
                axiom_str = ""
            print("v%d in {%s}%s" % (var, list(range(rang)), axiom_str))

    def output(self, stream):
        print(len(self.ranges), file=stream)
        for var, (rang, axiom_layer, values) in enumerate(
            zip(self.ranges, self.axiom_layers, self.value_names)
        ):
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
    def __init__(self, facts):
        self.facts = sorted(facts)

    def apply_variable_renaming(self, renaming):
        self.facts = sorted(renaming.translate_pairs(self.facts))
        if len(self.facts) < 2:
            raise SimplifiesToTrivial

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

    def apply_variable_renaming(self, renaming):
        self.values = renaming.apply_to_variable_vector(
            self.values, lambda var_id, value: renaming.translate_value(var_id, value)
        )

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
    def __init__(self, pairs):
        self.pairs = sorted(pairs)

    def apply_variable_renaming(self, renaming):
        self.pairs = sorted(renaming.translate_pair_conjunction(self.pairs))
        if len(self.pairs) == 0:
            raise SimplifiesToTrivial

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


class SASOperatorOutcome:
    def __init__(self, name, prevail, pre_post, reward, prob):
        self.id = None
        self.name = name
        self.prevail = sorted(prevail)
        self.pre_post = self._canonical_pre_post(pre_post)
        self.reward = reward
        self.prob = prob

    def apply_variable_renaming(self, renaming):
        """Compute a new operator from op where the var/value renaming has
        been applied. Return None if op should be pruned (because it
        is always inapplicable or has no effect.)"""

        # We do not call this apply_to_operator, breaking the analogy
        # with the other methods, because it creates a new operator
        # rather than transforming in-place. The reason for this is
        # that it would be quite difficult to generate the operator
        # in-place.

        # This method is trickier than it may at first appear. For
        # example, pre_post values should be fully sorted (see
        # documentation in the sas_tasks module), and pruning effect
        # conditions from a conditional effects can break this sort
        # order. Recreating the operator from scratch solves this
        # because the pre_post entries are sorted by
        # SASOperator.__init__.

        # Also, when we detect a pre_post pair where the effect part
        # can never trigger, the precondition part is still important,
        # but may be demoted to a prevail condition. Whether or not
        # this happens depends on the presence of other pre_post
        # entries for the same variable. We solve this by computing
        # the sorting into prevail vs. preconditions from scratch, too.

        applicability_conditions = self.get_applicability_conditions()
        # Check whether the operator is never applicable.
        applicability_conditions = list(
            renaming.translate_pair_conjunction(applicability_conditions)
        )

        conditions_dict = dict(applicability_conditions)
        new_prevail_vars = set(conditions_dict)

        new_pre_post = []
        for entry in self.pre_post:
            new_entry = self._translate_pre_post(renaming, entry, conditions_dict)
            if new_entry is not None:
                new_pre_post.append(new_entry)
                # Mark the variable in the entry as not prevailed.
                new_var = new_entry[0]
                new_prevail_vars.discard(new_var)

        self.prevail = sorted(
            (var, value)
            for (var, value) in conditions_dict.items()
            if var in new_prevail_vars
        )

        self.pre_post = self._canonical_pre_post(new_pre_post)

    def _translate_pre_post(self, renaming, pre_post_entry, conditions_dict):
        """Return a translated version of a pre_post entry.
        If the entry never causes a value change, return None.

        (It might seem that a possible precondition part of pre_post
        gets lost in this case, but pre_post entries that become
        prevail conditions are handled elsewhere.)

        conditions_dict contains all applicability conditions
        (prevail/pre) of the operator, already converted. This is
        used to detect effect conditions that can never fire.

        The method may assume that the operator remains reachable,
        i.e., that it does not have impossible preconditions, as these
        are already checked elsewhere.

        Possible cases:
        - effect is renaming.TRUE => return None
        - effect equals prevailed value => return None
        - effect condition is impossible given operator applicability
          condition => return None
        - otherwise => return converted pre_post tuple
        """

        var_no, pre, post, cond = pre_post_entry
        new_var_no, new_post = renaming.translate_pair((var_no, post))

        if new_post is renaming.TRUE:
            return None

        if pre == -1:
            new_pre = -1
        else:
            _, new_pre = renaming.translate_pair((var_no, pre))
        assert new_pre is not renaming.FALSE, (
            "This function should only be called for operators "
            "whose applicability conditions are deemed possible."
        )

        if new_post == new_pre:
            return None

        new_cond = list(cond)
        try:
            new_cond = list(renaming.translate_pair_conjunction(new_cond))
        except renaming.Impossible:
            # The effect conditions can never be satisfied.
            return None

        for cond_var, cond_value in new_cond:
            if cond_var in conditions_dict and conditions_dict[cond_var] != cond_value:
                # This effect condition is not compatible with
                # the applicability conditions.
                return None

        assert new_post is not renaming.FALSE, (
            "if we survived so far, this effect can trigger "
            "(as far as our analysis can determine this), "
            "and then new_post cannot be renaming.FALSE"
        )

        assert new_pre is not renaming.TRUE, (
            "if this pre_post changes the value and can fire, "
            "new_pre cannot be renaming.TRUE"
        )

        return new_var_no, new_pre, new_post, new_cond

    def clone(self, name=None, prevail=None, pre_post=None, reward=None, prob=None):
        return SASOperatorOutcome(
            _ifnnone(name, self.name),
            _ifnnone(prevail, self.prevail),
            _ifnnone(pre_post, self.pre_post),
            _ifnnone(reward, self.reward),
            _ifnnone(prob, self.prob),
        )

    def _canonical_pre_post(self, pre_post):
        # Return a sorted and uniquified version of pre_post. We would
        # like to just use sorted(set(pre_post)), but this fails because
        # the effect conditions are a list and hence not hashable.
        def tuplify(entry):
            var, pre, post, cond = entry
            return var, pre, post, tuple(cond)

        def listify(entry):
            var, pre, post, cond = entry
            return var, pre, post, list(cond)

        pre_post = map(tuplify, pre_post)
        pre_post = sorted(set(pre_post))
        pre_post = list(map(listify, pre_post))
        return pre_post

    def validate(self, variables):
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
        10. Costs are non-negative integers.

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
        assert self.pre_post == self._canonical_pre_post(self.pre_post)
        prevail_vars = {var for (var, value) in self.prevail}
        pre_values = {}
        for var, pre, post, cond in self.pre_post:
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
        for var, pre, post, cond in self.pre_post:
            for cvar, cval in cond:
                assert cvar not in pre_values or pre_values[cvar] == -1
                assert cvar not in prevail_vars
        # assert self.pre_post

    def dump(self):
        print(self.name)
        print("Prevail:")
        for var, val in self.prevail:
            print("  v%d: %d" % (var, val))
        print("Pre/Post:")
        for var, pre, post, cond in self.pre_post:
            if cond:
                cond_str = " [%s]" % ", ".join(["%d: %d" % tuple(c) for c in cond])
            else:
                cond_str = ""
            print("  v%d: %d -> %d%s" % (var, pre, post, cond_str))

    def output(self, stream):
        print("begin_operator", file=stream)
        print(self.name[1:-1], file=stream)
        print(len(self.prevail), file=stream)
        for var, val in self.prevail:
            print(var, val, file=stream)
        print(len(self.pre_post), file=stream)
        for var, pre, post, cond in self.pre_post:
            print(len(cond), end=" ", file=stream)
            for cvar, cval in cond:
                print(cvar, cval, end=" ", file=stream)
            print(var, pre, post, file=stream)
        print(self.reward, file=stream)
        print("end_operator", file=stream)

    def get_encoding_size(self):
        size = 1 + len(self.prevail)
        for var, pre, post, cond in self.pre_post:
            size += 1 + len(cond)
            if pre != -1:
                size += 1
        return size

    def get_applicability_conditions(self):
        """Return the combined applicability conditions
        (prevail conditions and preconditions) of the operator.

        Returns a sorted list of (var, value) pairs. This is
        guaranteed to contain at most one fact per variable and
        must hence be non-contradictory."""
        conditions = {}
        for var, val in self.prevail:
            assert var not in conditions
            conditions[var] = val
        for var, pre, post, cond in self.pre_post:
            if pre != -1:
                assert var not in conditions or conditions[var] == pre
                conditions[var] = pre
        return tuple(sorted(conditions.items()))

    def get_effect(self):
        effect = []
        for var, _, post, cond in self.pre_post:
            effect.append((var, post, tuple(cond)))
        return tuple(sorted(effect))

    @staticmethod
    def from_pre_eff(name, pre, eff, reward, prob):
        prevail = {var_id: val for (var_id, val) in pre}
        pre_post = []
        for var_id, post, cond in eff:
            pre = -1
            if var_id in prevail:
                pre = prevail[var_id]
                del prevail[var_id]
            pre_post.append((var_id, pre, post, cond))
        return SASOperatorOutcome(name, list(prevail.items()), pre_post, reward, prob)


class SASPOperator:
    def __init__(self, name: str, outcomes: list[SASOperatorOutcome]):
        self.name = name
        self.outcomes = tuple(outcomes)

    def apply_variable_renaming(self, renaming):
        for out in self.outcomes:
            out.apply_variable_renaming(renaming)
        if not self.canonicalize():
            raise SimplifiesToTrivial

    def canonicalize(self):
        canonicalized = defaultdict(lambda: Fraction(0))
        non_trivial = False
        for out in self.outcomes:
            canonicalized[(out.get_effect(), out.reward)] += out.prob
            if out.pre_post or out.reward > 0:
                non_trivial = True
        if not non_trivial:
            self.outcomes = []
            return False
        else:
            new_outcomes = []
            precond = self.outcomes[0].get_applicability_conditions()
            for ((effect, reward), prob) in canonicalized.items():
                new_outcomes.append(
                    SASOperatorOutcome.from_pre_eff(
                        self.name, precond, effect, reward, prob
                    )
                )
            self.outcomes = new_outcomes
            return True

    def validate(self, variables):
        assert len(self.outcomes) >= 1
        assert sum((o.prob for o in self.outcomes)) == Fraction(1)

    def dump(self):
        print(self.name)
        print("outcomes:", len(self.outcomes))
        for out, prob in self.outcomes:
            print(out, prob)

    def output(self, stream):
        print("begin_probabilistic_operator", file=stream)
        print(self.name[1:-1], file=stream)
        print(len(self.outcomes), file=stream)
        for out in self.outcomes:
            print(out.id, out.prob, file=stream)
        print("end_probabilistic_operator", file=stream)

    def get_encoding_size(self):
        return len(self.outcomes)


class SASAxiom:
    def __init__(self, condition, effect):
        self.condition = sorted(condition)
        self.effect = effect
        assert self.effect[1] in (0, 1)

        for _, val in condition:
            assert val >= 0, condition

    def apply_variable_renaming(self, renaming):
        # The following line may generate an Impossible exception,
        # which is propagated up.
        self.condition = sorted(renaming.translate_pair_conjunction(self.condition))
        new_var, new_value = renaming.translate_pair(self.effect)
        # If the new_value is always false, then the condition must
        # have been impossible.
        assert new_value is not renaming.FALSE
        if new_value is renaming.TRUE:
            raise SimplifiesToTrivial
        self.effect = new_var, new_value

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


class SASTask:
    """Planning task in finite-domain representation.

    The user is responsible for making sure that the data fits a
    number of structural restrictions. For example, conditions should
    generally be sorted and mention each variable at most once. See
    the validate methods for details."""

    def __init__(
        self,
        variables: SASVariables,
        mutexes: list[SASMutexGroup],
        init: SASInit,
        goal: SASGoal,
        operators: list[SASPOperator],
        axioms: list[SASAxiom],
        metric: bool,
    ):
        self._variables = variables
        self._mutexes = mutexes
        self._init = init
        self._goal = goal
        self._outcomes = None
        self.set_operators(operators)
        self._axioms = sorted(axioms, key=lambda axiom: (axiom.condition, axiom.effect))
        self._metric = metric
        self._is_prob = self._has_probabilistic_outcomes()
        if DEBUG:
            self.validate()

    def apply_variable_renaming(self, renaming):
        renaming = _RenamerUtils(renaming)
        self._variables.apply_variable_renaming(renaming)
        self._init.apply_variable_renaming(renaming)
        self._goal.apply_variable_renaming(renaming)
        self._apply_renaming_to_list(renaming, self._mutexes)
        num = self._apply_renaming_to_list(renaming, self._operators)
        print(f"{num} operators removed")
        num = self._apply_renaming_to_list(renaming, self._axioms)
        print(f"{num} axioms removed")
        self._set_outcomes()
        self._is_prob = self._has_probabilistic_outcomes()

    def _apply_renaming_to_list(self, renaming, lst):
        j = 0
        for _, obj in enumerate(lst):
            try:
                obj.apply_variable_renaming(renaming)
                lst[j] = obj
                j += 1
            except (Impossible, SimplifiesToTrivial):
                pass
        removed = len(lst) - j
        lst[:] = lst[:j]
        return removed

    def _set_outcomes(self):
        self._outcomes = list(
            itertools.chain.from_iterable((o.outcomes for o in self._operators))
        )
        for i, o in enumerate(self._outcomes):
            o.id = i

    def _has_probabilistic_outcomes(self):
        return any((len(o.outcomes) > 0 for o in self._operators))

    def get_initial_state(self) -> list[int]:
        return self._init.values

    def get_variable_domains(self) -> list[int]:
        return self._variables.ranges

    def get_axioms(self) -> list[SASAxiom]:
        return self._axioms

    def get_goal(self) -> list[tuple[int, int]]:
        return self._goal.pairs

    def get_outcomes(self) -> list[SASOperatorOutcome]:
        return self._outcomes

    def get_operators(self) -> list[SASPOperator]:
        return self._operators

    def add_variable(self, i, g, axiom_layer, value_names):
        self._variables.ranges.append(len(value_names))
        self._variables.axiom_layers.append(axiom_layer)
        self._variables.value_names.append(value_names)
        self._init.values.append(i)
        if g != None:
            self._goal.append((len(self._variables.ranges) - 1, g))

    def set_operators(self, operators: list[SASPOperator]):
        self._operators = sorted(
            (o for o in operators if o.canonicalize()), key=lambda o: o.name
        )
        self._set_outcomes()

    def set_goal(self, var_id: int, val_id: int):
        for i in range(len(self._goal.pairs)):
            if self._goal.pairs[i][0] == var_id:
                self._goal.pairs[i] = (var_id, val_id)
                return
        self._goal = SASGoal(self._goal.pairs + [(var_id, val_id)])

    def add_operator(self, op: SASPOperator):
        n = len(self._outcomes)
        self._operators.append(op)
        self._outcomes.extend(op.outcomes)
        for i in range(n, len(self._outcomes)):
            self._outcomes[i].id = i
        self._is_prob = self._is_prob or len(op.outcomes) > 1

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
        for mutex in self._mutexes:
            mutex.validate(self._variables)
        self._init.validate(self._variables)
        self._goal.validate(self._variables)
        for op in self._operators:
            op.validate(self.variables)
        for axiom in self._axioms:
            axiom.validate(self._variables, self._init)
        assert self._metric is False or self._metric is True, self._metric

    def statistics(self):
        print("Translator variables: %d" % len(self._variables.ranges))
        print(
            "Translator derived variables: %d"
            % len([layer for layer in self._variables.axiom_layers if layer >= 0])
        )
        print("Translator facts: %d" % sum(self._variables.ranges))
        print("Translator goal facts: %d" % len(self._goal.pairs))
        print("Translator mutex groups: %d" % len(self._mutexes))
        print(
            "Translator total mutex groups size: %d"
            % sum(mutex.get_encoding_size() for mutex in self._mutexes)
        )
        print("Translator operators: %d" % len(self._operators))
        print("Translator outcomes: %d" % len(self._outcomes))
        print("Translator axioms: %d" % len(self._axioms))
        print("Translator task size: %d" % self.get_encoding_size())

    def dump(self):
        print("variables:")
        self._variables.dump()
        print("%d mutex groups:" % len(self._mutexes))
        for mutex in self._mutexes:
            print("group:")
            mutex.dump()
        print("init:")
        self._init.dump()
        print("goal:")
        self._goal.dump()
        print("%d outcomes:" % len(self._outcomes))
        for operator in self._outcomes:
            operator.dump()
        print("%d axioms:" % len(self._axioms))
        for axiom in self._axioms:
            axiom.dump()
        print("%d probabilistic operators:" % len(self._operators))
        for op in self._operators:
            op.dump()
        print("metric: %s" % self.metric)

    def output(self, stream):
        print("begin_version", file=stream)
        if self._is_prob:
            print(SAS_FILE_VERSION_PROBABILISTIC, file=stream)
        else:
            print(SAS_FILE_VERSION, file=stream)
        print("end_version", file=stream)
        print("begin_metric", file=stream)
        print(int(self._metric), file=stream)
        print("end_metric", file=stream)
        self._variables.output(stream)
        print(len(self._mutexes), file=stream)
        for mutex in self._mutexes:
            mutex.output(stream)
        self._init.output(stream)
        self._goal.output(stream)
        print(len(self._outcomes), file=stream)
        for op in self._outcomes:
            op.output(stream)
        print(len(self._axioms), file=stream)
        for axiom in self._axioms:
            axiom.output(stream)
        if self._operators:
            print(len(self._operators), file=stream)
            for op in self._operators:
                op.output(stream)

    def get_encoding_size(self):
        task_size = 0
        task_size += self._variables.get_encoding_size()
        for mutex in self._mutexes:
            task_size += mutex.get_encoding_size()
        task_size += self._goal.get_encoding_size()
        for op in self._outcomes:
            task_size += op.get_encoding_size()
        for axiom in self._axioms:
            task_size += axiom.get_encoding_size()
        for op in self._operators:
            task_size += op.get_encoding_size()
        return task_size
