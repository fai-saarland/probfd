"""
Test module for Fast Downward driver script. Run with

    py.test driver/tests.py
"""

import os
import subprocess
import sys

import pytest

from . import limits
from . import returncodes
from .aliases import ALIASES
from .arguments import EXAMPLES
from .call import check_call
from .run_components import get_executable, REL_SEARCH_PATH
from .util import REPO_ROOT_DIR, find_domain_filename


def cleanup():
    subprocess.check_call([sys.executable, "fast-downward.py", "--cleanup"],
                          cwd=REPO_ROOT_DIR)


def teardown_module(module):
    cleanup()


def run_driver(parameters):
    cmd = [sys.executable, "fast-downward.py", "--keep"] + parameters
    return subprocess.check_call(cmd, cwd=REPO_ROOT_DIR)


def test_commandline_args():
    for description, cmd in EXAMPLES:
        parameters = [x.strip('"') for x in cmd]
        run_driver(parameters)


def test_aliases():
    for alias, config in ALIASES.items():
        parameters = ["--alias", alias, "output.sas"]
        run_driver(parameters)


def test_show_aliases():
    run_driver(["--show-aliases"])


def _convert_to_standalone_config(config):
    replacements = [
        ("H_COST_TRANSFORM", "no_transform()"),
        ("S_COST_TYPE", "normal"),
        ("BOUND", "infinity"),
    ]
    for index, part in enumerate(config):
        for before, after in replacements:
            part = part.replace(before, after)
        config[index] = part
    return config


def _run_search(config):
    check_call(
        "search",
        [get_executable("release", REL_SEARCH_PATH)] + list(config),
        stdin="output.sas")


@pytest.mark.skipif(not limits.can_set_time_limit(),
                    reason="Cannot set time limits on this system")
def test_hard_time_limit():
    def preexec_fn():
        limits.set_time_limit(10)

    driver = [sys.executable, "fast-downward.py"]
    parameters = [
        "--translate", "--translate-time-limit",
        "10s", "misc/tests/benchmarks/gripper/prob01.pddl"]
    subprocess.check_call(driver + parameters, preexec_fn=preexec_fn,
                          cwd=REPO_ROOT_DIR)

    parameters = [
        "--translate", "--translate-time-limit",
        "20s", "misc/tests/benchmarks/gripper/prob01.pddl"]
    with pytest.raises(subprocess.CalledProcessError) as exception_info:
        subprocess.check_call(driver + parameters, preexec_fn=preexec_fn,
                              cwd=REPO_ROOT_DIR)
    assert exception_info.value.returncode == returncodes.DRIVER_INPUT_ERROR


def test_automatic_domain_file_name_computation():
    benchmarks_dir = os.path.join(REPO_ROOT_DIR, "benchmarks")
    for dirpath, dirnames, filenames in os.walk(benchmarks_dir):
        for filename in filenames:
            if "domain" not in filename:
                assert find_domain_filename(os.path.join(dirpath, filename))
