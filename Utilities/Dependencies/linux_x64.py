import subprocess, getpass, os, sys, pathlib, shutil
import Utilities.Dependencies.common_system as common_system

gLinuxPassword = None
gUserHomeDir = None
gPrivateFoldersPath = "/usr/share"


def RemoveFile(path) -> bool:
    if common_system.VerifyFileExists(path) and CheckIfIsRoot():
        os.remove(path)
        return True
    elif common_system.VerifyFileExists(path):
        ExecuteSudoCmd("rm -r {}".format(path))
        return True
    return False


def RemoveDir(dir) -> bool:
    if common_system.VerifyDirExists(dir) and CheckIfIsRoot():
        shutil.rmtree(dir, ignore_errors=True)
        return True
    elif common_system.VerifyDirExists(dir):
        if len(os.listdir(dir)) > 0:
            ExecuteSudoCmd("rm -r {}".format(dir))
        else:
            ExecuteSudoCmd("rmdir {}".format(dir))
        return True
    return False


def ExecuteCmd(command: str) -> tuple[int, str]:
    return common_system.CmdSystem(command), command


def ExecuteSudoCmd(command: str) -> tuple[int, str]:
    global gLinuxPassword
    cmd = "echo {}|sudo -S {}".format(gLinuxPassword, command)
    if gLinuxPassword is not None:
        return common_system.CmdSystem(cmd), cmd
    print("[ERROR] Password not defined")
    return -1, cmd


def GetSudoPassword() -> str:
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


def CheckIfIsRoot() -> bool:
    return os.geteuid() == 0


def CheckForSudo():
    if not CheckIfIsRoot():
        global gLinuxPassword
        gLinuxPassword = GetSudoPassword()


def GetHomeDir():
    global gUserHomeDir
    gUserHomeDir = str(pathlib.Path.home())


def CheckForFolders():
    global gUserHomeDir
    folders = [
        "/usr/bin",
        "/usr/share",
        gUserHomeDir + "/.local/share",
        gUserHomeDir + "/.local/share/icons/hicolor/256x256/apps",
        gUserHomeDir + "/.local/share/applications",
    ]
    for folder in folders:
        if not common_system.VerifyDirExists(folder):
            print("Cannot find {} in the system!".format(folder))
            sys.exit(0)


def CheckIfCompiled():
    if not common_system.CheckIfCmakeInstalled():
        print("Cmake cannot be found in the system!")
        sys.exit(0)
    common_system.CompileEngine()


def CreatePrivateFolders(hierarcy: common_system.FolderClass, path: str):
    path += str("/" + hierarcy.name)
    if not os.path.exists(path):
        code, cmd = ExecuteSudoCmd("mkdir {}".format(path))
        if code != 0:
            print("Cannot execute the command {}!".format(cmd))
        else:
            print("Created folder -> {}".format(path))
    for folder in hierarcy.folders:
        CreatePrivateFolders(folder, path)


def CreatePublicFolders(hierarcy: common_system.FolderClass, path: str):
    path += str("/" + hierarcy.name)
    if not os.path.exists(path):
        code, cmd = ExecuteCmd("mkdir {}".format(path))
        if code != 0:
            print("Cannot execute the command {}!".format(cmd))
        else:
            print("Created folder -> {}".format(path))
    for folder in hierarcy.folders:
        CreatePublicFolders(folder, path)


def SudoCopyFilesFromDir(src: str, dst: str, anounce: bool = False):
    files = os.listdir(src)
    for file in files:
        code, _ = ExecuteSudoCmd("cp {} {}".format(os.path.join(src, file), dst))
        if code != 0:
            print("[ERROR] Cannot copy {} from {} to {}".format(file, src, dst))
        if anounce and code == 0:
            print("[INFO] Copied {} from {} to {}".format(file, src, dst))


def SudoCopyFile(src: str, dst: str, anounce: bool = False) -> tuple[int, str]:
    code, cmd = ExecuteSudoCmd("cp {} {}".format(src, dst))
    if code != 0:
        print("[ERROR] Cannot copy {} to {}".format(src, dst))
    else:
        print("[INFO] Copied {} -> {}".format(src, dst))
    return code, cmd


def CopyFontsToProgramFiles():
    src_dir = "Engine/Resources/Fonts"
    dst_dir = gPrivateFoldersPath + "/YeagerEngine/Resources/Fonts/Editor"
    SudoCopyFilesFromDir(src_dir, dst_dir, True)


def CopyShadersToProgramFiles():
    src_dir = "Engine/Resources/Shaders"
    dst_dir = gPrivateFoldersPath + "/YeagerEngine/Resources/Shaders"
    SudoCopyFilesFromDir(src_dir, dst_dir, True)


def CopyTexturesToProgramFiles():
    src_dir = "Engine/Resources/Textures"
    dst_dir = gPrivateFoldersPath + "/YeagerEngine/Resources/Textures/Default"
    SudoCopyFilesFromDir(src_dir, dst_dir, True)


def CopySoundsToProgramFiles():
    src_dir = "Engine/Resources/Sound"
    dst_dir = gPrivateFoldersPath + "/YeagerEngine/Resources/Sound"
    SudoCopyFilesFromDir(src_dir, dst_dir, True)


def CopyIconsToProgramFiles():
    src_dir = "Engine/Resources/Icons"
    dst_dir = gPrivateFoldersPath + "/YeagerEngine/Resources/Icons"
    SudoCopyFilesFromDir(src_dir, dst_dir, True)


def CopyAssetsToPrivateFolders():
    CopyShadersToProgramFiles()
    CopyFontsToProgramFiles()
    CopySoundsToProgramFiles()
    CopyTexturesToProgramFiles()
    CopyIconsToProgramFiles()


def CopyColorschemeToProgramFiles():
    src_dir = "Engine/Configuration/Private/Theme/Colorscheme"
    dst_dir = gPrivateFoldersPath + "/YeagerEngine/Configuration/Theme/Colorscheme"
    SudoCopyFilesFromDir(src_dir, dst_dir, True)


def CopySoundConfToProgramFiles():
    src_dir = "Engine/Configuration/Private/Theme/Sound"
    dst_dir = gPrivateFoldersPath + "/YeagerEngine/Configuration/Theme/Sound"
    SudoCopyFilesFromDir(src_dir, dst_dir, True)


def CopyShaderConfToProgramFiles():
    src_dir = "Engine/Configuration/Private/Shader"
    dst_dir = gPrivateFoldersPath + "/YeagerEngine/Configuration/Shader"
    SudoCopyFilesFromDir(src_dir, dst_dir, True)


def CopyLocalesToProgramFiles():
    src_dir = "Engine/Resources/Locales"
    dst_dir = gPrivateFoldersPath + "/YeagerEngine/Resources/Locales"
    locales_dir = os.listdir(src_dir)
    for dir in locales_dir:
        ExecuteSudoCmd("mkdir {}".format(os.path.join(dst_dir, dir)))
        SudoCopyFilesFromDir(
            os.path.join(src_dir, dir), os.path.join(dst_dir, dir), True
        )


def CopyConfigurationToPrivateFolders():
    CopyColorschemeToProgramFiles()
    CopySoundConfToProgramFiles()
    CopyShaderConfToProgramFiles()
    CopyLocalesToProgramFiles()


def CreateLoadedProjectsConfiguration():
    src_dir = "Engine/Configuration/Public/Projects/LoadedProjectsPath.yml"
    dst_dir = str(gUserHomeDir + "/.local/share/YeagerEngine/Configuration/Projects")
    if common_system.VerifyFileExists(src_dir) and not common_system.VerifyFileExists(
        str(os.path.join(dst_dir, "LoadedProjectsPath.yml"))
    ):
        shutil.copy(src_dir, dst_dir)
        print("[INFO] Copied {} to {}".format(src_dir, dst_dir))


def CreateEngineConfiguration():
    src_dir = "Engine/Configuration/Public/Variables/EngineConfiguration.yml"
    dst_dir = str(gUserHomeDir + "/.local/share/YeagerEngine/Configuration/Variables")
    if common_system.VerifyFileExists(src_dir) and not common_system.VerifyFileExists(
        str(os.path.join(dst_dir, "EngineConfiguration.yml"))
    ):
        shutil.copy(src_dir, dst_dir)
        print("[INFO] Copied {} to {}".format(src_dir, dst_dir))


def CopyInterfaceConfigurationToPublicFolder():
    src_dir = "Engine/Configuration/Public/Interface/imgui.ini"
    dst_dir = str(gUserHomeDir + "/.local/share/YeagerEngine/Configuration/Interface")
    if common_system.VerifyFileExists(src_dir) and not common_system.VerifyFileExists(
        str(os.path.join(dst_dir, "imgui.ini"))
    ):
        shutil.copy(src_dir, dst_dir)
        print("[INFO] Copied {} to {}".format(src_dir, dst_dir))


def CopyConfigurationToPublicFolder():
    CopyInterfaceConfigurationToPublicFolder()


def CreateEmptySceneTemplate():
    skybox_src = "Templates/TP_EmptyScene/Assets/Skybox"
    skybox_dst = (
        gPrivateFoldersPath + "/YeagerEngine/Templates/TP_EmptyScene/Assets/Skybox"
    )
    SudoCopyFilesFromDir(skybox_src, skybox_dst, True)
    conf_path = "Templates/TP_EmptyScene/Template.yml"
    conf_dst = gPrivateFoldersPath + "/YeagerEngine/Templates/TP_EmptyScene"
    if common_system.VerifyFileExists(conf_path):
        SudoCopyFile(conf_path, conf_dst, True)
        print("[INFO] Copied {} to {}".format(conf_path, conf_dst))
    asset_conf = "Templates/TP_EmptyScene/Assets/Assets.yml"
    assets_dst = gPrivateFoldersPath + "/YeagerEngine/Templates/TP_EmptyScene/Assets"
    if common_system.VerifyFileExists(asset_conf):
        SudoCopyFile(asset_conf, assets_dst, True)
        print("[INFO] Copied {} to {}".format(asset_conf, assets_dst))


def CreateDefaultTemplates():
    CreateEmptySceneTemplate()


def SudoMoveBinaries():
    if not common_system.VerifyFileExists("YeagerEngine"):
        print("Cannot find the binarie in the root directory of the source code!")
        return
    code, cmd = ExecuteSudoCmd(
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


def CreateSyslinkBinarie():
    src = gPrivateFoldersPath + "/YeagerEngine/Binarie/YeagerEngine"
    dst = "/usr/bin/YeagerEngine"
    if common_system.VerifyFileExists(dst):
        code, cmd = ExecuteSudoCmd("rm {}".format(dst))
        if code != 0:
            print("[ERROR] Cannot execute {}".format(cmd))
        else:
            print("[INFO] Removed {}".format(dst))
    code, cmd = ExecuteSudoCmd("ln -s {} {}".format(src, dst))
    if code != 0:
        print("[ERROR] Cannot execute {}".format(cmd))
    else:
        print("[INFO] Created syslink for the binarie, {} to {}".format(src, dst))


def InstallEngineLinux():
    CheckForSudo()
    GetHomeDir()
    CheckForFolders()
    CheckIfCompiled()
    CreatePublicFolders(common_system.gPublicFolders, gUserHomeDir + "/.local/share")
    CreatePrivateFolders(common_system.gPrivateFolders, gPrivateFoldersPath)
    CopyAssetsToPrivateFolders()
    CopyConfigurationToPrivateFolders()
    CreateLoadedProjectsConfiguration()
    CreateEngineConfiguration()
    CopyConfigurationToPublicFolder()
    CreateEmptySceneTemplate()
    SudoMoveBinaries()
    CreateSyslinkBinarie()
    input("[Installation completed! Press any key to exit]")
