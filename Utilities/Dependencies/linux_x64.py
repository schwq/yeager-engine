import subprocess, getpass, os, sys, pathlib, shutil
import Utilities.Dependencies.common_system as common_system

gLinuxPassword = None
gUserHomeDir = None
gPrivateFoldersPath = "/usr/share"


def removeFile(path) -> bool:
    if common_system.verifyFileExists(path) and isRoot():
        os.remove(path)
        return True
    elif common_system.verifyFileExists(path):
        sudoCommand("rm -r {}".format(path))
        return True
    return False


def removeDir(dir) -> bool:
    if common_system.verifyDirExists(dir) and isRoot():
        shutil.rmtree(dir, ignore_errors=True)
        return True
    elif common_system.verifyDirExists(dir):
        if len(os.listdir(dir)) > 0:
            sudoCommand("rm -r {}".format(dir))
        else:
            sudoCommand("rmdir {}".format(dir))
        return True
    return False


def exeCommand(command: str) -> tuple[int, str]:
    return common_system.commandSystem(command), command


def sudoCommand(command: str) -> tuple[int, str]:
    global gLinuxPassword
    cmd = "echo {}|sudo -S {}".format(gLinuxPassword, command)
    if gLinuxPassword is not None:
        return common_system.commandSystem(cmd), cmd
    print("[ERROR] Password not defined")
    return -1, cmd


def getSudoPassword() -> str:
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


def isRoot() -> bool:
    return os.geteuid() == 0


def checkForSudo():
    if not isRoot():
        global gLinuxPassword
        gLinuxPassword = getSudoPassword()


def getHomeDir():
    global gUserHomeDir
    gUserHomeDir = str(pathlib.Path.home())


def checkForFolders():
    global gUserHomeDir
    folders = [
        "/usr/bin",
        "/usr/share",
        gUserHomeDir + "/.local/share",
        gUserHomeDir + "/.local/share/icons/hicolor/256x256/apps",
        gUserHomeDir + "/.local/share/applications",
    ]
    for folder in folders:
        if not common_system.verifyDirExists(folder):
            print("Cannot find {} in the system!".format(folder))
            sys.exit(0)


def checkIfCompiled():
    if not common_system.checkIfCmakeInstalled():
        print("Cmake cannot be found in the system!")
        sys.exit(0)
    common_system.compileEngine()


def createPrivateFolders(hierarcy: common_system.FolderClass, path: str):
    path += str("/" + hierarcy.name)
    if not os.path.exists(path):
        code, cmd = sudoCommand("mkdir {}".format(path))
        if code != 0:
            print("Cannot execute the command {}!".format(cmd))
        else:
            print("Created folder -> {}".format(path))
    for folder in hierarcy.folders:
        createPrivateFolders(folder, path)


def createPublicFolders(hierarcy: common_system.FolderClass, path: str):
    path += str("/" + hierarcy.name)
    if not os.path.exists(path):
        code, cmd = exeCommand("mkdir {}".format(path))
        if code != 0:
            print("Cannot execute the command {}!".format(cmd))
        else:
            print("Created folder -> {}".format(path))
    for folder in hierarcy.folders:
        createPublicFolders(folder, path)


def sudoCopyFilesFromDir(src: str, dst: str, anounce: bool = False):
    files = os.listdir(src)
    for file in files:
        code, _ = sudoCommand("cp {} {}".format(os.path.join(src, file), dst))
        if code != 0:
            print("[ERROR] Cannot copy {} from {} to {}".format(file, src, dst))
        if anounce and code == 0:
            print("[INFO] Copied {} from {} to {}".format(file, src, dst))


def sudoCopyFile(src: str, dst: str, anounce: bool = False) -> tuple[int, str]:
    code, cmd = sudoCommand("cp {} {}".format(src, dst))
    if code != 0:
        print("[ERROR] Cannot copy {} to {}".format(src, dst))
    else:
        print("[INFO] Copied {} -> {}".format(src, dst))
    return code, cmd


def copyFontsToProgramFiles():
    src_dir = "Engine/Resources/Fonts"
    dst_dir = gPrivateFoldersPath + "/YeagerEngine/Resources/Fonts/Editor"
    sudoCopyFilesFromDir(src_dir, dst_dir, True)


def copyShadersToProgramFiles():
    src_dir = "Engine/Resources/Shaders"
    dst_dir = gPrivateFoldersPath + "/YeagerEngine/Resources/Shaders"
    sudoCopyFilesFromDir(src_dir, dst_dir, True)


def copyTexturesToProgramFiles():
    src_dir = "Engine/Resources/Textures"
    dst_dir = gPrivateFoldersPath + "/YeagerEngine/Resources/Textures/Default"
    sudoCopyFilesFromDir(src_dir, dst_dir, True)


def copySoundsToProgramFiles():
    src_dir = "Engine/Resources/Sound"
    dst_dir = gPrivateFoldersPath + "/YeagerEngine/Resources/Sound"
    sudoCopyFilesFromDir(src_dir, dst_dir, True)


def copyIconsToProgramFiles():
    src_dir = "Engine/Resources/Icons"
    dst_dir = gPrivateFoldersPath + "/YeagerEngine/Resources/Icons"
    sudoCopyFilesFromDir(src_dir, dst_dir, True)


def copyAssetsToPrivateFolders():
    copyShadersToProgramFiles()
    copyFontsToProgramFiles()
    copySoundsToProgramFiles()
    copyTexturesToProgramFiles()
    copyIconsToProgramFiles()


def copyColorschemeToProgramFiles():
    src_dir = "Engine/Configuration/Private/Theme/Colorscheme"
    dst_dir = gPrivateFoldersPath + "/YeagerEngine/Configuration/Theme/Colorscheme"
    sudoCopyFilesFromDir(src_dir, dst_dir, True)


def copySoundConfToProgramFiles():
    src_dir = "Engine/Configuration/Private/Theme/Sound"
    dst_dir = gPrivateFoldersPath + "/YeagerEngine/Configuration/Theme/Sound"
    sudoCopyFilesFromDir(src_dir, dst_dir, True)


def copyShaderConfToProgramFiles():
    src_dir = "Engine/Configuration/Private/Shader"
    dst_dir = gPrivateFoldersPath + "/YeagerEngine/Configuration/Shader"
    sudoCopyFilesFromDir(src_dir, dst_dir, True)


def copyLocalesToProgramFiles():
    src_dir = "Engine/Resources/Locales"
    dst_dir = gPrivateFoldersPath + "/YeagerEngine/Resources/Locales"
    locales_dir = os.listdir(src_dir)
    for dir in locales_dir:
        sudoCommand("mkdir {}".format(os.path.join(dst_dir, dir)))
        sudoCopyFilesFromDir(
            os.path.join(src_dir, dir), os.path.join(dst_dir, dir), True
        )


def copyConfigurationToPrivateFolders():
    copyColorschemeToProgramFiles()
    copySoundConfToProgramFiles()
    copyShaderConfToProgramFiles()
    copyLocalesToProgramFiles()


def createLoadedProjectsConfiguration():
    src_dir = "Engine/Configuration/Public/Projects/LoadedProjectsPath.yml"
    dst_dir = str(gUserHomeDir + "/.local/share/YeagerEngine/Configuration/Projects")
    if common_system.verifyFileExists(src_dir) and not common_system.verifyFileExists(
        str(os.path.join(dst_dir, "LoadedProjectsPath.yml"))
    ):
        shutil.copy(src_dir, dst_dir)
        print("[INFO] Copied {} to {}".format(src_dir, dst_dir))


def createEngineConfiguration():
    src_dir = "Engine/Configuration/Public/Variables/EngineConfiguration.yml"
    dst_dir = str(gUserHomeDir + "/.local/share/YeagerEngine/Configuration/Variables")
    if common_system.verifyFileExists(src_dir) and not common_system.verifyFileExists(
        str(os.path.join(dst_dir, "EngineConfiguration.yml"))
    ):
        shutil.copy(src_dir, dst_dir)
        print("[INFO] Copied {} to {}".format(src_dir, dst_dir))


def copyInterfaceConfigurationToPublicFolder():
    src_dir = "Engine/Configuration/Public/Interface/imgui.ini"
    dst_dir = str(gUserHomeDir + "/.local/share/YeagerEngine/Configuration/Interface")
    if common_system.verifyFileExists(src_dir) and not common_system.verifyFileExists(
        str(os.path.join(dst_dir, "imgui.ini"))
    ):
        shutil.copy(src_dir, dst_dir)
        print("[INFO] Copied {} to {}".format(src_dir, dst_dir))


def copyConfigurationToPublicFolder():
    copyInterfaceConfigurationToPublicFolder()


def createEmptySceneTemplate():
    skybox_src = "Templates/TP_EmptyScene/Assets/Skybox"
    skybox_dst = (
        gPrivateFoldersPath + "/YeagerEngine/Templates/TP_EmptyScene/Assets/Skybox"
    )
    sudoCopyFilesFromDir(skybox_src, skybox_dst, True)
    conf_path = "Templates/TP_EmptyScene/Template.yml"
    conf_dst = gPrivateFoldersPath + "/YeagerEngine/Templates/TP_EmptyScene"
    if common_system.verifyFileExists(conf_path):
        sudoCopyFile(conf_path, conf_dst, True)
        print("[INFO] Copied {} to {}".format(conf_path, conf_dst))
    asset_conf = "Templates/TP_EmptyScene/Assets/Assets.yml"
    assets_dst = gPrivateFoldersPath + "/YeagerEngine/Templates/TP_EmptyScene/Assets"
    if common_system.verifyFileExists(asset_conf):
        sudoCopyFile(asset_conf, assets_dst, True)
        print("[INFO] Copied {} to {}".format(asset_conf, assets_dst))


def createDefaultTemplates():
    createEmptySceneTemplate()


def sudoMoveBinaries():
    if not common_system.verifyFileExists("YeagerEngine"):
        print("Cannot find the binarie in the root directory of the source code!")
        return
    code, cmd = sudoCommand(
        "cp {} {}".format("YeagerEngine", gPrivateFoldersPath + "/YeagerEngine/Binarie")
    )
    if code != 0:
        print("[ERROR] Cannot execute {}".format(cmd))
    else:
        print(
            "[INFO] Copied {} to {}".format(
                "YeagerEngine", gPrivateFoldersPath + "/YeagerEngine/Binarie"
            )
        )


def createSyslinkBinarie():
    src = gPrivateFoldersPath + "/YeagerEngine/Binarie/YeagerEngine"
    dst = "/usr/bin/YeagerEngine"
    if common_system.verifyFileExists(dst):
        code, cmd = sudoCommand("rm {}".format(dst))
        if code != 0:
            print("[ERROR] Cannot execute {}".format(cmd))
        else:
            print("[INFO] Removed {}".format(dst))
    code, cmd = sudoCommand("ln -s {} {}".format(src, dst))
    if code != 0:
        print("[ERROR] Cannot execute {}".format(cmd))
    else:
        print("[INFO] Created syslink for the binarie, {} to {}".format(src, dst))


def installEngineLinux():
    checkForSudo()
    getHomeDir()
    checkForFolders()
    checkIfCompiled()
    createPublicFolders(common_system.public_folders, gUserHomeDir + "/.local/share")
    createPrivateFolders(common_system.private_folders, gPrivateFoldersPath)
    copyAssetsToPrivateFolders()
    copyConfigurationToPrivateFolders()
    createLoadedProjectsConfiguration()
    createEngineConfiguration()
    copyConfigurationToPublicFolder()
    createEmptySceneTemplate()
    sudoMoveBinaries()
    createSyslinkBinarie()
    input("[Installation completed! Press any key to exit]")
