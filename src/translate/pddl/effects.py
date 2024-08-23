from fractions import Fraction
from typing import List, Union, Tuple

from . import conditions
from .conditions import Condition, Literal
from .f_expression import Increase, NumericConstant, ArithmeticExpression
from .pddl_types import TypedObject

AnyEffect = Union[
    "ConditionalEffect",
    "ConjunctiveEffect",
    "UniversalEffect",
    "SimpleEffect",
    "CostEffect",
    "ProbabilisticEffect"]


def cartesian_product(*sequences):
    # TODO: Also exists in tools.py outside the pddl package (defined slightly
    #       differently). Not good. Need proper import paths.
    if not sequences:
        yield ()
    else:
        for tup in cartesian_product(*sequences[1:]):
            for item in sequences[0]:
                yield (item,) + tup


class Effect:
    def __init__(self,
                 parameters: List[TypedObject],
                 condition: Condition,
                 literal: Literal) -> None:
        self.parameters = parameters
        self.condition = condition
        self.literal = literal

    def __eq__(self, other):
        return (self.__class__ is other.__class__ and
                self.parameters == other.parameters and
                self.condition == other.condition and
                self.literal == other.literal)

    def dump(self, indent="  "):
        if self.parameters:
            print(
                "%sforall %s" % (indent, ", ".join(map(str, self.parameters))))
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
        self.parameters = [par.uniquify_name(type_map, renamings)
                           for par in self.parameters]
        self.condition = self.condition.uniquify_variables(type_map, renamings)
        self.literal = self.literal.rename_variables(renamings)

    def instantiate(self, var_mapping, init_facts, fluent_facts,
                    objects_by_type, result):
        if self.parameters:
            var_mapping = var_mapping.copy()  # Will modify this.
            object_lists = [objects_by_type.get(par.type_name, [])
                            for par in self.parameters]
            for object_tuple in cartesian_product(*object_lists):
                for (par, obj) in zip(self.parameters, object_tuple):
                    var_mapping[par.name] = obj
                self._instantiate(var_mapping, init_facts, fluent_facts, result)
        else:
            self._instantiate(var_mapping, init_facts, fluent_facts, result)

    def _instantiate(self, var_mapping, init_facts, fluent_facts, result):
        condition = []
        try:
            self.condition.instantiate(var_mapping, init_facts, fluent_facts,
                                       condition)
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
            return Effect(self.parameters, self.condition.relaxed(),
                          self.literal)

    def simplified(self):
        return Effect(self.parameters, self.condition.simplified(),
                      self.literal)


class ProbabilisticOutcomes(object):
    def __init__(self, outcomes: List[Tuple[Fraction, List[Effect]]]):
        self.outcomes = outcomes

    def __eq__(self, other):
        return (self.__class__ is other.__class__ and
                self.outcomes == other.outcomes)

    def dump(self):
        indent = "  "
        for prob, effects in self.outcomes:
            print(f"{indent}probability {prob}")
            for effect in effects:
                effect.dump()

    def copy(self):
        return ProbabilisticOutcomes(self.outcomes.copy())

    def uniquify_variables(self, type_map):
        for _, effects in self.outcomes:
            for effect in effects:
                effect.uniquify_variables(type_map)

    def instantiate(self, var_mapping, init_facts, fluent_facts,
                    objects_by_type, result):
        pass

    def relaxed(self):
        return ProbabilisticOutcomes(
            [(prob, [effect.relaxed() for effect in effects])
             for prob, effects in self.outcomes])

    def simplified(self):
        return ProbabilisticOutcomes(
            [(prob, [effect.simplified() for effect in effects])
             for prob, effects in self.outcomes])


class ConditionalEffect(object):
    def __init__(self, condition, effect):
        if isinstance(effect, ConditionalEffect):
            self.condition = conditions.Conjunction(
                [condition, effect.condition])
            self.effect = effect.effect
        else:
            self.condition = condition
            self.effect = effect

    def dump(self, indent="  "):
        print("%sif" % (indent))
        self.condition.dump(indent + "  ")
        print("%sthen" % (indent))
        self.effect.dump(indent + "  ")

    def normalize(self):
        norm_effect = self.effect.normalize()
        if isinstance(norm_effect, ConjunctiveEffect):
            new_effects = []
            for effect in norm_effect.effects:
                assert isinstance(effect, SimpleEffect) or isinstance(effect,
                                                                      ConditionalEffect)
                new_effects.append(ConditionalEffect(self.condition, effect))
            return ConjunctiveEffect(new_effects)
        elif isinstance(norm_effect, UniversalEffect):
            child = norm_effect.effect
            cond_effect = ConditionalEffect(self.condition, child)
            return UniversalEffect(norm_effect.parameters, cond_effect)
        elif isinstance(norm_effect, ProbabilisticEffect):
            pairs = norm_effect.effect_probability_pairs
            new_pairs = [(probability, ConjunctiveEffect(self.condition,
                                                         nested_effect)) for
                         probability, nested_effect in pairs]
            return ProbabilisticEffect(new_pairs)
        else:
            return ConditionalEffect(self.condition, norm_effect)

    def extract_cost(self):
        return None, self


class UniversalEffect(object):
    def __init__(self, parameters, effect):
        if isinstance(effect, UniversalEffect):
            self.parameters = parameters + effect.parameters
            self.effect = effect.effect
        else:
            self.parameters = parameters
            self.effect = effect

    def dump(self, indent="  "):
        print("%sforall %s" % (indent, ", ".join(map(str, self.parameters))))
        self.effect.dump(indent + "  ")

    def normalize(self):
        norm_effect = self.effect.normalize()

        # Probabilistic effects within universal effects make normalization
        # impossible. Pulling the probabilistic effects out of the
        # universal quantification would require instantiating the forall and
        # treating it as a normal conjunction, but instantiation is done
        # later in the translator. Even then, this would most likely lead to
        # an unmanagable combinatorical explosion.
        assert not isinstance(norm_effect, ProbabilisticEffect), \
            "Probabilistic effects within universal effects are not" \
            "supported."

        if isinstance(norm_effect, ConjunctiveEffect):
            new_effects = []
            for effect in norm_effect.effects:
                assert (isinstance(effect, SimpleEffect) or
                        isinstance(effect, ConditionalEffect) or
                        isinstance(effect, UniversalEffect))
                new_effects.append(UniversalEffect(self.parameters, effect))
            return ConjunctiveEffect(new_effects)
        else:
            return UniversalEffect(self.parameters, norm_effect)

    def extract_cost(self):
        assert (self.effect.extract_cost()[0] is None,
                "Costs within universal effects are not supported")
        return None, self


class ConjunctiveEffect(object):
    def __init__(self, effects):
        flattened_effects = []
        for effect in effects:
            if isinstance(effect, ConjunctiveEffect):
                flattened_effects += effect.effects
            else:
                flattened_effects.append(effect)
        self.effects = flattened_effects

    def dump(self, indent="  "):
        print("%sand" % (indent))
        for eff in self.effects:
            eff.dump(indent + "  ")

    def normalize(self):
        new_effects = []
        for effect in self.effects:
            new_effects.append(effect.normalize())

        probabilistic_effects = [effect for effect in new_effects if isinstance(
            effect,
            ProbabilisticEffect)]

        if not probabilistic_effects:
            return ConjunctiveEffect(new_effects)

        # Use the rule
        # (p1->e1|...|pm->em) /\ (p1'->e1'|...|pn'->fn') =
        # p1*p1'->(e1/\e1')|p1*p2'->(e1/\e2')|...|pk*pm'->(em/\en')
        # to reduce all involved probabilistic effects to a single one
        indices = [0] * len(probabilistic_effects)
        multiplied_out_pairs = []

        while True:
            product_probability = Fraction(1)
            effects = []

            for i in range(0, len(probabilistic_effects)):
                effect = probabilistic_effects[i]
                index = indices[i]
                (prob, nested_effect) = effect.effect_probability_pairs[index]
                product_probability *= prob
                effects.append(nested_effect)

            multiplied_out_pairs.append((product_probability, ConjunctiveEffect(
                effects)))

            for i in range(0, len(probabilistic_effects)):
                effect = probabilistic_effects[i]
                index = indices[i]
                if index + 1 < len(effect.effect_probability_pairs):
                    indices[i] = index + 1
                    break
                else:
                    indices[i] = 0
            else:
                break

        # Now use the rule
        # e /\ (p1->e1|...|pk->ek) = p1->(e/\e1)|...|pk->(e/\ek)
        normal_effects = [effect for effect in new_effects if
                          not isinstance(
                              effect,
                              ProbabilisticEffect)]

        new_pairs = [
            (probability, ConjunctiveEffect([effect, *normal_effects])) for
            probability, effect in multiplied_out_pairs]

        return ProbabilisticEffect(new_pairs)

    def extract_cost(self):
        new_effects = []
        cost_effects = []
        for effect in self.effects:
            if isinstance(effect, CostEffect):
                cost_effects.append(effect)
            else:
                new_effects.append(effect)

        conj = ConjunctiveEffect(new_effects)

        if not cost_effects:
            return None, conj

        if len(cost_effects) == 1:
            return cost_effects[0], conj

        # If multiple cost effects appear, their costs are additive.
        first_fluent = cost_effects[0].effect.fluent
        sum_expressions = []

        for cost_effect in cost_effects:
            assert cost_effect.effect.fluent == first_fluent
            sum_expressions.append(cost_effect.effect.expression)

        additive_cost_effect = CostEffect(Increase(
            first_fluent,
            ArithmeticExpression("+", sum_expressions)))

        return additive_cost_effect, conj


class ProbabilisticEffect(object):
    def __init__(self, effect_probability_pairs):
        flattened_pairs = []
        for probability, effect in effect_probability_pairs:
            if isinstance(effect, ProbabilisticEffect):
                for nested_probability, nested_effect in (
                        effect.effect_probability_pairs):
                    new_probability = probability * nested_probability
                    flattened_pairs.append((new_probability, nested_effect))
            else:
                flattened_pairs.append((probability, effect))

        self.effect_probability_pairs = flattened_pairs

    def dump(self, indent="  "):
        print(f"{indent}probabilistic")
        indent += "  "
        for prob, eff in self.effect_probability_pairs:
            print(f"{indent}{prob}")
            eff.dump(indent + "  ")

    def normalize(self):
        normalized_pairs = [(probability, effect.normalize()) for
                            probability, effect in
                            self.effect_probability_pairs]
        return ProbabilisticEffect(normalized_pairs)

    def extract_cost(self):
        remaining_pairs = []
        weighted_cost = 0
        fluent = None

        weighted_cost_exprs = []

        for probability, effect in self.effect_probability_pairs:
            cost_effect, remaining = effect.extract_cost()

            if cost_effect is not None:
                assert isinstance(cost_effect.effect, Increase)
                assert not fluent or cost_effect.effect.fluent == fluent

                weighted_cost_exprs.append(ArithmeticExpression(
                    "*",
                    [NumericConstant(probability),
                     cost_effect.effect.expression]))
                fluent = cost_effect.effect.fluent

            remaining_pairs.append((probability, remaining))

        if weighted_cost_exprs:
            cost_effect = CostEffect(
                Increase(fluent, ArithmeticExpression(
                    "+", weighted_cost_exprs)))
        else:
            cost_effect = None

        return cost_effect, ProbabilisticEffect(remaining_pairs)


class SimpleEffect(object):
    def __init__(self, effect):
        self.effect = effect

    def dump(self, indent="  "):
        print("%s%s" % (indent, self.effect))

    def normalize(self):
        return self

    def extract_cost(self):
        return None, self


class CostEffect(object):
    def __init__(self, effect):
        self.effect = effect

    def dump(self, indent="  "):
        print("%s%s" % (indent, self.effect))

    def normalize(self):
        return self

    def extract_cost(self):
        return self, None  # this would only happen if
    # an action has no effect apart from the cost effect
