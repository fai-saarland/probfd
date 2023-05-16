#!/usr/bin/env python3

import os
import sys
import subprocess

HOMEBREW_PREFIX = subprocess.check_output(["brew", "--prefix"], text=True).strip("\n")

print(HOMEBREW_PREFIX)

LLVM_BASE = os.path.join(HOMEBREW_PREFIX, "opt", "llvm")

print(LLVM_BASE)

if __name__ == "__main__":
    e = dict(os.environ)
    e['CC'] = os.path.join(LLVM_BASE, "bin", "clang")
    e['CXX'] = os.path.join(LLVM_BASE, "bin", "clang++")
    e['LDFLAGS'] = f"-L{LLVM_BASE}/lib/c++ -Wl,-rpath,{LLVM_BASE}/lib/c++"
    e['LIBCXX_INCLUDE'] = os.path.join(LLVM_BASE, "include", "c++", "v1")
    
    subprocess.run(["./build.py", *sys.argv[1:]], env=e)
