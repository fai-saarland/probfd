import argparse
import os.path
import re
import sys

from . import aliases
from . import returncodes
from . import util

PROBFD_SUBCOMMANDS = ["list-features", "search"]

DESCRIPTION = """Fast Downward driver script.

Input files can be either a PDDL problem file (with an optional PDDL domain
file), in which case the driver runs both planner components (translate and
search), or a SAS+ translator output file, in which case the driver runs just
the search component. You can override this default behaviour by selecting
components manually with the flags below. The first component to be run
determines the required input files:

--translate: [DOMAIN] PROBLEM
--search: TRANSLATE_OUTPUT

Arguments given before the specified input files are interpreted by the driver
script ("driver options"). Arguments given after the input files are passed on
to the planner components ("component options"). In exceptional cases where no
input files are needed, use "--" to separate driver from component options. In
even more exceptional cases where input files begin with "--", use "--" to
separate driver options from input files and also to separate input files from
component options.

By default, component options are passed to the search component. Use
"--translate-options" or "--search-options" within the component options to
override the default for the following options, until overridden again. (See
below for examples.)"""

LIMITS_HELP = """You can limit the time or memory for individual components
or the whole planner. The effective limit for each component is the minimum
of the component, overall, external soft, and external hard limits.

Limits are given in seconds or MiB. You can change the unit by using the
suffixes s, m, h and K, M, G.

By default, all limits are inactive. Only external limits (e.g. set with
ulimit) are respected."""

EXAMPLES = [
    ("Translate and find a policy with iLAO* + hroc:",
     ["misc/tests/benchmarks/gripper/prob01.pddl",
      "--search", '"ilao(hroc())"']),
    ("Translate and run no search:",
     ["--translate",
      "misc/tests/benchmarks/gripper/prob01.pddl"]),
    ("Run predefined configuration on translated task:",
     ["--alias", "my-alias-config", "output.sas"]),
    ("Run the search component in debug mode (with assertions enabled):",
     ["--debug", "output.sas", "--search", '"ilao()"']),
    ("Pass options to translator and search components:",
     ["misc/tests/benchmarks/gripper/prob01.pddl",
      "--translate-options", "--full-encoding",
      "--search-options", "--search", '"ilao()"']),
    ("Predefine an evaluator (new style):",
     ["misc/tests/benchmarks/gripper/prob01.pddl",
      "--search", '"let(hroc, hroc(), ilao(eval=[hroc]))"']),
]

EPILOG = """component options:
  --translate-options OPTION1 OPTION2 ...
  --search-options OPTION1 OPTION2 ...
                        pass OPTION1 OPTION2 ... to specified planner component
                        (default: pass component options to search)

Examples:

%s
""" % "\n\n".join(
    "%s\n%s" % (desc, " ".join([os.path.basename(sys.argv[0])] + parameters))
    for desc, parameters in EXAMPLES)

COMPONENTS_PLUS_OVERALL = ["translate", "search", "overall"]
DEFAULT_SAS_FILE = "output.psas"

"""
Function to emulate the behavior of ArgumentParser.error, but with our
custom exit codes instead of 2.
"""


def print_usage_and_exit_with_driver_input_error(parser, msg):
    parser.print_usage()
    returncodes.exit_with_driver_input_error(
        "{}: error: {}".format(os.path.basename(sys.argv[0]), msg))


class RawHelpFormatter(argparse.HelpFormatter):
    """Preserve newlines and spacing."""

    def _fill_text(self, text, width, indent):
        return ''.join([indent + line for line in text.splitlines(True)])

    def _format_args(self, action, default_metavar):
        """Show explicit help for remaining args instead of "..."."""
        if action.nargs == argparse.REMAINDER:
            return "INPUT_FILE1 [INPUT_FILE2] [COMPONENT_OPTION ...]"
        else:
            return argparse.HelpFormatter._format_args(self, action,
                                                       default_metavar)


def _rindex(seq, element):
    """Like list.index, but gives the index of the *last* occurrence."""
    seq = list(reversed(seq))
    reversed_index = seq.index(element)
    return len(seq) - 1 - reversed_index


def _split_off_filenames(planner_args):
    """Given the list of arguments to be passed on to the planner
    components, split it into a prefix of filenames and a suffix of
    options. Returns a pair (filenames, options).

    If a "--" separator is present, the last such separator serves as
    the border between filenames and options. The separator itself is
    not returned. (This implies that "--" can be a filename, but never
    an option to a planner component.)

    If no such separator is present, the first argument that begins
    with "-" and consists of at least two characters starts the list
    of options, and all previous arguments are filenames."""

    if "--" in planner_args:
        separator_pos = _rindex(planner_args, "--")
        num_filenames = separator_pos
        del planner_args[separator_pos]
    else:
        num_filenames = 0
        for arg in planner_args:
            if arg in PROBFD_SUBCOMMANDS:
                break
            
            # We treat "-" by itself as a filename because by common
            # convention it denotes stdin or stdout, and we might want
            # to support this later.
            if arg.startswith("-") and arg != "-":
                break
            num_filenames += 1
    return planner_args[:num_filenames], planner_args[num_filenames:]


def _split_planner_args(parser, args):
    """Partition args.planner_args, the list of arguments for the
    planner components, into args.filenames, args.translate_options
    and args.search_options. Modifies args directly and removes the original
    args.planner_args list."""

    args.filenames, options = _split_off_filenames(args.planner_args)

    args.translate_options = []
    args.search_options = []

    curr_options = args.search_options
    substitute_subcommands = True
    for option in options:
        if option == "--translate-options":
            curr_options = args.translate_options
            substitute_subcommands = False
        elif option == "--search-options":
            curr_options = args.search_options
            substitute_subcommands = True
        else:
            if (substitute_subcommands and
                option[:2] == "--" and
                option[2:] in PROBFD_SUBCOMMANDS):
                option = option[2:]
            
            curr_options.append(option)


def _check_mutex_args(parser, args, required=False):
    for pos, (name1, is_specified1) in enumerate(args):
        for name2, is_specified2 in args[pos + 1:]:
            if is_specified1 and is_specified2:
                print_usage_and_exit_with_driver_input_error(
                    parser, "cannot combine %s with %s" % (name1, name2))
    if required and not any(is_specified for _, is_specified in args):
        print_usage_and_exit_with_driver_input_error(
            parser, "exactly one of {%s} has to be specified" %
                    ", ".join(name for name, _ in args))


def _looks_like_search_input(filename):
    with open(filename) as input_file:
        first_line = next(input_file, "").rstrip()
    return first_line == "begin_version"


def _set_components_automatically(parser, args):
    """Guess which planner components to run based on the specified
    filenames and set args.components accordingly. Currently
    implements some simple heuristics:

    1. If there is exactly one input file and it looks like a
       Fast-Downward-generated file, run search only.
    2. Otherwise, run all components."""

    if len(args.filenames) == 1 and _looks_like_search_input(args.filenames[0]):
        args.components = ["search"]
    else:
        args.components = ["translate", "search"]


def _set_components_and_inputs(parser, args):
    """Set args.components to the planner components to be run and set
    args.translate_inputs and args.search_input to the correct input
    filenames.

    Rules:
    1. If any --run-xxx option is specified, then the union
       of the specified components is run.
    2. If nothing is specified, use automatic rules. See
       separate function."""

    args.components = []
    if args.translate or args.run_all:
        args.components.append("translate")
    if args.search or args.run_all:
        args.components.append("search")

    if not args.components:
        _set_components_automatically(parser, args)

    args.translate_inputs = []

    assert args.components
    first = args.components[0]
    num_files = len(args.filenames)
    # When passing --help to any of the components (or -h to the
    # translator), we don't require input filenames and silently
    # swallow any that are provided. This is undocumented to avoid
    # cluttering the driver's --help output.
    if first == "translate":
        if "--help" in args.translate_options or "-h" in args.translate_options:
            args.translate_inputs = []
        elif num_files == 1:
            task_file, = args.filenames
            domain_file = util.find_domain_filename(task_file)
            args.translate_inputs = [domain_file, task_file]
        elif num_files == 2:
            args.translate_inputs = args.filenames
        else:
            print_usage_and_exit_with_driver_input_error(
                parser, "translator needs one or two input files")
    elif first == "search":
        if num_files == 0:
            args.search_input = None
        elif num_files == 1:
            args.search_input, = args.filenames
        else:
            print_usage_and_exit_with_driver_input_error(
                parser, "search needs zero or one input files")
    else:
        assert False, first


def _set_translator_output_options(parser, args):
    if any("--sas-file" in opt for opt in args.translate_options):
        print_usage_and_exit_with_driver_input_error(
            parser,
            "Cannot pass the \"--sas-file\" option to translate.py from the "
            "fast-downward.py script. Pass it directly to fast-downward.py instead.")

    args.search_input = args.sas_file
    args.translate_options += ["--sas-file", args.search_input]


def _get_time_limit_in_seconds(limit, parser):
    match = re.match(r"^(\d+)(s|m|h)?$", limit, flags=re.I)
    if not match:
        print_usage_and_exit_with_driver_input_error(parser,
                                                     "malformed time limit parameter: {}".format(
                                                         limit))
    time = int(match.group(1))
    suffix = match.group(2)
    if suffix is not None:
        suffix = suffix.lower()
    if suffix == "m":
        time *= 60
    elif suffix == "h":
        time *= 3600
    return time


def _get_memory_limit_in_bytes(limit, parser):
    match = re.match(r"^(\d+)(k|m|g)?$", limit, flags=re.I)
    if not match:
        print_usage_and_exit_with_driver_input_error(parser,
                                                     "malformed memory limit parameter: {}".format(
                                                         limit))
    memory = int(match.group(1))
    suffix = match.group(2)
    if suffix is not None:
        suffix = suffix.lower()
    if suffix == "k":
        memory *= 1024
    elif suffix is None or suffix == "m":
        memory *= 1024 * 1024
    elif suffix == "g":
        memory *= 1024 * 1024 * 1024
    return memory


def set_time_limit_in_seconds(parser, args, component):
    param = component + "_time_limit"
    limit = getattr(args, param)
    if limit is not None:
        setattr(args, param, _get_time_limit_in_seconds(limit, parser))


def set_memory_limit_in_bytes(parser, args, component):
    param = component + "_memory_limit"
    limit = getattr(args, param)
    if limit is not None:
        setattr(args, param, _get_memory_limit_in_bytes(limit, parser))


def _convert_limits_to_ints(parser, args):
    for component in COMPONENTS_PLUS_OVERALL:
        set_time_limit_in_seconds(parser, args, component)
        set_memory_limit_in_bytes(parser, args, component)


def parse_args():
    parser = argparse.ArgumentParser(
        description=DESCRIPTION, epilog=EPILOG,
        formatter_class=RawHelpFormatter,
        add_help=False)

    help_options = parser.add_argument_group(
        title=("driver options that show information and exit "
               "(don't run planner)"))
    # We manually add the help option because we want to control
    # how it is grouped in the output.
    help_options.add_argument(
        "-h", "--help",
        action="help", default=argparse.SUPPRESS,
        help="show this help message and exit")
    help_options.add_argument(
        "-v", "--version", action="store_true",
        help="print version number and exit")
    help_options.add_argument(
        "--show-aliases", action="store_true",
        help="show the known aliases (see --alias) and exit")

    components = parser.add_argument_group(
        title=("driver options selecting the planner components to be run\n"
               "(may select several; default: auto-select based on input file(s))"))
    components.add_argument(
        "--run-all", action="store_true",
        help="run all components of the planner")
    components.add_argument(
        "--translate", action="store_true",
        help="run translator component")
    components.add_argument(
        "--search", action="store_true",
        help="run search component")

    limits = parser.add_argument_group(
        title="time and memory limits", description=LIMITS_HELP)
    for component in COMPONENTS_PLUS_OVERALL:
        limits.add_argument("--{}-time-limit".format(component))
        limits.add_argument("--{}-memory-limit".format(component))

    driver_other = parser.add_argument_group(
        title="other driver options")
    driver_other.add_argument(
        "--alias",
        help="run a config with an alias (e.g. seq-sat-lama-2011)")
    driver_other.add_argument(
        "--build",
        help="absolute or relative path to a directory holding the planner "
             "binaries. Mutually exclusive with --preset.")
    driver_other.add_argument(
        "--preset",
        help="The targetted configuration preset of the planner executable. "
             "Mutually exclusive with --build.",
        choices=["default", "tests", "no_lp", "minimal"])
    driver_other.add_argument(
        "--build-config",
        help="The build configuration to run (debug or release).",
        choices=["debug", "release"])
    driver_other.add_argument(
        "--debug", action="store_true",
        help="alias for --build-config='debug'")
    driver_other.add_argument(
        "--log-level", choices=["debug", "info", "warning"],
        default="info",
        help="set log level (most verbose: debug; least verbose: warning; default: %(default)s)")

    driver_other.add_argument(
        "--policy-file", metavar="FILE", default="sas_policy",
        help="write policies(s) to FILE (default: %(default)s; anytime "
             "configurations append .1, .2, ...)")

    driver_other.add_argument(
        "--sas-file", metavar="FILE",
        help="intermediate file for storing the translator output "
             "(implies --keep-sas-file, default: {})".format(DEFAULT_SAS_FILE))
    driver_other.add_argument(
        "--keep-sas-file", action="store_true",
        help="keep translator output file (implied by --sas-file, default: "
             "delete file if translator and search component are active)")

    driver_other.add_argument(
        "--cleanup", action="store_true",
        help="clean up temporary files (translator output and policy files) "
             "and exit")

    parser.add_argument(
        "planner_args", nargs=argparse.REMAINDER,
        help="file names and options passed on to planner components")

    # Using argparse.REMAINDER relies on the fact that the first
    # argument that doesn't belong to the driver doesn't look like an
    # option, i.e., doesn't start with "-". This is usually satisfied
    # because the argument is a filename; in exceptional cases, "--"
    # can be used as an explicit separator. For example, "./fast-downward.py --
    # --help" passes "--help" to the search code.

    args = parser.parse_args()

    if args.sas_file:
        args.keep_sas_file = True
    else:
        args.sas_file = DEFAULT_SAS_FILE

    if args.debug:
        if args.build_config:
            print_usage_and_exit_with_driver_input_error(
                parser, "The option --debug is an alias for "
                        "'--build-config debug'. Do not specify "
                        "both --debug and --build-config.")

        if args.build:
            print_usage_and_exit_with_driver_input_error(
                parser, "The option --debug is an alias for "
                        "'--build-config debug'. The options "
                        "--build and --build-config are mutually exclusive. "
                        "Do not specify both --debug and --build.")

    if args.build and (args.preset or args.build_config):
        print_usage_and_exit_with_driver_input_error(
            parser, "The options --build and --preset as well as --build and "
                    "--build-config are mutually exclusive.")

    if not args.build:
        if not args.preset:
            args.preset = "default"

        if not args.build_config:
            args.build_config = "debug" if args.debug else "release"

    _split_planner_args(parser, args)

    _check_mutex_args(parser, [
        ("--alias", args.alias is not None),
        ("options for search component", bool(args.search_options))])

    _set_translator_output_options(parser, args)

    _convert_limits_to_ints(parser, args)

    if args.alias:
        try:
            aliases.set_options_for_alias(args.alias, args)
        except KeyError:
            print_usage_and_exit_with_driver_input_error(
                parser, "unknown alias: %r" % args.alias)

    if not args.version and not args.show_aliases and not args.cleanup:
        _set_components_and_inputs(parser, args)
        if "translate" not in args.components or "search" not in args.components:
            args.keep_sas_file = True

    return args
