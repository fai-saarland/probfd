import itertools
import os
import os.path
import re

from . import returncodes

_POLICY_INFO_REGEX = re.compile(
    r"; cost = (\d+) \((unit cost|general cost)\)\n")


def _read_last_line(filename):
    line = None
    with open(filename) as input_file:
        for line in input_file:
            pass
    return line


def _parse_policy(policy_filename):
    """Parse a policy file and return a pair (cost, problem_type)
    summarizing the salient information. Return (None, None) for
    incomplete policies."""

    last_line = _read_last_line(policy_filename) or ""
    match = _POLICY_INFO_REGEX.match(last_line)
    if match:
        return int(match.group(1)), match.group(2)
    else:
        return None, None


class PolicyManager:
    def __init__(self, policy_prefix, single_policy=False):
        self._policy_prefix = policy_prefix
        self._policy_costs = []
        self._problem_type = None
        self._single_policy = single_policy

    def get_policy_prefix(self):
        return self._policy_prefix

    def get_policy_counter(self):
        return len(self._policy_costs)

    def get_problem_type(self):
        if self._problem_type is None:
            returncodes.exit_with_driver_critical_error(
                "no policies found yet: cost type not set")
        return self._problem_type

    def process_new_policies(self):
        """Update information about policies after a planner run.

        Read newly generated policies and store the relevant information.
        If the last policy file is incomplete, delete it.
        """

        had_incomplete_policy = False
        for counter in itertools.count(self.get_policy_counter() + 1):
            policy_filename = self._get_policy_file(counter)

            def bogus_policy(msg):
                returncodes.exit_with_driver_critical_error(
                    "%s: %s" % (policy_filename, msg))

            if not os.path.exists(policy_filename):
                break
            if had_incomplete_policy:
                bogus_policy("policy found after incomplete policy")
            cost, problem_type = _parse_policy(policy_filename)
            if cost is None:
                had_incomplete_policy = True
                print("%s is incomplete. Deleted the file." % policy_filename)
                os.remove(policy_filename)
            else:
                print("policy manager: found new policy with cost %d" % cost)
                if self._problem_type is None:
                    # This is the first policy we found.
                    self._problem_type = problem_type
                else:
                    # Check if info from this policy matches previous info.
                    if self._problem_type != problem_type:
                        bogus_policy("problem type has changed")
                    if cost >= self._policy_costs[-1]:
                        bogus_policy("policy quality has not improved")
                self._policy_costs.append(cost)

    def get_existing_policies(self):
        """Yield all policies that match the given policy prefix."""
        if os.path.exists(self._policy_prefix):
            yield self._policy_prefix

        for counter in itertools.count(start=1):
            policy_filename = self._get_policy_file(counter)
            if os.path.exists(policy_filename):
                yield policy_filename
            else:
                break

    def delete_existing_policies(self):
        """Delete all policies that match the given policy prefix."""
        for policy in self.get_existing_policies():
            os.remove(policy)

    def _get_policy_file(self, number):
        return "%s.%d" % (self._policy_prefix, number)
