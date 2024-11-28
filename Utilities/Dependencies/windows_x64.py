import ctypes, sys, enum, os, shutil, itertools, string
import Utilities.Dependencies.common_system as common_system

if os.name == "nt":
    import pyuac

gProgramFilesPath = str()
gProgramFilesFolder = common_system.private_folders
gAppdataFolders = common_system.public_folders


def GetAppdataPath() -> str | None:  # None? You might created a big problem
    return os.getenv("APPDATA")


def CreateAppdataFolders(hierarcy: common_system.FolderClass, path: str):
    path += str("\\" + hierarcy.name)
    if not os.path.exists(path):
        os.mkdir(path)
        print("[INFO] Created folder -> {}".format(path))
    for folder in hierarcy.folders:
        CreateAppdataFolders(folder, path)


def CreateProgramFilesFolders(hierarcy: common_system.FolderClass, path: str):
    path += str("\\" + hierarcy.name)
    if not os.path.exists(path):
        os.mkdir(path)
        print("[INFO] Created folder -> {}".format(path))
    for folder in hierarcy.folders:
        CreateProgramFilesFolders(folder, path)


def CopyAssetsToAppdataFolders():
    src_dir = "Engine/Resources/Shaders"
    dst_dir = str(GetAppdataPath() + "\\YeagerEngine\\Resources\\Shaders")
    common_system.copyFilesFromDir(src_dir, dst_dir, True)


def CopyFontsToProgramFiles():
    src_dir = "Engine/Resources/Fonts"
    dst_dir = gProgramFilesPath + "\\YeagerEngine\\Resources\\Fonts\\Editor"
    common_system.copyFilesFromDir(src_dir, dst_dir, True)


def CopyShadersToProgramFiles():
    src_dir = "Engine/Resources/Shaders"
    dst_dir = gProgramFilesPath + "\\YeagerEngine\\Resources\\Shaders"
    common_system.copyFilesFromDir(src_dir, dst_dir, True)


def CopyTexturesToProgramFiles():
    src_dir = "Engine/Resources/Textures"
    dst_dir = gProgramFilesPath + "\\YeagerEngine\\Resources\\Textures\\Default"
    common_system.copyFilesFromDir(src_dir, dst_dir, True)


def CopySoundsToProgramFiles():
    src_dir = "Engine/Resources/Sound"
    dst_dir = gProgramFilesPath + "\\YeagerEngine\\Resources\\Sound"
    common_system.copyFilesFromDir(src_dir, dst_dir, True)


def CopyIconsToProgramFiles():
    src_dir = "Engine/Resources/Icons"
    dst_dir = gProgramFilesPath + "\\YeagerEngine\\Resources\\Icons"
    common_system.copyFilesFromDir(src_dir, dst_dir, True)


def CopyAssetsToProgramFiles():
    CopyShadersToProgramFiles()
    CopyFontsToProgramFiles()
    CopySoundsToProgramFiles()
    CopyTexturesToProgramFiles()
    CopyIconsToProgramFiles()


def CopyColorschemeToProgramFiles():
    src_dir = "Engine/Configuration/Private/Theme/Colorscheme"
    dst_dir = gProgramFilesPath + "\\YeagerEngine\\Configuration\\Theme\\Colorscheme"
    common_system.copyFilesFromDir(src_dir, dst_dir, True)


def CopySoundConfToProgramFiles():
    src_dir = "Engine/Configuration/Private/Theme/Sound"
    dst_dir = gProgramFilesPath + "\\YeagerEngine\\Configuration\\Theme\\Sound"
    common_system.copyFilesFromDir(src_dir, dst_dir, True)


def CopyShaderConfToProgramFiles():
    src_dir = "Engine/Configuration/Private/Shader"
    dst_dir = gProgramFilesPath + "\\YeagerEngine\\Configuration\\Shader"
    common_system.copyFilesFromDir(src_dir, dst_dir, True)


def CopyConfigurationToProgramFiles():
    CopyColorschemeToProgramFiles()
    CopySoundConfToProgramFiles()
    CopyShaderConfToProgramFiles()


if os.name == "nt":

    def CheckForAdministratorPrivileges():
        if not pyuac.isUserAdmin():
            print("The program returned {}".format(pyuac.runAsAdmin()))
            os._exit(0)


def GetAvailableDrives():
    drive_bitmask = ctypes.cdll.kernel32.GetLogicalDrives()
    return list(
        itertools.compress(
            string.ascii_uppercase,
            map(lambda x: ord(x) - ord("0"), bin(drive_bitmask)[:1:-1]),
        )
    )


def CopyInterfaceConfToAppdata():
    src_dir = "Engine/Configuration/Public/Interface/imgui.ini"
    dst_dir = str(GetAppdataPath() + "\\YeagerEngine\\Configuration\\Interface")
    if common_system.verifyFileExists(src_dir):
        shutil.copy(src_dir, dst_dir)
        print("[INFO] Copied {} to {}".format(src_dir, dst_dir))


def CopyConfigurationToAppdata():
    CopyInterfaceConfToAppdata()


def CreateEmptySceneTemplate():
    skybox_src = "Templates/TP_EmptyScene/Assets/Skybox"
    skybox_dst = (
        gProgramFilesPath + "\\YeagerEngine\\Templates\\TP_EmptyScene\\Assets\\Skybox"
    )
    common_system.copyFilesFromDir(skybox_src, skybox_dst, True)
    conf_path = "Templates/TP_EmptyScene/Template.yml"
    conf_dst = gProgramFilesPath + "\\YeagerEngine\\Templates\\TP_EmptyScene"
    if common_system.verifyFileExists(conf_path):
        shutil.copy(conf_path, conf_dst)
        print("[INFO] Copied {} to {}".format(conf_path, conf_dst))
    asset_conf = "Templates/TP_EmptyScene/Assets/Assets.yml"
    assets_dst = gProgramFilesPath + "\\YeagerEngine\\Templates\\TP_EmptyScene\\Assets"
    if common_system.verifyFileExists(asset_conf):
        shutil.copy(asset_conf, assets_dst)
        print("[INFO] Copied {} to {}".format(asset_conf, assets_dst))


def CreateDefaultTemplates():
    CreateEmptySceneTemplate()


def CheckForProgramFiles():
    drives = GetAvailableDrives()
    available_paths = []
    for d in drives:
        if common_system.verifyDirExists("{}:\\Program Files".format(d)):
            available_paths.append("{}:\\Program Files".format(d))
            print("[INFO] Find {}:\\Program Files!".format(d))
    if len(available_paths) == 0:
        print("Cannot find any drive with the Program Files folder!")
        return
    global gProgramFilesPath
    gProgramFilesPath = available_paths[0]
    return


def CheckIfInstalled():
    if common_system.verifyDirExists(
        gProgramFilesPath + "\\YeagerEngine"
    ) and common_system.verifyDirExists(GetAppdataPath() + "\\YeagerEngine"):
        print("[INFO]: Engine is already installed!")
        input("[Press any key to exit]")
        os._exit(0)


def CreateLoadedProjectsConfiguration():
    src_dir = "Engine/Configuration/Public/Projects/LoadedProjectsPath.yml"
    dst_dir = str(GetAppdataPath() + "YeagerEngine\\Configuration\\Projects")
    if common_system.verifyFileExists(src_dir) and not common_system.verifyFileExists(
        str(os.path.join(dst_dir, "LoadedProjectsPath.yml"))
    ):
        shutil.copy(src_dir, dst_dir)
        print("[INFO] Copied {} to {}".format(src_dir, dst_dir))


def CreateEngineConfigurationFile():
    src_dir = "Engine/Configuration/Public/Variables/EngineConfiguration.yml"
    dst_dir = str(GetAppdataPath() + "YeagerEngine\\Configuration\\Variables")
    if common_system.verifyFileExists(src_dir) and not common_system.verifyFileExists(
        str(os.path.join(dst_dir, "EngineConfiguration.yml"))
    ):
        shutil.copy(src_dir, dst_dir)
        print("[INFO] Copied {} to {}".format(src_dir, dst_dir))


def InstallEngineWindows():
    if os.name == "nt":
        checkForAdministratorPrivileges()
    CheckForProgramFiles()
    CreateAppdataFolders(gAppdataFolders, GetAppdataPath())
    CreateProgramFilesFolders(gProgramFilesFolder, gProgramFilesPath)
    CopyAssetsToProgramFiles()
    CopyConfigurationToProgramFiles()
    CreateLoadedProjectsConfiguration()
    CreateEngineConfigurationFile()
    CopyConfigurationToAppdata()
    CreateDefaultTemplates()
    input("[Press any key to exit]")
