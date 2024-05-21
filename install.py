import shutil, os, sys, subprocess , pwd
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

class FolderType(Enum):
    BINARIE = 1 
    SHARE_DATA = 2
    CACHE_DATA = 3 

# follows the enumation of OperatingSystem, so it can be called like binaries_paths_os[OperatingSystem.value - 1]
binaries_paths_os = ['undefined', '/usr/bin', 'undefined', 'undefine']
share_data = ['undefined ']
currentOperatingSystem = OperatingSystem.UNDEFINED
scriptMode = ModeRequested.UNDEFINED
# the script was involked with the --rootless option on, and a user name was given
rootless = False
unix_user = None
custom_bin_locale = None
shared_data_locale = None

class Args(Enum):
    INSTALL = 0
    UNINSTALL = 1
    REINSTALL = 2
    CLEAN_CACHE = 3
    BINARIE_LOCALE = 4 
    ENGINE_CALLS = 5 
    SHARED_DATA_LOCALE = 6
    ROOTLESS = 7

valid_arguments = ['install', 'uninstall', 'reinstall' '-clean_cache', '-binarie_locale', '-engine_calls', '-shared_data_locale', '-rootless']

def _verify_if_exits(path) -> bool:
    return os.path.isfile(str(path))

def _verify_if_dir_exists(dir) -> bool:
    return os.path.isdir(str(dir))

def _remove_file(path) -> bool:
    if _verify_if_exits(path):
        os.remove(path)
        return True 
    return False

def _print_help():
    print("Usage command: sudo python3 install.py [mode] [args] \n Modes:")
    print("install -- Installs the Engine to the system", end = "\n")
    print("reinstall -- Reinstals and cleans the system from the Engine", end = "\n")
    print("uninstall -- Uninstall the Engine from the system, and cleans", end = "\n")
    print("Args: ", end = "\n")
    print("-cc or -clean_cache -- Cleans the engine cache and configuration from the shared data folder", end = "\n")
    print("-lb or -binarie_locale -- Change the location of instalation of the engine binarie", end = "\n")

def _check_platform():
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
    if not n  > len(sys.argv) - 1:
        return sys.argv[n]
    print('Trying to receive out of index from sys.argv! lenght of argv = ' + str(len(sys.argv)) + ' index = ' +  str(n - 1))    
    return None

def _iterate_args(arg, n) -> int:
    if arg in valid_arguments:
        if arg == valid_arguments[Args.INSTALL.value]:
            scriptMode = ModeRequested.INSTALL
        elif arg == valid_arguments[Args.REINSTALL.value]:
            scriptMode = ModeRequested.REINSTALL
        elif arg == valid_arguments[Args.UNINSTALL.value]:
            scriptMode = ModeRequested.UNINSTALL
        elif arg == valid_arguments[Args.CLEAN_CACHE.value]:
            print('Clean cache')
        elif arg == valid_arguments[Args.BINARIE_LOCALE.value]:
            locale = str(sys.argv[n + 1])
            if not _verify_if_dir_exists(locale):
                print('[ERROR] Given binarie location does not exits! ' + str(locale))
                sys.exit(0)
            custom_bin_locale = locale
            return 2 
        elif arg == valid_arguments[Args.ENGINE_CALLS.value]:
            calls = _safe_from_argv(n + 1)
            return 2 
        elif arg == valid_arguments[Args.SHARED_DATA_LOCALE.value]:
            locale = _safe_from_argv(n + 1)
            if not _verify_if_dir_exists(locale):
                print('[ERROR] Given shared data location does not exits! ' + str(locale))
                sys.exit(0)
            shared_data_locale = locale
            return 2 
        elif arg == valid_arguments[Args.ROOTLESS.value]:
            user = _safe_from_argv(n + 1)
            _check_if_user_exists(user)
            unix_user = user
            rootless = True
            return 2 
    return 1 

def _process_args():
    if len(sys.argv) > 1:
        if str(_safe_from_argv(1)) == 'help':
            _print_help()
            sys.exit(0)

    it = 2
    while it < len(sys.argv): 
        it += _iterate_args(str(sys.argv[it]), it)

def _check_if_user_exists(user):
    try:
        pwd.getpwnam(user)
    except KeyError:
        print('User ' + user + ' does not exits')
        sys.exit(0)

def _local_install_user(user):
        _check_if_user_exists(user)
        shared = '/home/' + user + '/.local/share/applications'
        shutil.copy('Utils/YeagerEngine.desktop', shared)
        print("[INFO] Moved Utils/YeagerEngine.desktop -> " + shared)
    

def _get_binarie_folder_path() -> str:
    return binaries_paths_os[currentOperatingSystem.value - 1]

def _check_if_cmake_installed() -> bool: 
    return subprocess.call(["cmake", "--version"], shell=False, stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT) == 0
        
def _check_if_engine_compiled():
    if not _check_if_cmake_installed():
        print("[ERROR] Cmake isnt installed! Please install it from https://cmake.org/")
        sys.exit(0)
    else:
        print("[INFO] Cmake found!")

    if not os.path.isfile('YeagerEngine'):
        print("[INFO] Yeager Engine is not compiled! Compiling...")
        home = os.getcwd()
        cmd = str("cmake --build " + home + "/build --config Debug --target all --")
        os.system(cmd)

def _get_folder_type(type=FolderType.BINARIE) -> str:
    if type == FolderType.BINARIE:
        return _get_binarie_folder_path()
    elif type == FolderType.CACHE_DATA:
        return 

def _mv_binaries_global():
    dir_global = _get_folder_type(FolderType.BINARIE)
    moved = dir_global
    if custom_bin_locale is None: 
        shutil.copy('YeagerEngine', dir_global)
    else:
        shutil.copy('YeagerEngine', custom_bin_locale)
        moved = custom_bin_locale
    print("[INFO] Moved YeagerEngine -> " + moved)

def _install_engine():
    _mv_binaries_global()
    if rootless and unix_user is not None:
        _local_install_user(unix_user)
    else:
        home = str(pathlib.Path.home())
        shutil.copy('Utils/YeagerEngine.desktop', str(home + "/.local/share/applications"))
        print("[INFO] Moved Utils/YeagerEngine.desktop -> " + str(home + "/.local/share/applications"))
    
    assets = 'Apps/Yeager/Assets/'
    configuration = 'Apps/Yeager/Configuration/'

    try:
        shutil.copytree(src=assets, dst='/usr/share/Yeager/Assets', dirs_exist_ok=True)
        shutil.copytree(src=configuration, dst='/usr/share/Yeager/Configuration', dirs_exist_ok=True)
    except FileExistsError:
        print('[ERROR] Files exists, this error should not be raised!')

def reinstall():
    print()
def uninstall():
    print()

def _execute_instructions():
    if scriptMode == ModeRequested.INSTALL:
        print("Installing Yeager Engine on " + platform.platform())
        _install_engine()
    elif scriptMode == ModeRequested.REINSTALL:
        reinstall()
        print("Reinstalling Yeager Engine")
    elif scriptMode == ModeRequested.UNINSTALL:
        uninstall()
        print('Uninstalling Yeager Engine')
    else:
        print('Non gived mode requested!')
        sys.exit(0)

_check_platform()
_process_args()
_check_if_engine_compiled()
_execute_instructions()






