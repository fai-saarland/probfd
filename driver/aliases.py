import json
import os

from .util import REPO_ROOT_DIR

aliases_file = os.path.join(REPO_ROOT_DIR, "misc", "aliases.json")

if os.path.exists(aliases_file):
    with open(aliases_file, "r") as file:
        ALIASES = json.load(file)
else:
    ALIASES = dict()


def show_aliases():
    for alias in sorted(list(ALIASES)):
        print(alias)


def set_options_for_alias(alias_name, args):
    """
    If alias_name is an alias for a configuration, set args.search_options
    to the corresponding command-line arguments.
    Otherwise, raise `KeyError`.
    """
    assert not args.search_options

    if alias_name in ALIASES:
        args.search_options = [x.replace(" ", "").replace("\n", "")
                               for x in ALIASES[alias_name]]
    else:
        raise KeyError(alias_name)
