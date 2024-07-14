#!/usr/bin/env python3
import shutil, os, sys, subprocess, pwd, getpass
import pathlib
import platform
from enum import Enum

# TODO: finish this two files
import Utils.Dependencies.common_system as common_system
import Utils.Dependencies.linux_x64 as linux


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


def _compile_engine():
    home = os.getcwd()
    cmd = str("cmake --build " + home + "/build --config Debug --target all --")
    os.system(cmd)


def _check_if_engine_compiled():
    if not common_system._check_if_cmake_installed():
        print("[ERROR] Cmake isnt installed! Please install it from https://cmake.org/")
        sys.exit(0)
    else:
        print("[INFO] Cmake found!")

    if not os.path.isfile("YeagerEngine"):
        print("[INFO] Yeager Engine is not compiled! Compiling...")
        _compile_engine()


def _print_help():
    print("Usage command: sudo python3 install.py [mode] [args] \nModes:")
    print("install -- Installs the Engine to the system", end="\n")
    print("reinstall -- Reinstals and cleans the system from the Engine", end="\n")
    print("uninstall -- Uninstall the Engine from the system, and cleans", end="\n")


def _iterate_args(arg, n) -> int:
    global scriptMode
    if arg == "install":
        scriptMode = ModeRequested.INSTALL
    elif arg == "reinstall":
        scriptMode = ModeRequested.REINSTALL
    elif arg == "uninstall":
        scriptMode = ModeRequested.UNINSTALL
    elif arg == "-rootless":
        user = common_system._safe_from_argv(n + 1)
        common_system._check_if_user_exists(user)
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


def _process_args():

    if len(sys.argv) > 1:
        if str(common_system._safe_from_argv(1)) == "help":
            _print_help()
            sys.exit(0)

    it = 1
    while it < len(sys.argv):
        it += _iterate_args(str(sys.argv[it]), it)


def _build_user_local_shared_dir(user):
    if common_system.currentOperatingSystem == common_system.OperatingSystem.LINUX:
        return "/home/" + user + "/.local/share/applications"
    elif (
        common_system.currentOperatingSystem == common_system.OperatingSystem.WINDOWS_32
    ):
        return str("Undefined")


def _local_install_user(user):
    common_system._check_if_user_exists(user)
    shared = _build_user_local_shared_dir(system_user)
    shutil.copy("Utils/YeagerEngine.desktop", shared)
    print("[INFO] Moved Utils/YeagerEngine.desktop -> " + shared)


def _mv_binaries_global():
    dir_global = binaries_paths[common_system.currentOperatingSystem.value - 1]
    code = common_system._cp_sudo_command("YeagerEngine", "/usr/bin")
    if code == 0:
        print("[INFO] Copied YeagerEngine -> " + dir_global)
        return
    print(
        "[ERROR] Cannot copied YeagerEngine -> {} Code returned {}".format(
            dir_global, code
        )
    )


def _compartible_icon_dir(home) -> str:
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


def _solve_icon_dir(home):
    icon_dir = _compartible_icon_dir(home)
    if common_system._verify_if_dir_exists(icon_dir):
        shutil.copy(common_system._path_compatible("Utils/YeagerEngine.png"), icon_dir)
        print("[INFO] Copied Utils/YeagerEngine.png -> ", icon_dir)
        if not common_system._public_permissions(icon_dir):
            print("[ERROR] cannot change permissions on ", icon_dir)
    else:
        print("[ERROR] Cannot copy YeagerEngine.png to ", icon_dir)


def _mv_desktop_info(home):
    if rootless and system_user is not None:
        _local_install_user(system_user)
    else:
        shutil.copy(
            "Utils/YeagerEngine.desktop", str(home + "/.local/share/applications")
        )
        print(
            "[INFO] Copied Utils/YeagerEngine.desktop -> "
            + str(home + "/.local/share/applications")
        )


def _mv_shared_data(home):
    assets = common_system._path_compatible("Apps/Yeager/Assets/")
    configuration = common_system._path_compatible("Apps/Yeager/Configuration/")
    public_configuration = common_system._path_compatible(
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
            common_system._sudo_command("rm -r {}".format(private_shared))
            print("[INFO] Removed -> {} for upgrade!".format(private_shared))

        common_system._sudo_command("mkdir {}".format(private_shared))
        print("[INFO] Created directory -> {}".format(private_shared))
        common_system._command_system("mkdir {}".format(public_shared))
        print("[INFO] Created directory -> {}".format(public_shared))
        common_system._command_system("mkdir {}".format(public_configuration_path))
        print("[INFO] Created directory -> {}".format(public_configuration_path))
        common_system._cp_sudo_command(src=assets, dst=private_shared, is_dir=True)
        common_system._cp_sudo_command(
            src=configuration, dst=private_shared, is_dir=True
        )
        common_system._cp_command(
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


def _install_engine():
    home = str(pathlib.Path.home())
    if forced_compilation:
        _compile_engine()
    _mv_binaries_global()
    _mv_desktop_info(home)
    _mv_shared_data(home)
    _solve_icon_dir(home)


def _rm_binaries_global():
    removed = binaries_paths[common_system.currentOperatingSystem.value - 1]
    if not common_system._remove_file(removed + "/YeagerEngine"):
        print("[ERROR] Cannot remove binarie from ", removed + "/YeagerEngine")
    else:
        print("[INFO] Removed -> ", removed + "/YeagerEngine")


def _local_uninstall_user(user):
    common_system._check_if_user_exists(user)
    shared = _build_user_local_shared_dir(user) + common_system._path_compatible(
        "/YeagerEngine.desktop"
    )
    if common_system._remove_file(shared):
        print("[INFO] Removed -> ", shared)
    else:
        print("[ERROR] Cannot remove -> ", shared)
        sys.exit(0)


def _rm_desktop_info(home):
    if rootless and system_user is not None:
        _local_uninstall_user(system_user)
    else:
        application = str(home + "/.local/share/applications/YeagerEngine.desktop")
        if not common_system._verify_if_exits(application):
            print(
                "[ERROR] Desktop application information does not exists! ", application
            )
        else:
            common_system._remove_file(application)
            print("[INFO] Removed -> ", application)


def _rm_shared_data():
    shared = private_shared_data_paths[common_system.currentOperatingSystem.value - 1]
    public_shared = public_shared_data_paths[
        common_system.currentOperatingSystem.value - 1
    ]
    home = str(pathlib.Path.home())
    public_shared = home + public_shared
    if not common_system._verify_if_dir_exists(shared):
        print(
            "[ERROR] Shared data directory does not exists, cannot be removed! ",
            shared,
        )
    else:
        if common_system._remove_dir(shared):
            print("[INFO] Removed -> ", shared)
        else:
            print("[ERROR] Cannot remove -> ", shared)
    if not common_system._verify_if_dir_exists(public_shared):
        print(
            "[ERROR] Shared data directory does not exists, cannot be removed! ",
            public_shared,
        )
    else:
        if common_system._remove_dir(public_shared):
            print("[INFO] Removed -> ", public_shared)
        else:
            print("[ERROR] Cannot remove -> ", public_shared)


def _uninstall_engine():
    home = str(pathlib.Path.home())
    _rm_binaries_global()
    _rm_desktop_info(home)
    _rm_shared_data()


def _execute_instructions():
    if scriptMode == ModeRequested.INSTALL:
        print("[INFO] Installing Yeager Engine on " + platform.platform())
        _install_engine()
    elif scriptMode == ModeRequested.UNINSTALL:
        print("[INFO] Uninstalling Yeager Engine on " + platform.platform())
        _uninstall_engine()
    elif scriptMode == ModeRequested.REINSTALL:
        print("[INFO] Reinstalling Yeager Engine on " + platform.platform())
        _uninstall_engine()
        _install_engine()
    else:
        print("[ERROR] Non gived mode requested on " + platform.platform())
        sys.exit(0)


common_system.password = common_system._get_sudo_password()
common_system._check_platform()
_process_args()
_check_if_engine_compiled()
_execute_instructions()
