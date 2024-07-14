#!/usr/bin/env python3
import shutil, os, sys, subprocess, pwd, getpass
import pathlib
import platform
from enum import Enum


class OperatingSystem(Enum):
    WINDOWS_32 = 1
    LINUX = 2
    DARWIN = 3
    UNDEFINED = 4


password = None
currentOperatingSystem = OperatingSystem.UNDEFINED


# for files
def _verify_if_exits(path) -> bool:
    return os.path.isfile(str(path))


# for directories
def _verify_if_dir_exists(dir) -> bool:
    return os.path.isdir(str(dir))


def _remove_file(path) -> bool:
    if _verify_if_exits(path) and _is_root():
        os.remove(path)
        return True
    elif _verify_if_exits(path):
        _sudo_command("rm -r {}".format(path))
        return True
    return False


def _remove_dir(dir) -> bool:
    if _verify_if_dir_exists(dir) and _is_root():
        shutil.rmtree(dir, ignore_errors=True)
        return True
    elif _verify_if_dir_exists(dir):
        if len(os.listdir(dir)) > 0:
            _sudo_command("rm -r {}".format(dir))
        else:
            _sudo_command("rmdir {}".format(dir))
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


def _get_sudo_password() -> str:
    password = getpass.getpass(prompt="Enter sudo password: ")
    process = subprocess.Popen(
        ["sudo", "-S", "ls"],
        stderr=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stdin=subprocess.PIPE,
    )
    try:
        out, err = process.communicate(input=(password + "\n").encode(), timeout=5)
    except subprocess.TimeoutExpired:
        process.kill()
        print("[ERROR] Timeout awaiting the password!")
        sys.exit(0)
    return password


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


def _check_if_user_exists(user):
    try:
        pwd.getpwnam(user)
    except KeyError:
        print("User " + user + " does not exits")
        sys.exit(0)


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
            ["sudo", "chmod", "777", file],
            shell=False,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.STDOUT,
        )
        == 0
    )


def _sudo_command(command: str) -> int:
    global password
    cmd = "echo {}|sudo -S {}".format(password, command)
    if password is not None:
        return _command_system(cmd)
    print("[ERROR] Password not defined")
    return -1


def _command_system(command: str) -> int:
    return os.system(command)


def _validade_command(command: str) -> tuple[bool, str]:
    n, msg = subprocess.getstatusoutput(command)
    return n == 0, msg


# dont use the fucking mv command, or the directories and the files will dissapear from the engine source code
def _cp_sudo_command(src: str, dst: str, is_dir=False) -> int:
    command = "cp {} {}".format(src, dst)
    if is_dir:
        command = "cp -r {} {}".format(src, dst)
    return _sudo_command(command)


def _cp_command(src: str, dst: str, is_dir=False) -> int:
    command = "cp {} {}".format(src, dst)
    if is_dir:
        command = "cp -r {} {}".format(src, dst)
    return _command_system(command)


def _copy_tree(src, dst):
    try:
        shutil.copytree(src=src, dst=dst, dirs_exist_ok=True)
    except PermissionError:
        print("[ERROR] Permissions error on {} to {}".format(src, dst))
        sys.exit(0)


def _change_dir(dir: str):
    if _verify_if_dir_exists(dir):
        os.chdir(dir)


def _git_clone_link(link: str, dir: str = None) -> bool:
    current = os.getcwd()
    _change_dir(dir)
    print("[INFO] Cloning {} on {}".format(link, dir))
    process = (
        subprocess.call(
            ["git", "clone", link],
            shell=False,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.STDOUT,
        )
        == 0
    )
    _change_dir(current)
    return process
