import shutil, os, sys, subprocess, pwd
import pathlib
import platform
from enum import Enum


class ModeRequested(Enum):
    INSTALL = 1
    REINSTALL = 2
    UNINSTALL = 3
    UNDEFINED = 4


class OperatingSystem(Enum):
    WINDOWS_32 = 1
    LINUX = 2
    DARWIN = 3
    UNDEFINED = 4


# follows the enumation of OperatingSystem, so it can be called like binaries_paths_os[OperatingSystem.value - 1]
binaries_paths_os = ["undefined", "/usr/bin", "undefined", "undefined"]
share_data = ["undefined", "/usr/share", "undefined", "undefined"]
config_data = ["undefined", "/.config/YeagerEngine", "undefined", "undefined"]
currentOperatingSystem = OperatingSystem.UNDEFINED
scriptMode = ModeRequested.UNDEFINED
# the script was involked with the --rootless option on, and a user name was given
rootless = False
unix_user = None


def _verify_if_exits(path) -> bool:
    return os.path.isfile(str(path))


def _verify_if_dir_exists(dir) -> bool:
    return os.path.isdir(str(dir))


def _remove_file(path) -> bool:
    if _verify_if_exits(path):
        os.remove(path)
        return True
    return False


def _is_root() -> bool:
    return os.geteuid() == 0


# retrieves a path in the unix format of /folder/file and converts to the windows version \folder\file
def _path_compatible(path) -> str:
    if currentOperatingSystem == OperatingSystem.WINDOWS_32:
        path = str(path).replace("/", "\\")
        return path
    return path


def _print_help():
    print("Usage command: sudo python3 install.py [mode] [args] \nModes:")
    print("install -- Installs the Engine to the system", end="\n")
    print("reinstall -- Reinstals and cleans the system from the Engine", end="\n")
    print("uninstall -- Uninstall the Engine from the system, and cleans", end="\n")


def _check_platform():
    global currentOperatingSystem
    if platform.system() == "Linux":
        currentOperatingSystem = OperatingSystem.LINUX
    elif platform.system() == "Darwin":
        currentOperatingSystem = OperatingSystem.DARWIN
    elif platform.system() == "Windows":
        currentOperatingSystem = OperatingSystem.WINDOWS_32
    else:
        print("[ERROR] Unknown operating system!")
        sys.exit(0)


def _safe_from_argv(n) -> str:
    if not n > len(sys.argv) - 1:
        return sys.argv[n]
    print(
        "Trying to receive out of index from sys.argv! lenght of argv = "
        + str(len(sys.argv))
        + " index = "
        + str(n - 1)
    )
    return None


def _iterate_args(arg, n) -> int:
    global scriptMode
    if arg == "install":
        scriptMode = ModeRequested.INSTALL
    elif arg == "reinstall":
        scriptMode = ModeRequested.REINSTALL
    elif arg == "uninstall":
        scriptMode = ModeRequested.UNINSTALL
    elif arg == "-rootless":
        user = _safe_from_argv(n + 1)
        _check_if_user_exists(user)
        global unix_user
        unix_user = user
        global rootless
        rootless = True
        return 2
    else:
        print("[IGNORE] Argument invalid " + arg)
    return 1


def _process_args():

    if not _is_root():
        print("[ERROR] This script need administrator privileges to run properly")
        sys.exit(0)

    if len(sys.argv) > 1:
        if str(_safe_from_argv(1)) == "help":
            _print_help()
            sys.exit(0)

    it = 1
    while it < len(sys.argv):
        it += _iterate_args(str(sys.argv[it]), it)


def _check_if_user_exists(user):
    try:
        pwd.getpwnam(user)
    except KeyError:
        print("User " + user + " does not exits")
        sys.exit(0)


def _build_user_local_shared_dir(user):
    if currentOperatingSystem == OperatingSystem.LINUX:
        return "/home/" + user + "/.local/share/applications"
    elif currentOperatingSystem == OperatingSystem.WINDOWS_32:
        return str("Undefined")


def _local_install_user(user):
    _check_if_user_exists(user)
    shared = _build_user_local_shared_dir(unix_user)
    shutil.copy("Utils/YeagerEngine.desktop", shared)
    print("[INFO] Moved Utils/YeagerEngine.desktop -> " + shared)


def _check_if_cmake_installed() -> bool:
    return (
        subprocess.call(
            ["cmake", "--version"],
            shell=False,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.STDOUT,
        )
        == 0
    )


def _public_permissions(file: str) -> bool:
    return (
        subprocess.call(
            ["sudo", "chmod", "666", file],
            shell=False,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.STDOUT,
        )
        == 0
    )


def _check_if_engine_compiled():
    if not _check_if_cmake_installed():
        print("[ERROR] Cmake isnt installed! Please install it from https://cmake.org/")
        sys.exit(0)
    else:
        print("[INFO] Cmake found!")

    if not os.path.isfile("YeagerEngine"):
        print("[INFO] Yeager Engine is not compiled! Compiling...")
        home = os.getcwd()
        cmd = str("cmake --build " + home + "/build --config Debug --target all --")
        os.system(cmd)


def _mv_binaries_global():
    dir_global = binaries_paths_os[currentOperatingSystem.value - 1]
    shutil.copy("YeagerEngine", dir_global)
    print("[INFO] Moved YeagerEngine -> " + dir_global)


def _compartible_icon_dir(home) -> str:
    if currentOperatingSystem == OperatingSystem.LINUX:
        icon_dir = home + ".local/share/icons/hicolor/256x256/apps"
        if rootless and unix_user is not None:
            icon_dir = "/home/" + unix_user + "/.local/share/icons/hicolor/256x256/apps"
        return icon_dir
    elif currentOperatingSystem == OperatingSystem.WINDOWS_32:
        return str("undefined")


def _solve_icon_dir(home):
    icon_dir = _compartible_icon_dir(home)
    if _verify_if_dir_exists(icon_dir):
        shutil.copy(_path_compatible("Utils/YeagerEngine.png"), icon_dir)
        print("[INFO] Moved Utils/YeagerEngine.png -> ", icon_dir)
        if not _public_permissions(icon_dir):
            print("[ERROR] cannot change permissions on ", icon_dir)
    else:
        print("[ERROR] Cannot moved YeagerEngine.png to ", icon_dir)


def _install_engine():
    home = str(pathlib.Path.home())
    _mv_binaries_global()
    if rootless and unix_user is not None:
        _local_install_user(unix_user)
    else:
        shutil.copy(
            "Utils/YeagerEngine.desktop", str(home + "/.local/share/applications")
        )
        print(
            "[INFO] Moved Utils/YeagerEngine.desktop -> "
            + str(home + "/.local/share/applications")
        )

    assets = _path_compatible("Apps/Yeager/Assets/")
    configuration = _path_compatible("Apps/Yeager/Configuration/")

    try:
        shutil.copytree(src=assets, dst="/usr/share/Yeager/Assets", dirs_exist_ok=True)
        shutil.copytree(
            src=configuration, dst="/usr/share/Yeager/Configuration", dirs_exist_ok=True
        )
    except FileExistsError:
        print("[ERROR] Files exists, this error should not be raised!")

    _solve_icon_dir(home)


def _rm_binaries_global():
    removed = binaries_paths_os[currentOperatingSystem.value - 1]
    if not _remove_file(removed + "/YeagerEngine"):
        print("[ERROR] Cannot remove binarie from ", removed + "/YeagerEngine")
    else:
        print("[INFO] Removed -> ", removed + "/YeagerEngine")


def _local_uninstall_user(user):
    _check_if_user_exists(user)
    shared = _build_user_local_shared_dir(user) + _path_compatible(
        "/YeagerEngine.desktop"
    )
    if _remove_file(shared):
        print("[INFO] Removed -> ", shared)
    else:
        print("[ERROR] Cannot remove -> ", shared)
        sys.exit(0)


def _uninstall_engine():
    home = str(pathlib.Path.home())
    _rm_binaries_global()

    if rootless and unix_user is not None:
        _local_uninstall_user(unix_user)
    else:
        application = str(home + "/.local/share/applications/YeagerEngine.desktop")
        if not _verify_if_exits(application):
            print(
                "[ERROR] Desktop application information does not exists! ", application
            )
        else:
            _remove_file(application)
            print("[INFO] Removed -> ", application)

    shared = share_data[currentOperatingSystem.value - 1] + _path_compatible("/Yeager")
    if not _verify_if_dir_exists(shared):
        print(
            "[ERROR] Shared data directory does not exists, cannot be removed! ",
            shared,
        )
    else:
        shutil.rmtree(shared, ignore_errors=True)
        print("[INFO] Removed -> ", shared)


def _execute_instructions():
    if scriptMode == ModeRequested.INSTALL:
        print("[INFO] Installing Yeager Engine on " + platform.platform())
        _install_engine()
    elif scriptMode == ModeRequested.UNINSTALL:
        print("[INFO] Uninstalling Yeager Engine on " + platform.platform())
        _uninstall_engine()
    else:
        print("[ERROR] Non gived mode requested on " + platform.platform())
        sys.exit(0)


_check_platform()
_process_args()
_check_if_engine_compiled()
_execute_instructions()
