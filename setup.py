#!/usr/bin/env python3
import shutil, os, sys, subprocess, pwd, getpass
import pathlib
import platform
from enum import Enum

# TODO: finish this two files
import Utils.Dependencies.common_system as common_system
import Utils.Dependencies.linux_x64 as linux_x64
import Utils.Dependencies.windows_x64 as windows_x64


class ModeRequested(Enum):
    INSTALL = 1
    REINSTALL = 2
    UNINSTALL = 3
    UNDEFINED = 4


# follows the enumation of OperatingSystem, so it can be called like binaries_paths_os[OperatingSystem.value - 1]
binaries_paths = ["undefined", "/usr/bin", "undefined", "undefined"]
private_shared_data_paths = ["undefined", "/usr/share/yeager", "undefined", "undefined"]
config_data_paths = ["undefined", "/.config/YeagerEngine", "undefined", "undefined"]
public_shared_data_paths = [
    "undefined",
    "/.local/share/yeager",
    "undefined",
    "undefined",
]
scriptMode = ModeRequested.UNDEFINED
# the script was involked with the --rootless option on, and a user name was given
rootless = False
system_user = None
upgrade_shared = False
forced_compilation = False


def compileEngine():
    home = os.getcwd()
    cmd = str("cmake --build " + home + "/build --config Debug --target all --")
    os.system(cmd)


def checkIfEngineIsCompiled():
    if not common_system.checkIfCmakeInstalled():
        print("[ERROR] Cmake isnt installed! Please install it from https://cmake.org/")
        sys.exit(0)
    else:
        print("[INFO] Cmake found!")

    if not os.path.isfile("YeagerEngine"):
        print("[INFO] Yeager Engine is not compiled! Compiling...")
        compileEngine()


def printHelp():
    print("Usage command: sudo python3 install.py [mode] [args] \nModes:")
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
    elif arg == "-rootless":
        user = common_system.safeFromArgv(n + 1)
        common_system.checkIfUserExists(user)
        global system_user
        system_user = user
        global rootless
        rootless = True
        return 2
    elif arg == "-upgrade_shared":
        global upgrade_shared
        upgrade_shared = True
    elif arg == "-compile_forced":
        global forced_compilation
        forced_compilation = True
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


def buildUserLocalSharedDir(user):
    if common_system.currentOperatingSystem == common_system.OperatingSystem.LINUX:
        return "/home/" + user + "/.local/share/applications"
    elif (
        common_system.currentOperatingSystem == common_system.OperatingSystem.WINDOWS_32
    ):
        return str("Undefined")


def localInstallUser(user):
    common_system.checkIfUserExists(user)
    shared = buildUserLocalSharedDir(system_user)
    shutil.copy("Utils/YeagerEngine.desktop", shared)
    print("[INFO] Moved Utils/YeagerEngine.desktop -> " + shared)


def copyBinariesGlobal():
    bin_path = binaries_paths[common_system.currentOperatingSystem.value - 1]

    if common_system.currentOperatingSystem == common_system.OperatingSystem.LINUX:
        code = common_system.cpSudoCommand("YeagerEngine", "/usr/bin")
        if code == 0:
            print("[INFO] Copied YeagerEngine -> " + bin_path)
            return
        print(
            "[ERROR] Cannot copied YeagerEngine -> {} Code returned {}".format(
                bin_path, code
            )
        )
    elif common_system.currentOperatingSystem == common_system.OperatingSystem.WINDOWS_32:
        bin_path = os.environ["ProgramFiles"] + "\\YeagerEngine"
        os.mkdir(bin_path)

def compartibleIconDir(home) -> str:
    if common_system.currentOperatingSystem == common_system.OperatingSystem.LINUX:
        icon_dir = home + "/.local/share/icons/hicolor/256x256/apps"
        if rootless and system_user is not None:
            icon_dir = (
                "/home/" + system_user + "/.local/share/icons/hicolor/256x256/apps"
            )
        return icon_dir
    elif (
        common_system.currentOperatingSystem == common_system.OperatingSystem.WINDOWS_32
    ):
        return str("undefined")


def solveIconDir(home):
    icon_dir = compartibleIconDir(home)
    if common_system.verifyDirExists(icon_dir):
        shutil.copy(common_system.pathCompatible("Utils/YeagerEngine.png"), icon_dir)
        print("[INFO] Copied Utils/YeagerEngine.png -> ", icon_dir)
        if not common_system.publicPermissions(icon_dir):
            print("[ERROR] cannot change permissions on ", icon_dir)
    else:
        print("[ERROR] Cannot copy YeagerEngine.png to ", icon_dir)


def copyDesktopInfo(home):
    if rootless and system_user is not None:
        localInstallUser(system_user)
    else:
        shutil.copy(
            "Utils/YeagerEngine.desktop", str(home + "/.local/share/applications")
        )
        print(
            "[INFO] Copied Utils/YeagerEngine.desktop -> "
            + str(home + "/.local/share/applications")
        )


def copySharedData(home):
    assets = common_system.pathCompatible("Apps/Yeager/Assets/")
    configuration = common_system.pathCompatible("Apps/Yeager/Configuration/")
    public_configuration = common_system.pathCompatible(
        "Apps/Yeager/Configuration/Editor/Public/"
    )

    private_shared = private_shared_data_paths[
        common_system.currentOperatingSystem.value - 1
    ]
    public_shared = public_shared_data_paths[
        common_system.currentOperatingSystem.value - 1
    ]

    if common_system.currentOperatingSystem == common_system.OperatingSystem.LINUX:
        global system_user
        public_shared = str(home) + str(public_shared)
    elif (
        common_system.currentOperatingSystem == common_system.OperatingSystem.WINDOWS_32
    ):
        # TODO:
        print("TODO")

    public_configuration_path = public_shared + "/Configuration"

    try:
        if upgrade_shared:
            common_system.sudoCommand("rm -r {}".format(private_shared))
            print("[INFO] Removed -> {} for upgrade!".format(private_shared))

        common_system.sudoCommand("mkdir {}".format(private_shared))
        print("[INFO] Created directory -> {}".format(private_shared))
        common_system.commandSystem("mkdir {}".format(public_shared))
        print("[INFO] Created directory -> {}".format(public_shared))
        common_system.commandSystem("mkdir {}".format(public_configuration_path))
        print("[INFO] Created directory -> {}".format(public_configuration_path))
        common_system.cpSudoCommand(src=assets, dst=private_shared, is_dir=True)
        common_system.cpSudoCommand(
            src=configuration, dst=private_shared, is_dir=True
        )
        common_system.cpCommand(
            src=public_configuration, dst=public_configuration_path, is_dir=True
        )
        print("[INFO] Copied {} -> {}".format(assets, private_shared))
        print("[INFO] Copied {} -> {}".format(configuration, private_shared))
        print(
            "[INFO] Copied {} -> {}".format(
                public_configuration, public_configuration_path
            )
        )
    except FileExistsError:
        print("[ERROR] Files exists, this error should not be raised!")


def installEngine():
    home = str(pathlib.Path.home())
    if forced_compilation:
        compileEngine()
    copyBinariesGlobal()
    copyDesktopInfo(home)
    copySharedData(home)
    solveIconDir(home)


def removeBinariesGlobal():
    removed = binaries_paths[common_system.currentOperatingSystem.value - 1]
    if not common_system.removeFile(removed + "/YeagerEngine"):
        print("[ERROR] Cannot remove binarie from ", removed + "/YeagerEngine")
    else:
        print("[INFO] Removed -> ", removed + "/YeagerEngine")


def localUninstallUser(user):
    common_system.checkIfUserExists(user)
    shared = buildUserLocalSharedDir(user) + common_system.pathCompatible(
        "/YeagerEngine.desktop"
    )
    if common_system.removeFile(shared):
        print("[INFO] Removed -> ", shared)
    else:
        print("[ERROR] Cannot remove -> ", shared)
        sys.exit(0)


def removeDesktopInfo(home):
    if rootless and system_user is not None:
        localUninstallUser(system_user)
    else:
        application = str(home + "/.local/share/applications/YeagerEngine.desktop")
        if not common_system.verifyFileExists(application):
            print(
                "[ERROR] Desktop application information does not exists! ", application
            )
        else:
            common_system.removeFile(application)
            print("[INFO] Removed -> ", application)


def removeSharedData():
    shared = private_shared_data_paths[common_system.currentOperatingSystem.value - 1]
    public_shared = public_shared_data_paths[
        common_system.currentOperatingSystem.value - 1
    ]
    home = str(pathlib.Path.home())
    public_shared = home + public_shared
    if not common_system.verifyDirExists(shared):
        print(
            "[ERROR] Shared data directory does not exists, cannot be removed! ",
            shared,
        )
    else:
        if common_system.removeDir(shared):
            print("[INFO] Removed -> ", shared)
        else:
            print("[ERROR] Cannot remove -> ", shared)
    if not common_system.verifyDirExists(public_shared):
        print(
            "[ERROR] Shared data directory does not exists, cannot be removed! ",
            public_shared,
        )
    else:
        if common_system.removeDir(public_shared):
            print("[INFO] Removed -> ", public_shared)
        else:
            print("[ERROR] Cannot remove -> ", public_shared)


def uninstallEngine():
    home = str(pathlib.Path.home())
    removeBinariesGlobal()
    removeDesktopInfo(home)
    removeSharedData()


def executeInstructions():
    if scriptMode == ModeRequested.INSTALL:
        print("[INFO] Installing Yeager Engine on " + platform.platform())
        installEngine()
    elif scriptMode == ModeRequested.UNINSTALL:
        print("[INFO] Uninstalling Yeager Engine on " + platform.platform())
        uninstallEngine()
    elif scriptMode == ModeRequested.REINSTALL:
        print("[INFO] Reinstalling Yeager Engine on " + platform.platform())
        uninstallEngine()
        installEngine()
    else:
        print("[ERROR] Non gived mode requested on " + platform.platform())
        sys.exit(0)


common_system.password = common_system.getSudoPassword()
common_system.checkPlatform()
processArguments()
checkIfEngineIsCompiled()
executeInstructions()
