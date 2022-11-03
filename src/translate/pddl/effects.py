from fractions import Fraction

from . import conditions


class ProbabilityWeightedEffect:
    def __init__(self, prob, effect, reward=None):
        self.prob = Fraction(prob)
        self.effect = effect
        self.reward = reward

    def __str__(self):
        return "{0}{1}".format(self.prob, str(self.effect))

    def dump(self, indent="  "):
        print("{0}{1}".format(indent, str(self)))

    def normalize(self):
        neff = self.effect.normalize()
        return ProbabilityWeightedEffect(self.prob, neff)

    def __repr__(self):
        if self.reward != None:
            return f"{self.prob} (and {self.effect.__repr__()} {repr(self.reward)})"
        else:
            return f"{self.prob} {self.effect.__repr__()}"


class ProbabilisticEffect:
    def __init__(self, effs):
        self.effects = []
        total = Fraction(1)
        for p_eff in effs:
            assert isinstance(p_eff, ProbabilityWeightedEffect)
            if isinstance(p_eff.effect, ProbabilisticEffect):
                self.effects.extend(
                    [
                        ProbabilityWeightedEffect(
                            p_eff.prob * sub_eff.prob, sub_eff.effect
                        )
                        for sub_eff in p_eff.effect.effects
                    ]
                )
            else:
                self.effects.append(p_eff)
            total -= p_eff.prob
        if total > Fraction(0):
            self.effects.append(ProbabilityWeightedEffect(total, ConjunctiveEffect([])))

    def dump(self, indent="  "):
        print("{0}{1}".format(indent, " | ".join((str(eff) for eff in self.effects))))

    def normalize(self):
        neffs = [eff.normalize() for eff in self.effects]
        return ProbabilisticEffect(neffs)

    def __repr__(self):
        return f"(probabilistic {' '.join((repr(e) for e in self.effects))})"


class ConditionalEffect:
    def __init__(self, condition, effect):
        if isinstance(effect, ConditionalEffect):
            self.condition = conditions.Conjunction([condition, effect.condition])
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
        if isinstance(self.effect, ProbabilisticEffect):
            new_effects = []
            for effect in self.effect.effects:
                new_effects.append(
                    ProbabilityWeightedEffect(
                        effect.prob,
                        ConditionalEffect(self.condition, effect.effect).normalize(),
                    )
                )
            return ProbabilisticEffect(new_effects)
        norm_effect = self.effect.normalize()
        if isinstance(norm_effect, ConjunctiveEffect):
            new_effects = []
            for effect in norm_effect.effects:
                assert (
                    isinstance(effect, SimpleEffect)
                    or isinstance(effect, ConditionalEffect)
                    or isinstance(effect, RewardEffect)
                )
                new_effects.append(ConditionalEffect(self.condition, effect))
            return ConjunctiveEffect(new_effects)
        elif isinstance(norm_effect, UniversalEffect):
            child = norm_effect.effect
            cond_effect = ConditionalEffect(self.condition, child)
            return UniversalEffect(norm_effect.parameters, cond_effect)
        else:
            return ConditionalEffect(self.condition, norm_effect)

    def __repr__(self):
        return f"(when {repr(self.condition)} {repr(self.effect)})"


class UniversalEffect:
    def __init__(self, parameters, effect):
        if isinstance(effect, UniversalEffect):
            self.parameters = parameters + effect.parameters
            self.effect = effect.effect
        else:
            self.parameters = parameters
            self.effect = effect

    def __repr__(self):
        return f"(forall ({' '.join(self.parameters)}) {repr(self.effect)})"

    def dump(self, indent="  "):
        print("%sforall %s" % (indent, ", ".join(map(str, self.parameters))))
        self.effect.dump(indent + "  ")

    def normalize(self):
        norm_effect = self.effect.normalize()
        assert not isinstance(norm_effect, ProbabilisticEffect)
        if isinstance(norm_effect, ConjunctiveEffect):
            new_effects = []
            for effect in norm_effect.effects:
                assert (
                    isinstance(effect, SimpleEffect)
                    or isinstance(effect, ConditionalEffect)
                    or isinstance(effect, UniversalEffect)
                    or isinstance(effect, RewardEffect)
                )
                new_effects.append(UniversalEffect(self.parameters, effect))
            return ConjunctiveEffect(new_effects)
        else:
            return UniversalEffect(self.parameters, norm_effect)


class ConjunctiveEffect:
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
        return ConjunctiveEffect(new_effects)

    def __repr__(self):
        return f"(and {' '.join((repr(e) for e in self.effects))})"


class SimpleEffect:
    def __init__(self, effect):
        self.effect = effect

    def dump(self, indent="  "):
        print("%s%s" % (indent, self.effect))

    def normalize(self):
        return self

    def __repr__(self):
        return repr(self.effect)


class RewardEffect:
    def __init__(self, effect):
        self.effect = effect

    def dump(self, indent="  "):
        print("%s%s" % (indent, self.effect))

    def normalize(self):
        return self

    def __repr__(self):
        return repr(self.effect)
