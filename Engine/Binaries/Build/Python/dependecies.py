import os, subprocess, shutil

# TODO later 

def ChangeDir(dir):
    if os.path.exists(dir):
        os.chdir(dir)


def InvolkeGitCloneLink(link: str, dir: str = None) -> bool:
    current = os.getcwd()
    ChangeDir(dir)
    print("[INFO] Cloning {} on {}".format(link, dir))
    process = subprocess.call(["git", "clone", link]) == 0
    ChangeDir(current)
    return process


def NotMainCall():
    if __name__ == "__main__":
        print("dependecies.py cannot be called directly!")
        exit(0)


def CheckForEngineLibDir() -> bool:
    return os.path.exists(os.path.join(os.getcwd(), "Engine", "Lib"))


def CheckForEngineDir() -> bool:
    return os.path.exists(os.path.join(os.getcwd(), "Engine"))


def CreateLibDirIfNotExists() -> tuple[bool, str]:
    lib = os.path.join(os.getcwd(), "Engine", "Lib")
    if CheckForEngineDir() and not CheckForEngineLibDir():
        os.mkdir(lib)
        print("Created folder: ", lib)
        return False, lib
    elif CheckForEngineDir() and CheckForEngineLibDir():
        return True, lib
    else:
        print("Cannot find Engine folder!")
        exit(0)
    return False, None


def GetAllLibraries(lfolder: str):
    tfolder = os.path.join(lfolder, "temp")
    if os.path.exists(tfolder):
        shutil.rmtree(tfolder, ignore_errors=)
    os.mkdir(tfolder)

    if not InvolkeGitCloneLink(
        "https://github.com/NVIDIA-Omniverse/PhysX.git", tfolder
    ):
        print("Cannot git clone the https://github.com/NVIDIA-Omniverse/PhysX.git!")


def InstallDependecies():
    NotMainCall()
    existed, path = CreateLibDirIfNotExists()
    GetAllLibraries(path)
