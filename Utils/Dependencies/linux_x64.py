import subprocess, getpass, os, sys
import common_system

# dont use the fucking mv command, or the directories and the files will dissapear from the engine source code
def cpSudoCommand(src: str, dst: str, is_dir=False) -> int:
    command = "cp {} {}".format(src, dst)
    if is_dir:
        command = "cp -r {} {}".format(src, dst)
    return sudoCommand(command)


def cpCommand(src: str, dst: str, is_dir=False) -> int:
    command = "cp {} {}".format(src, dst)
    if is_dir:
        command = "cp -r {} {}".format(src, dst)
    return common_system.commandSystem(command)


def publicPermissions(file: str) -> bool:
    return (
        subprocess.call(
            ["sudo", "chmod", "777", file],
            shell=False,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.STDOUT,
        )
        == 0
    )


def sudoCommand(command: str) -> int:
    global password
    cmd = "echo {}|sudo -S {}".format(password, command)
    if password is not None:
        return common_system.commandSystem(cmd)
    print("[ERROR] Password not defined")
    return -1


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