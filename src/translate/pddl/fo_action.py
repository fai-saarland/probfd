import itertools
from fractions import Fraction

from . import actions, conditions, f_expression


class Effect:
    def __init__(self, parameters, condition, literal):
        self.parameters = parameters
        self.condition = condition
        self.literal = literal

    def __eq__(self, other):
        return (
            self.__class__ is other.__class__
            and self.parameters == other.parameters
            and self.condition == other.condition
            and self.literal == other.literal
        )

    def __hash__(self):
        return hash((tuple(self.parameters), self.condition, self.literal))

    def dump(self):
        indent = "  "
        if self.parameters:
            print("%sforall %s" % (indent, ", ".join(map(str, self.parameters))))
            indent += "  "
        if self.condition != conditions.Truth():
            print("%sif" % indent)
            self.condition.dump(indent + "  ")
            print("%sthen" % indent)
            indent += "  "
        print("%s%s" % (indent, self.literal))

    def copy(self):
        return Effect(self.parameters, self.condition, self.literal)

    def uniquify_variables(self, type_map):
        renamings = {}
        self.parameters = [
            par.uniquify_name(type_map, renamings) for par in self.parameters
        ]
        self.condition = self.condition.uniquify_variables(type_map, renamings)
        self.literal = self.literal.rename_variables(renamings)

    def instantiate(
        self, var_mapping, init_facts, fluent_facts, objects_by_type, result
    ):
        if self.parameters:
            var_mapping = var_mapping.copy()  # Will modify this.
            object_lists = [
                objects_by_type.get(par.type_name, []) for par in self.parameters
            ]
            for object_tuple in itertools.product(*object_lists):
                for (par, obj) in zip(self.parameters, object_tuple):
                    var_mapping[par.name] = obj
                self._instantiate(var_mapping, init_facts, fluent_facts, result)
        else:
            self._instantiate(var_mapping, init_facts, fluent_facts, result)

    def _instantiate(self, var_mapping, init_facts, fluent_facts, result):
        condition = []
        try:
            self.condition.instantiate(var_mapping, init_facts, fluent_facts, condition)
        except conditions.Impossible:
            return
        effects = []
        self.literal.instantiate(var_mapping, init_facts, fluent_facts, effects)
        assert len(effects) <= 1
        if effects:
            result.append((condition, effects[0]))

    def relaxed(self):
        if self.literal.negated:
            return None
        else:
            return Effect(self.parameters, self.condition.relaxed(), self.literal)

    def simplified(self):
        return Effect(self.parameters, self.condition.simplified(), self.literal)


class ProbabilisticOutcome:
    prob: Fraction
    effects: list[Effect]
    reward: f_expression.FunctionAssignment

    def __init__(
        self,
        prob: Fraction,
        effects: list[Effect],
        reward: f_expression.FunctionAssignment,
    ):
        self.prob = prob
        self.effects = effects
        self.reward = reward

    def __eq__(self, other):
        if not isinstance(other, ProbabilisticOutcome):
            return False
        return (
            self.prob == other.prob
            and self.reward == other.reward
            and self.effects == other.effects
        )

    def __repr__(self):
        return f"<ProbabilisticOutcome at {id(self)}"

    def __hash__(self):
        return hash((self.prob, tuple(self.effects), self.reward))

    def instantiate(
        self,
        var_mapping,
        init_facts,
        init_assignment,
        fluent_facts,
        objects_by_type,
        result,
    ):
        i_effs = []
        for eff in self.effects:
            eff.instantiate(
                var_mapping, init_facts, fluent_facts, objects_by_type, i_effs
            )
        i_reward = (
            0
            if self.reward is None
            else (self.reward.instantiate(var_mapping, init_assignment)).expression.value
        )
        if i_effs or i_reward:
            result.append(
                actions.PropositionalActionEffect(self.prob, i_effs, i_reward)
            )

    def _forward(self, fn, *args):
        return ProbabilisticOutcome(
            self.prob, [getattr(e, fn)(*args) for e in self.effects], self.reward
        )

    def relaxed(self):
        return self._forward("relaxed")

    def simplified(self):
        return self._forward("simplified")

    def copy(self):
        return self._forward("copy")

    def uniquify_variables(self, type_map):
        return self._forward("uniquify_variables", type_map)

    def dump(self):
        [e.dump() for e in self.effects]


class ProbabilisticEffect:
    outcomes: list[ProbabilisticOutcome]

    def __init__(self, outcomes: list[ProbabilisticOutcome]):
        self.outcomes = outcomes
        assert sum((out.prob for out in outcomes)) == Fraction(
            1
        ), "outcome probabilities do not sum up to 1"

    def __repr__(self):
        return f"<ProbabilisticEffect at {id(self)}"

    def __eq__(self, other):
        return (
            isinstance(other, ProbabilisticEffect) and self.outcomes == other.outcomes
        )

    def instantiate(
        self,
        var_mapping,
        init_facts,
        init_assignment,
        fluent_facts,
        objects_by_type,
        result,
    ):
        for out in self.outcomes:
            out.instantiate(
                var_mapping,
                init_facts,
                init_assignment,
                fluent_facts,
                objects_by_type,
                result,
            )

    def _forward(self, fn, *args):
        return ProbabilisticEffect([getattr(e, fn)(*args) for e in self.outcomes])

    def relaxed(self):
        return self._forward("relaxed")

    def simplified(self):
        return self._forward("simplified")

    def copy(self):
        return self._forward("copy")

    def uniquify_variables(self, type_map):
        return self._forward("uniquify_variables", type_map)

    def dump(self):
        [o.dump() for o in self.outcomes]


class Action(object):
    name: str
    parameters: list[str]
    num_external_parameters: int
    precondition: conditions.Condition
    effects: list[ProbabilisticEffect]

    def __init__(
        self,
        name: str,
        parameters: list[str],
        num_external_parameters: int,
        precondition: conditions.Condition,
        effects: list[ProbabilisticEffect],
    ):
        assert 0 <= num_external_parameters <= len(parameters)
        self.name = name
        self.parameters = parameters
        # num_external_parameters denotes how many of the parameters
        # are "external", i.e., should be part of the grounded action
        # name. Usually all parameters are external, but "invisible"
        # parameters can be created when compiling away existential
        # quantifiers in conditions.
        self.num_external_parameters = num_external_parameters
        self.precondition = precondition
        self.effects = effects
        self.uniquify_variables()  # TODO: uniquify variables in cost?

    def __repr__(self):
        return "<Action %r at %#x>" % (self.name, id(self))

    def dump(self):
        print("%s(%s)" % (self.name, ", ".join(map(str, self.parameters))))
        print("Precondition:")
        self.precondition.dump()
        print("Effects:")
        for eff in self.effects:
            eff.dump()

    def uniquify_variables(self):
        self.type_map = {par.name: par.type_name for par in self.parameters}
        self.precondition = self.precondition.uniquify_variables(self.type_map)
        for effect in self.effects:
            effect.uniquify_variables(self.type_map)

    def relaxed(self):
        new_effects = []
        for eff in self.effects:
            relaxed_eff = eff.relaxed()
            if relaxed_eff:
                new_effects.append(relaxed_eff)
        return Action(
            self.name,
            self.parameters,
            self.num_external_parameters,
            self.precondition.relaxed().simplified(),
            new_effects,
        )

    def instantiate(
        self,
        var_mapping,
        init_facts,
        init_assignments,
        fluent_facts,
        objects_by_type,
        metric,
    ):
        """Return a PropositionalAction which corresponds to the instantiation of
        this action with the arguments in var_mapping. Only fluent parts of the
        conditions (those in fluent_facts) are included. init_facts are evaluated
        while instantiating.
        Precondition and effect conditions must be normalized for this to work.
        Returns None if var_mapping does not correspond to a valid instantiation
        (because it has impossible preconditions or an empty effect list.)"""
        assert len(self.effects) == 1, "action is not in 1NF"
        arg_list = [
            var_mapping[par.name]
            for par in self.parameters[: self.num_external_parameters]
        ]
        name = "(%s %s)" % (self.name, " ".join(arg_list))

        precondition = []
        try:
            self.precondition.instantiate(
                var_mapping, init_facts, fluent_facts, precondition
            )
        except conditions.Impossible:
            return None
        effects = []
        self.effects[0].instantiate(
            var_mapping,
            init_facts,
            init_assignments,
            fluent_facts,
            objects_by_type,
            effects,
        )

        if effects:
            return actions.PropositionalAction(name, precondition, effects)
        else:
            return None
