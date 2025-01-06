import logging
import os
import sys

from . import __version__
from . import aliases
from . import arguments
from . import cleanup
from . import limits
from . import run_components
from . import util


def main():
    args = arguments.parse_args()
    logging.basicConfig(level=getattr(logging, args.log_level.upper()),
                        format="%(levelname)-8s %(message)s",
                        stream=sys.stdout)
    logging.debug("processed args: %s" % args)

    if args.version:
        print(__version__)
        sys.exit()

    if args.show_aliases:
        aliases.show_aliases()
        sys.exit()

    if args.cleanup:
        cleanup.cleanup_temporary_files(args)
        sys.exit()

    limits.print_limits("planner", args.overall_time_limit,
                        args.overall_memory_limit)
    print()

    exitcode = None
    for component in args.components:
        if component == "translate":
            (exitcode, continue_execution) = run_components.run_translate(args)
        elif component == "search":
            (exitcode, continue_execution) = run_components.run_search(args)
            if not args.keep_sas_file:
                print("Remove intermediate file {}".format(args.sas_file))
                os.remove(args.sas_file)
        else:
            assert False, "Error: unhandled component: {}".format(component)
        print("{component} exit code: {exitcode}".format(**locals()))
        print()
        if not continue_execution:
            print("Driver aborting after {}".format(component))
            break

    try:
        logging.info(f"Planner time: {util.get_elapsed_time():.2f}s")
    except NotImplementedError:
        # Measuring the runtime of child processes is not supported on Windows.
        pass

    # Exit with the exit code of the last component that ran successfully.
    sys.exit(exitcode)


if __name__ == "__main__":
    main()
