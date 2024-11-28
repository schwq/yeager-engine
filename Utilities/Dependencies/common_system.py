#!/usr/bin/env python3
import shutil, os, sys, subprocess, getpass
import pathlib
import platform
from enum import Enum


class OperatingSystem(Enum):
    WINDOWS_32 = 1
    LINUX = 2
    DARWIN = 3
    UNDEFINED = 4


gCurrentOperatingSystem = OperatingSystem.UNDEFINED


class FolderClass:
    def __init__(self, name: str, folders: list[any] | None = []) -> None:
        self.name = name
        self.folders = folders


gPrivateFolders = FolderClass(
    "YeagerEngine",
    [
        FolderClass("Binarie", []),
        FolderClass("Utilities", [FolderClass("Setup", [])]),
        FolderClass(
            "Configuration",
            [
                FolderClass(
                    "Theme", [FolderClass("Colorscheme", []), FolderClass("Sound", [])]
                ),
                FolderClass("Shader", []),
            ],
        ),
        FolderClass(
            "Resources",
            [
                FolderClass("Shaders", []),
                FolderClass("Icons", []),
                FolderClass("Textures", [FolderClass("Default")]),
                FolderClass("Fonts", [FolderClass("Editor", [])]),
                FolderClass("Locales", []),
                FolderClass("Sound", []),
            ],
        ),
        FolderClass(
            "Templates",
            [
                FolderClass(
                    "TP_EmptyScene",
                    [
                        FolderClass("Assets", [FolderClass("Skybox", [])]),
                        FolderClass("Configuration", []),
                    ],
                )
            ],
        ),
    ],
)

gPublicFolders = FolderClass(
    "YeagerEngine",
    [
        FolderClass(
            "Resources",
            [
                FolderClass("Sound", []),
                FolderClass("Shaders", []),
                FolderClass("Locales", []),
            ],
        ),
        FolderClass(
            "Configuration",
            [
                FolderClass("Projects", []),
                FolderClass("Variables", []),
                FolderClass(
                    "Themes", [FolderClass("Colorscheme", []), FolderClass("Sound", [])]
                ),
                FolderClass("Interface", []),
                FolderClass("Shaders", []),
            ],
        ),
        FolderClass("Extensions", [FolderClass("ModuleAPI", [])]),
        FolderClass("Versions", [FolderClass("1.0.0-dev", [])]),
        FolderClass(
            "Cache",
            [
                FolderClass(
                    "Resources", [FolderClass("Textures", []), FolderClass("Sound", [])]
                ),
            ],
        ),
        FolderClass("Template", [FolderClass("Custom", [])]),
        FolderClass("Logs", [FolderClass("CrashReport", [])]),
    ],
)


def CompileEngine():
    home = os.getcwd()
    cmd = str("cmake --build " + home + "/build --config Debug --target all --")
    os.system(cmd)


def CopyFilesFromDir(src: str, dst: str, anounce: bool = False):
    files = os.listdir(src)
    for file in files:
        shutil.copy(os.path.join(src, file), dst)
        if anounce:
            print("[INFO] Copied {} from {} to {}".format(file, src, dst))


def VerifyFileExists(path) -> bool:
    return os.path.isfile(str(path))


def VerifyDirExists(dir) -> bool:
    return os.path.isdir(str(dir))


def VerifyItemExists(path) -> tuple[bool, str]:
    if VerifyDirExists(path):
        return True, "Dir"
    if VerifyFileExists(path):
        return True, "File"
    return False, None


# retrieves a path in the unix format of /folder/file and converts to the windows version \folder\file
def GetPathCompatible(path) -> str:
    if gCurrentOperatingSystem == OperatingSystem.WINDOWS_32:
        path = str(path).replace("/", "\\")
        return path
    return path


def CheckOSPlatform():
    global gCurrentOperatingSystem
    if platform.system() == "Linux":
        gCurrentOperatingSystem = OperatingSystem.LINUX
    elif platform.system() == "Darwin":
        gCurrentOperatingSystem = OperatingSystem.DARWIN
    elif platform.system() == "Windows":
        gCurrentOperatingSystem = OperatingSystem.WINDOWS_32
    else:
        print("[ERROR] Unknown operating system!")
        sys.exit(0)


def SafeFromArgv(n) -> str:
    if not n > len(sys.argv) - 1:
        return sys.argv[n]
    print(
        "Trying to receive out of index from sys.argv! lenght of argv = "
        + str(len(sys.argv))
        + " index = "
        + str(n - 1)
    )
    return None


def CheckIfUserExists(user):
    try:
        if os.name == "posix":
            import pwd

            pwd.getpwnam(user)
    except KeyError:
        print("User " + user + " does not exits")
        sys.exit(0)


def CheckIfCmakeInstalled() -> bool:
    return (
        subprocess.call(
            ["cmake", "--version"],
            shell=False,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.STDOUT,
        )
        == 0
    )


def CmdSystem(command: str) -> int:
    return os.system(command)


def validateCommand(command: str) -> tuple[bool, str]:
    n, msg = subprocess.getstatusoutput(command)
    return n == 0, msg


def copyType(src, dst):
    try:
        shutil.copytree(src=src, dst=dst, dirs_exist_ok=True)
    except PermissionError:
        print("[ERROR] Permissions error on {} to {}".format(src, dst))
        sys.exit(0)


def changeDirectory(dir: str):
    if VerifyDirExists(dir):
        os.chdir(dir)


def gitCloneLink(link: str, dir: str = None) -> bool:
    current = os.getcwd()
    changeDirectory(dir)
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
    changeDirectory(current)
    return process
