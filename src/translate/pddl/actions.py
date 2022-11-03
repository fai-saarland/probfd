class PropositionalActionEffect:
    def __init__(self, prob, effects, reward):
        self.prob = prob
        self.reward = reward
        self.add_effects = []
        self.del_effects = []
        for condition, effect in effects:
            if not effect.negated:
                self.add_effects.append((condition, effect))
        # Warning: This is O(N^2), could be turned into O(N).
        # But that might actually harm performance, since there are
        # usually few effects.
        # TODO: Measure this in critical domains, then use sets if acceptable.
        for condition, effect in effects:
            if effect.negated and (condition, effect.negate()) not in self.add_effects:
                self.del_effects.append((condition, effect.negate()))

    def __repr__(self):
        return "<PropositionalActionEffect at %#x>" % (id(self))

    def dump(self):
        for cond, fact in self.add_effects:
            print("ADD: %s -> %s" % (", ".join(map(str, cond)), fact))
        for cond, fact in self.del_effects:
            print("DEL: %s -> %s" % (", ".join(map(str, cond)), fact))


class PropositionalAction:
    def __init__(
        self,
        name,
        precondition,
        outcomes,
    ):
        self.name = name
        self.precondition = precondition
        self.outcomes = outcomes

    def __repr__(self):
        return "<PropositionalAction %r at %#x>" % (self.name, id(self))

    def dump(self):
        print(self.name)
        print("PRE:")
        for cond in self.precondition:
            print(str(cond))
        for eff in self.outcomes:
            eff.dump()
