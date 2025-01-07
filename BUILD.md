# Getting Started

[TOC]

## Obtaining the Planner

To obtain the planner, clone it from the official [GitHub repository](https://github.com/fai-saarland/probfd).

```
# Via https
git clone https://github.com/fai-saarland/probfd.git

# Via ssh
git clone git@github.com:fai-saarland/probfd.git
```

## Build Instructions

### Mandatory Dependencies

You need a C++ 23 compiler, CMake, the Ninja build system and Python 3.

**Linux:** On Debian/Ubuntu the following should install all dependencies:

```
sudo apt install g++-14 cmake ninja-build python3
```

**MacOS**: All dependencies can be installed via Homebrew:

```
brew install llvm cmake ninja python3
```

**Windows:** We recommend to use Windows' native package manager winget.
We provide a configuration file for the installation of Build Tools for Visual
Studio 2022 (`misc/vsconfig.json`) that installs everything needed for C++
development. If Visual Studio is already installed, you can skip the
installation.

```
winget install Microsoft.VisualStudio.2022.BuildTools --custom "--config misc/vsconfig.json"
winget install Kitware.CMake Ninja-build.Ninja Python.Python.3.13
```

### Optional: Linear-Programming Solvers

Some planner configurations depend on an LP or MIP solver. We support CPLEX (
commercial, [free academic license](http://ibm.com/academic)) and SoPlex (Apache
License, no MIP support). You can install one or both solvers without causing
conflicts.

Once LP solvers are installed and the environment variables `cplex_DIR`
and/or `soplex_DIR` are set up correctly, Fast Downward automatically includes
each solver detected on the system in the build.

#### Installing CPLEX

Obtain CPLEX and follow the guided installation.
See [troubleshooting](#troubleshooting) if you have problems accessing the
installer.
On Windows, install CPLEX into a directory without spaces.

After the installation, set the environment variable `cplex_DIR` to the
subdirectory `/cplex` of the installation.
For example on Ubuntu:

```bash
export cplex_DIR=/opt/ibm/ILOG/CPLEX_Studio2211/cplex
```

Note that on Windows, setting up the environment variable might require
using `/` instead of the more Windows-common `\ `.

#### Installing SoPlex on Linux/macOS

**Important:** The GNU Multiple Precision library (GMP) is critical for the
performance of SoPlex but the build does not complain if it is not present.
Make sure that the build uses the library (check the output of CMake for
`Found GMP`).

We require at least SoPlex 7.1.0, which can be built from source as follows
(adapt the paths if you install a different version or want to use a different
location):

```bash
sudo apt install libgmp3-dev
wget https://github.com/scipopt/soplex/archive/refs/tags/release-710.tar.gz -O - | tar -xz
cmake -S soplex-release-710 -B build
cmake --build build
export soplex_DIR=/opt/soplex-7.1.0
cmake --install build --prefix $soplex_DIR
rm -rf soplex-release-710 build
```

After installation, permanently set the environment variable soplex_DIR to the
value you used during the installation.

### Compiling the Planner

#### Configuring the Build

The planner has different configurations which allow to
exclude specific planner components from compilation.
For further information, type:

```bash
cmake --list-presets
```

First, configure the build for a specific configuration, for
example the `default` configuration that includes all planner
components:

```bash
cmake --preset default
```

#### Building

Each configuration can be built in either `Debug` or `Release` mode.
In debug mode, optimizations are disabled, debugging symbols are
embedded into the executable and assertions are enabled.
In release mode, optimizations are enabled, no debugging symbols
are omitted, and assertions are turned off.
After the build has been configured, run the compilation
process for the chosen configuration as follows:

```bash
# Builds configuration 'default' in release mode.
cmake --build --preset default-release

# Builds configuration 'tests' in debug mode.
cmake --build --preset tests-debug
```

## Running the Planner

After the planner has compiled successfully, you can run it using the
wrapper script `fast_downward.py`.
For further information:

```bash
# Prints a help message.
./fast_downward.py --help
```

## Running the Tests (Optional)

To assert that the planner functions correctly, the tests
of the planner can be run as follows after successfully compiling the
`tests` configuration in release mode:

```bash
# Runs all unit tests.
ctest --preset all_tests
```

## Troubleshooting

* **CPLEX:** After logging in at the IBM website, you find the Ilog studio
  software under Technology -> Data Science. Choose the right version and switch
  to HTTP download unless you have the IBM download manager installed. If you
  have problems using their website with Firefox, try Chrome instead. Execute
  the downloaded binary and follow the guided installation.
* **CPLEX:** If you get warnings about unresolved references with CPLEX, visit
  their [help pages](http://www-01.ibm.com/support/docview.wss?uid=swg21399926).
