import ctypes, sys, enum, os, shutil, pyuac, itertools, string
import common_system

class FolderClass:
    def __init__(self, name: str, folders: (list[any] | None) = [])  -> None:
        self.name = name
        self.folders = folders

gProgramFilesPath = str()

programfiles_folders = FolderClass( "YeagerEngine", [
    FolderClass("Binarie", []),
    FolderClass("Utilities", [
        FolderClass("Setup", [])
    ]),
    FolderClass("Configuration", [
        FolderClass("Theme", [
            FolderClass("Colorscheme", []),
            FolderClass("Sound", [])
        ]),
        FolderClass("Shader", [])
    ]),
    FolderClass("Resources", [
        FolderClass("Shaders", []),
        FolderClass("Icons", []),
        FolderClass("Textures", [
            FolderClass("Default")
        ]),
        FolderClass("Fonts", [
            FolderClass("Editor", [])
        ]),
        FolderClass("Locales", []),
        FolderClass("Sound", [])
    ]),
    FolderClass("Templates", [
            FolderClass("EmptyScene", [
                FolderClass("Assets", [
                    FolderClass("Skybox", [])
                ]),
                FolderClass("Configuration", [])
            ])
    ])
])

appdata_folders = FolderClass( "YeagerEngine", [
    FolderClass("Resources", [
        FolderClass("Sound", []),
        FolderClass("Shaders", []),
        FolderClass("Locales", [])
    ]
    ),
    FolderClass("Configuration", [
        FolderClass("Projects", []),
        FolderClass("Variables", []),
        FolderClass("Themes", [
            FolderClass("Colorscheme", []),
            FolderClass("Sound", [])
        ]),
        FolderClass("Interface", []),
        FolderClass("Shaders", [])
    ]
    ),
    FolderClass("Extensions", [
        FolderClass("ModuleAPI", [])
    ]
    ),
    FolderClass("Versions", [
        FolderClass("1.0.0-dev", [])
    ]
    ),
    FolderClass("Cache",[
        FolderClass("Resources", [
            FolderClass("Textures", []),
            FolderClass("Sound", [])
        ]),
    ]
    ),
    FolderClass("Template", [
        FolderClass("Custom", [])
    ]),
    FolderClass("Logs", [
        FolderClass("CrashReport", [])
    ]) 
])

def getAppdataPath() -> (str | None): # None? You might created a big problem
    return os.getenv('APPDATA')

def createAppdataFolders(hierarcy : FolderClass, path: str):
    path += str("\\" + hierarcy.name)
    if not os.path.exists(path):
        os.mkdir(path)
        print("[INFO] Created folder -> {}".format(path))
    for folder in hierarcy.folders:
        createAppdataFolders(folder, path)

def createProgramFilesFolders(hierarcy : FolderClass, path : str):
    path += str("\\" + hierarcy.name)
    if not os.path.exists(path):
        os.mkdir(path)
        print("[INFO] Created folder -> {}".format(path))
    for folder in hierarcy.folders:
        createProgramFilesFolders(folder, path)

def copyAssetsToAppdataFolders():
    src_dir = 'Apps/Yeager/Assets/Shaders'
    dst_dir = str(getAppdataPath() + "\\YeagerEngine\\Resources\\Shaders")
    common_system.copyFilesFromDir(src_dir, dst_dir, True)

def copyFontsToProgramFiles():
    src_dir = "Apps/Yeager/Assets/Fonts"
    dst_dir = gProgramFilesPath +  "\\YeagerEngine\\Resources\\Fonts\\Editor"
    common_system.copyFilesFromDir(src_dir, dst_dir, True)

def copyShadersToProgramFiles():
    src_dir = "Apps/Yeager/Assets/Shaders"
    dst_dir = gProgramFilesPath +  "\\YeagerEngine\\Resources\\Shaders"
    common_system.copyFilesFromDir(src_dir, dst_dir, True)

def copyTexturesToProgramFiles():
    src_dir = "Apps/Yeager/Assets/Textures"
    dst_dir = gProgramFilesPath +  "\\YeagerEngine\\Resources\\Textures\\Default"
    common_system.copyFilesFromDir(src_dir, dst_dir, True)

def copySoundsToProgramFiles():
    src_dir = "Apps/Yeager/Assets/Sounds"
    dst_dir = gProgramFilesPath +  "\\YeagerEngine\\Resources\\Sound"
    common_system.copyFilesFromDir(src_dir, dst_dir, True)

def copyIconsToProgramFiles():
    src_dir = "Apps/Yeager/Assets/Icons"
    dst_dir = gProgramFilesPath +  "\\YeagerEngine\\Resources\\Icons"
    common_system.copyFilesFromDir(src_dir, dst_dir, True)

def copyAssetsToProgramFiles():
    copyShadersToProgramFiles()
    copyFontsToProgramFiles()
    copySoundsToProgramFiles()
    copyTexturesToProgramFiles()
    copyIconsToProgramFiles()

def copyColorschemeToProgramFiles():
    src_dir = "Apps/Yeager/Configuration/Editor/Private/Colorschemes"
    dst_dir = gProgramFilesPath +  "\\YeagerEngine\\Configuration\\Theme\\Colorscheme"
    common_system.copyFilesFromDir(src_dir, dst_dir, True)

def copySoundConfToProgramFiles():
    src_dir = "Apps/Yeager/Configuration/Editor/Private/Sound"
    dst_dir = gProgramFilesPath +  "\\YeagerEngine\\Configuration\\Theme\\Sound"
    common_system.copyFilesFromDir(src_dir, dst_dir, True)

def copyShaderConfToProgramFiles():
    src_dir = "Apps/Yeager/Configuration/Editor/Private/Shaders"
    dst_dir =  gProgramFilesPath + "\\YeagerEngine\\Configuration\\Shader"
    common_system.copyFilesFromDir(src_dir, dst_dir, True)

def copyConfigurationToProgramFiles():
    copyColorschemeToProgramFiles()
    copySoundConfToProgramFiles()
    copyShaderConfToProgramFiles()

def checkForAdministratorPrivileges():
    if not pyuac.isUserAdmin():
        print("The program returned {}".format(pyuac.runAsAdmin()))
        os._exit(0)

def getAvailableDrives():
    drive_bitmask = ctypes.cdll.kernel32.GetLogicalDrives()
    return list(itertools.compress(string.ascii_uppercase, map(lambda x: ord(x) - ord('0'), bin(drive_bitmask)[:1:-1])))

def copyInterfaceConfToAppdata():
    src_dir = "Apps/Yeager/Configuration/Editor/Public/Interface/imgui.ini"
    dst_dir = str(getAppdataPath() + "\\YeagerEngine\\Configuration\\Interface")
    if common_system.verifyFileExists(src_dir):
        shutil.copy(src_dir, dst_dir)
        print("[INFO] Copied {} to {}".format(src_dir, dst_dir))

def copyConfigurationToAppdata():
    copyInterfaceConfToAppdata()

def createEmptySceneTemplate():
    skybox_src = "Templates/EmptyScene/Assets/Skybox"
    skybox_dst = gProgramFilesPath + "\\YeagerEngine\\Templates\\EmptyScene\\Assets\\Skybox"
    common_system.copyFilesFromDir(skybox_src, skybox_dst, True)
    conf_path = "Templates/EmptyScene/Template.yml"
    conf_dst = gProgramFilesPath + "\\YeagerEngine\\Templates\\EmptyScene"
    if common_system.verifyFileExists(conf_path):
        shutil.copy(conf_path, conf_dst)
        print("[INFO] Copied {} to {}".format(conf_path, conf_dst))
    asset_conf = "Templates/EmptyScene/Assets/Assets.yml"
    assets_dst = gProgramFilesPath + "\\YeagerEngine\\Templates\\EmptyScene\\Assets"
    if common_system.verifyFileExists(asset_conf):
        shutil.copy(asset_conf, assets_dst)
        print("[INFO] Copied {} to {}".format(asset_conf, assets_dst))

def createDefaultTemplates():
    createEmptySceneTemplate()

def checkForProgramFiles():
    drives = getAvailableDrives()
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

def checkIfInstalled():
    if common_system.verifyDirExists(gProgramFilesPath + "\\YeagerEngine") and common_system.verifyDirExists(getAppdataPath() + "\\YeagerEngine"):
        print("[INFO]: Engine is already installed!")
        input("[Press any key to exit]")
        os._exit(0)

def installEngineWindows():
    checkForAdministratorPrivileges()
    checkForProgramFiles()
    # checkIfInstalled()
    createAppdataFolders(appdata_folders, getAppdataPath())
    createProgramFilesFolders(programfiles_folders, gProgramFilesPath)
    copyAssetsToProgramFiles()
    copyConfigurationToProgramFiles()
    common_system.createLoadedProjectsConfiguration(getAppdataPath() + "\\YeagerEngine\\Configuration\\Projects")
    common_system.createEngineConfigurationFile(getAppdataPath() + "\\YeagerEngine\\Configuration\\Variables")
    copyConfigurationToAppdata()
    createDefaultTemplates()
    input("[Press any key to exit]")

installEngineWindows()

