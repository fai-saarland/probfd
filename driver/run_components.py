import logging
import os.path
import shutil
import subprocess
import sys

from . import call
from . import limits
from . import returncodes
from . import util
from .policy_manager import PolicyManager

if os.name == "posix":
    BINARY_EXT = ""
elif os.name == "nt":
    BINARY_EXT = ".exe"
else:
    returncodes.exit_with_driver_unsupported_error("Unsupported OS: " + os.name)

# TODO: We might want to turn translate into a module and call it with "python3 -m translate".
REL_TRANSLATE_PATH = os.path.join("translate", "translate.py")
REL_SEARCH_PATH = f"probfd{BINARY_EXT}"
# Older versions of VAL use lower case, newer versions upper case. We prefer the
# older version because this is what our build instructions recommend.
VALIDATE = (shutil.which(f"validate{BINARY_EXT}") or
            shutil.which(f"Validate{BINARY_EXT}"))


def get_executable_from_path(path, rel_path):
    # First, consider 'build' to be a path directly to the binaries.
    # The path can be absolute or relative to the current working
    # directory.
    abs_path = os.path.join(path, rel_path)

    if not os.path.exists(abs_path):
        abs_path = os.path.join(util.REPO_ROOT_DIR, path, rel_path)

        if not os.path.exists(abs_path):
            returncodes.exit_with_driver_input_error(
                "Could not find planner executable at absolute or relative "
                "path {path}.")

    return abs_path


def get_executable(preset, build_configuration, rel_path):
    # The binaries are in
    #   '<repo-root>/bin/<build>/<configuration>'.
    build_dir = os.path.join(util.BUILDS_DIR, preset)

    if not os.path.exists(build_dir):
        returncodes.exit_with_driver_input_error(
            f"Could not find build directory {build_dir}. "
            f"Please run 'cmake --preset {preset}' and afterwards "
            f"'cmake --build --preset {preset}-{build_configuration}'"
            f"to build.")

    binary_dir = os.path.join(
        util.REPO_ROOT_DIR, "bin", preset, build_configuration, rel_path)
    if not os.path.exists(binary_dir):
        returncodes.exit_with_driver_input_error(
            f"Could not find planner executable for configuration preset "
            f"'{preset}' and build configuration {build_configuration}. "
            f"Please run "
            f"'cmake --build --preset {preset}-{build_configuration}'.")

    return binary_dir


def run_translate(args):
    logging.info("Running translator.")
    time_limit = limits.get_time_limit(
        args.translate_time_limit, args.overall_time_limit)
    memory_limit = limits.get_memory_limit(
        args.translate_memory_limit, args.overall_memory_limit)
    translate = os.path.join(util.REPO_ROOT_DIR, REL_TRANSLATE_PATH)
    assert sys.executable, "Path to interpreter could not be found"
    cmd = [sys.executable] + [
        translate] + args.translate_inputs + args.translate_options

    stderr, returncode = call.get_error_output_and_returncode(
        "translator",
        cmd,
        time_limit=time_limit,
        memory_limit=memory_limit)

    # We collect stderr of the translator and print it here, unless
    # the translator ran out of memory and all output in stderr is
    # related to MemoryError.
    do_print_on_stderr = True
    if returncode == returncodes.TRANSLATE_OUT_OF_MEMORY:
        output_related_to_memory_error = True
        if not stderr:
            output_related_to_memory_error = False
        for line in stderr.splitlines():
            if "MemoryError" not in line:
                output_related_to_memory_error = False
                break
        if output_related_to_memory_error:
            do_print_on_stderr = False

    if do_print_on_stderr and stderr:
        returncodes.print_stderr(stderr)

    if returncode == 0:
        return (0, True)
    elif returncode == 1:
        # Unlikely case that the translator crashed without raising an
        # exception.
        return (returncodes.TRANSLATE_CRITICAL_ERROR, False)
    else:
        # Pass on any other exit code, including in particular signals or
        # exit codes such as running out of memory or time.
        return (returncode, False)


def run_search(args):
    logging.info("Running search (%s)." % args.build)
    time_limit = limits.get_time_limit(
        args.search_time_limit, args.overall_time_limit)
    memory_limit = limits.get_memory_limit(
        args.search_memory_limit, args.overall_memory_limit)

    if args.build is not None:
        executable = get_executable_from_path(args.build, REL_SEARCH_PATH)
    else:
        executable = get_executable(
            args.preset, args.build_config, REL_SEARCH_PATH)

    policy_manager = PolicyManager(args.policy_file)
    policy_manager.delete_existing_policies()

    if not args.search_options:
        returncodes.exit_with_driver_input_error(
            "search needs --alias or search options")
    if "--help" not in args.search_options:
        pass
    try:
        call.check_call(
            "search",
            [executable] + args.search_options,
            stdin=args.search_input,
            time_limit=time_limit,
            memory_limit=memory_limit)
    except subprocess.CalledProcessError as err:
        # TODO: if we ever add support for SEARCH_PLAN_FOUND_AND_* directly
        # in the planner, this assertion no longer holds. Furthermore, we
        # would need to return (err.returncode, True) if the returncode is
        # in [0..10].
        # Negative exit codes are allowed for passing out signals.
        assert err.returncode >= 10 or err.returncode < 0, "got returncode < 10: {}".format(
            err.returncode)
        return (err.returncode, False)
    else:
        return (0, True)
