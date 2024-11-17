#!/usr/bin/env python3
import os, sys, platform
from enum import Enum

import Utils.Dependencies.common_system as common_system


class ModeRequested(Enum):
    INSTALL = 1
    REINSTALL = 2
    UNINSTALL = 3
    UNDEFINED = 4


scriptMode = ModeRequested.UNDEFINED


def printHelp():
    print("Usage command: python3 setup.py [mode] [args] \nModes:")
    print("install -- Installs the Engine to the system", end="\n")
    print("reinstall -- Reinstals and cleans the system from the Engine", end="\n")
    print("uninstall -- Uninstall the Engine from the system, and cleans", end="\n")


def iterateArguments(arg, n) -> int:
    global scriptMode
    if arg == "install":
        scriptMode = ModeRequested.INSTALL
    elif arg == "reinstall":
        scriptMode = ModeRequested.REINSTALL
    elif arg == "uninstall":
        scriptMode = ModeRequested.UNINSTALL
    else:
        print("[IGNORE] Argument invalid " + arg)
    return 1


def processArguments():

    if len(sys.argv) > 1:
        if str(common_system.safeFromArgv(1)) == "help":
            printHelp()
            sys.exit(0)

    it = 1
    while it < len(sys.argv):
        it += iterateArguments(str(sys.argv[it]), it)


def executeInstructions():
    if scriptMode == ModeRequested.INSTALL:
        print("[INFO] Installing Yeager Engine on " + platform.platform())

        if os.name == "posix":
            import Utils.Dependencies.linux_x64 as linux_x64

            linux_x64.installEngineLinux()
        if os.name == "nt":
            import Utils.Dependencies.windows_x64 as windows_x64

            windows_x64.installEngineWindows()


common_system.checkPlatform()
processArguments()
executeInstructions()
