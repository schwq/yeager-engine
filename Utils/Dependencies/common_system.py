#!/usr/bin/env python3
import shutil, os, sys, subprocess, getpass
import pathlib
import platform
from enum import Enum
if os.name == "posix": import pwd

import linux_x64

class OperatingSystem(Enum):
    WINDOWS_32 = 1
    LINUX = 2
    DARWIN = 3
    UNDEFINED = 4

password = None
currentOperatingSystem = OperatingSystem.UNDEFINED



def createLoadedProjectsConfiguration(path : str):
    if verifyFileExists(path):
       return 

    file = open(str(os.path.join(path,"LoadedProjectsPath.yml")), 'w')
    file.write("ProjectsLoaded: \n []")
    file.close()

def createEngineConfigurationFile(path : str):
    file = open(str(os.path.join(path, "EngineConfiguration.yml")), "w")
    file.write("YeagerLauncherWindowWidth: 1920 \nYeagerLauncherWindowHeight: 1080 \nYeagerEditorWindowWidth: 1920 \nYeagerEditorWindowHeight: 1080")
    file.close()

def copyFilesFromDir(src : str, dst : str, anounce : bool = False):
    files = os.listdir(src)
    for file in files:
        shutil.copy(os.path.join(src, file), dst)
        if anounce: print("[INFO] Copied {} from {} to {}".format(file, src, dst))

def verifyFileExists(path) -> bool:
    return os.path.isfile(str(path))

def verifyDirExists(dir) -> bool:
    return os.path.isdir(str(dir))

def verifyItemExists(path) -> tuple[bool, str]:
    if verifyDirExists(path): return True, "Dir"
    if verifyFileExists(path): return True, "File"
    return False, None

def removeFile(path) -> bool:
    if verifyFileExists(path) and linux_x64.isRoot():
        os.remove(path)
        return True
    elif verifyFileExists(path):
        linux_x64.sudoCommand("rm -r {}".format(path))
        return True
    return False

def removeDir(dir) -> bool:
    if verifyDirExists(dir) and linux_x64.isRoot():
        shutil.rmtree(dir, ignore_errors=True)
        return True
    elif verifyDirExists(dir):
        if len(os.listdir(dir)) > 0:
            linux_x64.sudoCommand("rm -r {}".format(dir))
        else:
            linux_x64.sudoCommand("rmdir {}".format(dir))
        return True
    return False

# retrieves a path in the unix format of /folder/file and converts to the windows version \folder\file
def pathCompatible(path) -> str:
    if currentOperatingSystem == OperatingSystem.WINDOWS_32:
        path = str(path).replace("/", "\\")
        return path
    return path

def checkPlatform():
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

def safeFromArgv(n) -> str:
    if not n > len(sys.argv) - 1:
        return sys.argv[n]
    print(
        "Trying to receive out of index from sys.argv! lenght of argv = "
        + str(len(sys.argv))
        + " index = "
        + str(n - 1)
    )
    return None

def checkIfUserExists(user):
    try:
        if os.name == "posix":
            pwd.getpwnam(user)
    except KeyError:
        print("User " + user + " does not exits")
        sys.exit(0)

def checkIfCmakeInstalled() -> bool:
    return (
        subprocess.call(
            ["cmake", "--version"],
            shell=False,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.STDOUT,
        )
        == 0
    )

def commandSystem(command: str) -> int:
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
    if verifyDirExists(dir):
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
