# <p align="center"> Yeager Engine </p>

#### <p align="center"> ```An open source 3D/2D engine written in C++``` </p>

#### <p align="center"> About the project </p>

Yeager engine is an open source engine, under the *GNU General Public License v3.0*. It is written in C/C++ and uses OpenGL 3.3 for rendering. It aims to deliver 3D realistic scenarios with desirable performance, still having support for the development of 2D scenarios. Other features are: complete customization, flexible for modifications, and cross platform, with full support on Windows, Mac and Linux!

```⚠️ Disclaimer: This project is in early development! It has the possibility of crashing, numerous bugs and several missing features ```

#### <p align="center"> Usage </p>
To install the engine, you can clone the project directly and run the python installer:
``` bash
$ git clone https://github.com/schwq/YeagerEngine.git
$ cd YeagerEngine
$ python3 setup.py install 
```
To use the engine, you can call it on the terminal with the desiariable arguments by:
``` bash
$ YeagerEngine -args
```

Available arguments:
```bash 
$ YeagerEngine -ForcedEntry (Project name) 
$ YeagerEngine -DebugTitleString (Title) (Ex. DEBUG_VSCODE)
$ YeagerEngine -DebugVirtualMachine (VM) (Ex. valgrind)
$ YeagerEngine -FromSource
$ YeagerEngine -NetworkSocket (Role) (IP) (Port) (Ex. Client 127.0.0.1 5005)
```
- Forced Entry: A way to skip the launcher and starts running inside the scene
- Debug Title String: Gives a new title to the engine during runtime, useful to debugging
- Debug Virtual Machine: If the engine is running inside a virtual machine (Valgrind for example), you can set the name  of the vm, so the engine works properlly
- From Source: Sets to true if you are running the engine from the source code
- Network Socket: Makes a network connection with the given role, useful to benchmarking with python   

#### <p align="center"> Supporting </p>
You can support the project by pull requests and by point out issues and bugs! Fell free to reach out the developers. 





