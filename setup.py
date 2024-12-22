#!/usr/bin/env python3
import os, sys, platform
from enum import Enum

import Utilities.Dependencies.common_system as common_system

import Engine.Binaries.Build.Python.gen_cmakelists as gcmake
import Engine.Binaries.Build.Python.dependecies as dep


class ModeRequested(Enum):
    INSTALL = 1
    REINSTALL = 2
    UNINSTALL = 3
    BUILD = 4
    UNDEFINED = 5


scriptMode = ModeRequested.UNDEFINED


def PromptHelp():
    print("Usage command: python3 setup.py [mode] [args] \nModes:")
    print("install -- Installs the Engine to the system", end="\n")
    print("reinstall -- Reinstals and cleans the system from the Engine", end="\n")
    print("uninstall -- Uninstall the Engine from the system, and cleans", end="\n")
    print("build -- Build the Engine using cmake and run", end="\n")


def IterateGivenArguments(arg, n) -> int:
    global scriptMode
    if arg == "install":
        scriptMode = ModeRequested.INSTALL
    elif arg == "reinstall":
        scriptMode = ModeRequested.REINSTALL
    elif arg == "uninstall":
        scriptMode = ModeRequested.UNINSTALL
    elif arg == "build":
        scriptMode = ModeRequested.BUILD
    else:
        print("[IGNORE] Argument invalid " + arg)
    return 1


def ProcessAllArguments():

    if len(sys.argv) > 1:
        if str(common_system.SafeFromArgv(1)) == "help":
            PromptHelp()
            sys.exit(0)

    it = 1
    while it < len(sys.argv):
        it += IterateGivenArguments(str(sys.argv[it]), it)


def ExeInstructions():
    if scriptMode == ModeRequested.INSTALL:
        print("[INFO] Installing Yeager Engine on " + platform.platform())

        if os.name == "posix":
            import Utilities.Dependencies.linux_x64 as linux_x64

            linux_x64.InstallEngineLinux()
        if os.name == "nt":
            import Utilities.Dependencies.windows_x64 as windows_x64

            windows_x64.InstallEngineWindows()


if __name__ == "__main__":
    dep.InstallDependecies()
    common_system.CheckOSPlatform()
    ProcessAllArguments()
    ExeInstructions()
