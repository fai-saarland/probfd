## Dependencies
### Mandatory Dependencies

You need a C++ 20 compiler and CMake.
To run the planner, you also need Python 3.

**Linux/MacOS:** On these platforms, you also need GNU make.
  On Debian/Ubuntu the following should install all
  dependencies:
  ```
  sudo apt install cmake g++-12 make python3
  ```

On MacOS, all dependencies can be installed via Homebrew:
  ```
  brew install cmake llvm make python3
  ```

  Note that LLVM's clang is not symlinked to `/usr/bin/` or `/usr/local/bin/`,
  so the environment variables `CC` and `CXX` must be set before the build.
  This can be achieved by
  ```
  export CC=`brew --prefix llvm`/bin/clang
  export CXX=`brew --prefix llvm`/bin/clang++
  ```

**Windows:** Install [Visual Studio 2022](https://visualstudio.microsoft.com/de/downloads/),
[Python](https://www.python.org/downloads/windows/), and [CMake](http://www.cmake.org/download/).
Build Tools for Visual Studio 2022 can also be installed if the Visual Studio
IDE is not desired.
The C++ Desktop Development workload of Visual Studio installs both the
compiler, as well as CMake.


### Optional: Linear-Programming Solvers

Some planner configurations depend on an LP or MIP solver. We support CPLEX (commercial, [free academic license](http://ibm.com/academic)) and SoPlex (Apache License, no MIP support). You can install one or both solvers without causing conflicts.

Once LP solvers are installed and the environment variables `cplex_DIR` and/or `soplex_DIR` are set up correctly, Fast Downward automatically includes each solver detected on the system in the build.

#### Installing CPLEX

Obtain CPLEX and follow the guided installation. See [troubleshooting](#troubleshooting) if you have problems accessing the installer.
On Windows, install CPLEX into a directory without spaces.

After the installation, set the environment variable `cplex_DIR` to the subdirectory `/cplex` of the installation.
For example on Ubuntu:
```bash
export cplex_DIR=/opt/ibm/ILOG/CPLEX_Studio2211/cplex
```
Note that on Windows, setting up the environment variable might require using `/` instead of the more Windows-common `\`.


#### Installing SoPlex on Linux/macOS

**Important:**  The GNU Multiple Precision library (GMP) is critical for the performance of SoPlex but the build does not complain if it is not present.
Make sure that the build uses the library (check the output of CMake for `Found GMP`).

As of SoPlex 6.0.4, the release does not support C++-20, so we build  from the tip of the [GitHub main branch](https://github.com/scipopt/soplex) (adapt the path if you install a different version or want to use a different location):
```bash
sudo apt install libgmp3-dev
git clone https://github.com/scipopt/soplex.git
export soplex_DIR=/opt/soplex-6.0.4x
export CXXFLAGS="$CXXFLAGS -Wno-use-after-free" # Ignore compiler warnings about use-after-free
cmake -S soplex -B build
cmake --build build
cmake --install build --prefix $soplex_DIR
rm -rf soplex build
```

After installation, permanently set the environment variable `soplex_DIR` to the value you used during the installation.

**Note:** Once [support for C++-20](https://github.com/scipopt/soplex/pull/15) has been included in a SoPlex release, we can update this and can recommend the [SoPlex homepage](https://soplex.zib.de/index.php#download) for downloads instead.

## Compiling the planner

To build the planner, from the top-level directory run:

```bash
./build.py
```

This creates the default build `release` in the directory `builds`. For information on alternative builds (e.g. `debug`) and further options, call
`./build.py --help`. [Our website](https://www.fast-downward.org/ForDevelopers/CMake) has details on how to set up development builds.


### Compiling on Windows

Windows does not interpret the shebang in Python files, so you have to call `build.py` as `python3 build.py` (make sure `python3` is on your `PATH`).

Note that compiling from the terminal is only possible with the right environment. The easiest way to get such an environment is to use the `Developer PowerShell for VS 2022` or `Developer PowerShell`.

Alternatively, you can [create a Visual Studio Project](https://www.fast-downward.org/ForDevelopers/CMake#Custom_Builds), open it in Visual Studio and build from there. Visual Studio creates its binary files in subdirectories of the project that our driver script currently does not recognize. If you build with Visual Studio, you have to run the individual components of the planner yourself.

## Troubleshooting

* If you changed the build environment, delete the `builds` directory and rebuild.
* **Windows:** If you cannot execute the Fast Downward binary in a new command line, then it might be unable to find a dynamically linked library.
  Use `dumpbin /dependents PATH\TO\DOWNWARD\BINARY` to list all required libraries and ensure that they can be found in your `PATH` variable.
* **CPLEX:** After logging in at the IBM website, you find the Ilog studio software under Technology -> Data Science. Choose the right version and switch to HTTP download unless you have the IBM download manager installed. If you have problems using their website with Firefox, try Chrome instead. Execute the downloaded binary and follow the guided installation.
* **CPLEX:** If you get warnings about unresolved references with CPLEX, visit their [help pages](http://www-01.ibm.com/support/docview.wss?uid=swg21399926).
