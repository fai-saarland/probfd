import copy
from fractions import Fraction
from typing import List, Optional, Tuple

from . import conditions
from .conditions import Condition, Literal
from .effects import Effect
from .f_expression import Increase
from .pddl_types import TypedObject


class Action(object):
    def __init__(self, name: str, parameters: List[TypedObject],
                 num_external_parameters: int,
                 precondition: Condition,
                 outcomes: List[Tuple[Fraction, List[Effect]]],
                 cost: Optional[Increase]):
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
        self.outcomes = outcomes
        self.cost = cost
        self.uniquify_variables()  # TODO: uniquify variables in cost?

    def determinize(self):
        det_actions = []
        for i, (_, effects) in enumerate(self.outcomes):
            det_actions.append(DeterminizedAction(self.name + f"_outcome_{i}",
                                                  self.parameters,
                                                  self.num_external_parameters,
                                                  self.precondition, effects,
                                                  self.cost))

        return det_actions

    def __repr__(self):
        return "<Action %r at %#x>" % (self.name, id(self))

    def dump(self):
        print("%s(%s)" % (self.name, ", ".join(map(str, self.parameters))))
        print("Precondition:")
        self.precondition.dump()
        print("Outcomes:")
        for prob, effects in self.outcomes:
            print("  Probability: %s" % prob)
            for eff in effects:
                eff.dump(indent="    ")
        print("Cost:")
        if self.cost:
            if isinstance(self.cost, Fraction):
                print("  " + str(self.cost))
            else:
                self.cost.dump()
        else:
            print("  None")

    def uniquify_variables(self):
        self.type_map = {par.name: par.type_name for par in self.parameters}
        self.precondition = self.precondition.uniquify_variables(self.type_map)
        for prob, effects in self.outcomes:
            for effect in effects:
                effect.uniquify_variables(self.type_map)

    def relaxed(self):
        new_outcomes = []
        for prob, effects in self.outcomes:
            new_effects = []
            for eff in effects:
                relaxed_eff = eff.relaxed()
                if relaxed_eff:
                    new_effects.append(relaxed_eff)
            new_outcomes.append((prob, new_effects))
        return Action(self.name, self.parameters,
                      self.num_external_parameters,
                      self.precondition.relaxed().simplified(),
                      new_outcomes, self.cost)

    def untyped(self):
        # We do not actually remove the types from the parameter lists,
        # just additionally incorporate them into the conditions.
        # Maybe not very nice.
        result = copy.copy(self)
        parameter_atoms = [par.to_untyped_strips() for par in self.parameters]
        new_precondition = self.precondition.untyped()
        result.precondition = conditions.Conjunction(
            parameter_atoms + [new_precondition])
        result.outcomes = [(prob, [eff.untyped() for eff in effects])
                           for prob, effects in self.outcomes]
        return result

    def instantiate(self, var_mapping, init_facts, init_assignments,
                    fluent_facts, objects_by_type, metric):
        """Return a PropositionalAction which corresponds to the instantiation of
        this action with the arguments in var_mapping. Only fluent parts of the
        conditions (those in fluent_facts) are included. init_facts are evaluated
        while instantiating.
        Precondition and effect conditions must be normalized for this to work.
        Returns None if var_mapping does not correspond to a valid instantiation
        (because it has impossible preconditions or an empty effect list.)"""
        arg_list = [var_mapping[par.name]
                    for par in self.parameters[:self.num_external_parameters]]
        name = "(%s %s)" % (self.name, " ".join(arg_list))

        precondition = []
        try:
            self.precondition.instantiate(var_mapping, init_facts,
                                          fluent_facts, precondition)
        except conditions.Impossible:
            return None

        inst_outcomes = []
        for prob, effects in self.outcomes:
            inst_effects = []
            for eff in effects:
                eff.instantiate(var_mapping, init_facts, fluent_facts,
                                objects_by_type, inst_effects)
            inst_outcomes.append((prob, inst_effects))

        if metric:
            if self.cost is None:
                cost = Fraction(0)
            else:
                cost = self.cost.instantiate(var_mapping,
                                             init_assignments).expression.value
        else:
            cost = Fraction(1)

        assert isinstance(cost, Fraction)

        return PropositionalAction(name, precondition, inst_outcomes, cost)


class DeterminizedAction:
    def __init__(self, name: str, parameters: List[TypedObject],
                 num_external_parameters: int,
                 precondition: Condition, effects: List[Effect],
                 cost: Optional[Increase]):
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
        self.cost = cost
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
        print("Cost:")
        if self.cost:
            self.cost.dump()
        else:
            print("  None")

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
        return Action(self.name, self.parameters, self.num_external_parameters,
                      self.precondition.relaxed().simplified(),
                      new_effects)

    def untyped(self):
        # We do not actually remove the types from the parameter lists,
        # just additionally incorporate them into the conditions.
        # Maybe not very nice.
        result = copy.copy(self)
        parameter_atoms = [par.to_untyped_strips() for par in self.parameters]
        new_precondition = self.precondition.untyped()
        result.precondition = conditions.Conjunction(
            parameter_atoms + [new_precondition])
        result.effects = [eff.untyped() for eff in self.effects]
        return result

    def instantiate(self, var_mapping, init_facts, init_assignments,
                    fluent_facts, objects_by_type, metric):
        """Return a PropositionalAction which corresponds to the instantiation of
        this action with the arguments in var_mapping. Only fluent parts of the
        conditions (those in fluent_facts) are included. init_facts are evaluated
        while instantiating.
        Precondition and effect conditions must be normalized for this to work.
        Returns None if var_mapping does not correspond to a valid instantiation
        (because it has impossible preconditions or an empty effect list.)"""
        arg_list = [var_mapping[par.name]
                    for par in self.parameters[:self.num_external_parameters]]
        name = "(%s %s)" % (self.name, " ".join(arg_list))

        precondition = []
        try:
            self.precondition.instantiate(var_mapping, init_facts,
                                          fluent_facts, precondition)
        except conditions.Impossible:
            return None
        effects = []
        for eff in self.effects:
            eff.instantiate(var_mapping, init_facts, fluent_facts,
                            objects_by_type, effects)
        if effects:
            if metric:
                if self.cost is None:
                    cost = 0
                else:
                    cost = int(self.cost.instantiate(
                        var_mapping, init_assignments).expression.value)
            else:
                cost = 1
            return PropositionalAction(name, precondition, effects, cost)
        else:
            return None


class PropositionalAction:
    class StripsOutcome:
        def __init__(self, probability: Fraction):
            self.probability = probability
            self.add_effects = []
            self.del_effects = []

    def __init__(self, name: str, precondition: List[Literal],
                 outcomes: List[Tuple[Fraction, List[Effect]]],
                 cost: Fraction):
        self.name = name
        self.precondition = precondition
        self.strips_outcomes = []
        for prob, effects in outcomes:
            strips_outcome = self.StripsOutcome(prob)
            for condition, effect in effects:
                if not effect.negated:
                    strips_outcome.add_effects.append((condition, effect))
            # Warning: This is O(N^2), could be turned into O(N).
            # But that might actually harm performance, since there are
            # usually few effects.
            # TODO: Measure this in critical domains, then use sets if acceptable.
            for condition, effect in effects:
                if effect.negated and (
                        condition,
                        effect.negate()) not in strips_outcome.add_effects:
                    strips_outcome.del_effects.append(
                        (condition, effect.negate()))
            self.strips_outcomes.append(strips_outcome)
        self.cost = cost

    def determinize(self):
        det_actions = []

        for i, strips_outcome in enumerate(self.strips_outcomes):
            det_actions.append(
                DeterministicPropositionalAction(self.name + f"_outcome_{i}",
                                                 self.precondition,
                                                 strips_outcome.add_effects,
                                                 strips_outcome.del_effects,
                                                 self.cost))

        return det_actions

    def __repr__(self):
        return "<PropositionalAction %r at %#x>" % (self.name, id(self))

    def dump(self):
        print(self.name)
        for fact in self.precondition:
            print("PRE: %s" % fact)
        for strips_outcome in self.strips_outcomes:
            print("PROBABILITY: %s" % strips_outcome.probability)
            for cond, fact in strips_outcome.add_effects:
                print("  ADD: %s -> %s" % (", ".join(map(str, cond)), fact))
            for cond, fact in strips_outcome.del_effects:
                print("  DEL: %s -> %s" % (", ".join(map(str, cond)), fact))
        print("cost:", self.cost)


class DeterministicPropositionalAction:
    def __init__(self, name: str, precondition: List[Literal],
                 add_effects: List[Tuple[List[Literal], Literal]],
                 del_effects: List[Tuple[List[Literal], Literal]],
                 cost: Fraction):
        self.name = name
        self.precondition = precondition
        self.add_effects = add_effects
        self.del_effects = del_effects
        self.cost = cost

    def __repr__(self):
        return "<DeterministicPropositionalAction %r at %#x>" % (self.name,
                                                                 id(self))

    def dump(self):
        print(self.name)
        for fact in self.precondition:
            print("PRE: %s" % fact)
        for cond, fact in self.add_effects:
            print("ADD: %s -> %s" % (", ".join(map(str, cond)), fact))
        for cond, fact in self.del_effects:
            print("DEL: %s -> %s" % (", ".join(map(str, cond)), fact))
        print("cost:", self.cost)
