### Windows Installation Archtecture 
Private Files and Folders, need the Administrator privileges to be installed and managed.
These files and folder must not be changed!  
- (DriverLetter):\\Program Files 
    (Folder) YeagerEngine
        (Folder) Binarie 
            (Exe) YeagerEngine.exe
            (Exe) YeagerCrashReport.exe
            (Dll) YeagerEngine.dll
            (Dll) All Dlls included
        (Folder) Utilities
            (Folder) Setup
                (Python) Repair.py
                (Python) Reinstall.py 
        (Folder) Configuration
            (Folder) Theme
                (Folder) Colorscheme
                    (YAML) Colorscheme.yml
                (Folder) Sound
                    (YAML) Sounds.yml 
            (Folder) Shader
                (YAML) Shaders default configuration
        (Folder) Resources
            (Folder) Shaders
                (GLSL) All shaders that the engine uses by default
            (Folder) Icons
                (Image) YeagerEngineIcon.png
                (Image) YeagerCrashReportIcon.png
            (Folder) Textures
                (Folder) Default
                    (Image) All textures included in the engine
            (Folder) Fonts
                (Folder) Editor
                    (TTF) All fonts included in the editor
            (Folder) Locales
                (YAML) Default languages installed 
            (Folder) Sound
                (WAV or MP3) All sounds that the engine uses
                (METADATA) All sounds stored in memory
        (Folder) Templates
            (Folder) Default
                (Folder) EmptyScene 
                    (Folder) Assets
                        (Folder) Skybox
                            (3D Model .OBJ) skybox.obj
                            (PNG or JPG) skybox.png
                            (MTL) skybox.mtl
                    (Folder) Configuration
                        (YAML) DefaultLighting.yml
                        

Public files and folder, that are stored in the user %appdata% folder, those can be changed and modify by the user
- (%appdata%)
    (Folder) YeagerEngine
        (Folder) Configuration
            (Folder) Projects
                (YAML) LoadedProjects.yml 
            (Folder) Variables 
                (YAML) EngineConfiguration.yml
                (YAML) Settings.yml
            (Folder) Themes
                (Folder) Colorscheme
                    (YAML) All custom colorscheme that the user created (Ex: ColorschemeTheme.yml)
                (Folder) Sound
                    (YAML) All custom sounds configuration the user want to use (Ex: CustomSounds.yml)
            (Folder) Shaders
                (Folder) Custom 
                    (YAML) CustomShader.yml
            (Folder) Interface
                (INI) imgui.ini 
        (Folder) Resources
            (Folder) Sound
                (WAV or MP3) All sounds that the user want to use
                (METADATA) All sounds that the user want to use, stored in memory 
            (Folder) Shaders
                (GLSL) Custom shaders goes here
            (Folder) Locales
                (Folder) Language Folder, with the format of "Locale_(language-region)" (Ex: Locale_en-us) 
                    (Folder) Editor
                        (YAML) All the language translanted, with the format of "Editor_(languange-region).yml" (Ex: Editor_en-us.yml) 
                    (Folder) Launcher 
                        (YAML) All the language translanted, with the format of "Launcher_(languange-region).yml" (Ex: Launcher_en-us.yml)
        (Folder) Extensions
            (Folder) All extensions folders
            (Folder) Module API (It is the template used to created extensions for the engine)  
                (Folder) Resources
                    (Folder) Locales
                (Folder) Source
                    (C++ Source) Entry.cpp (The entry point for the engine) 
                    (Folder) Public 
                        (C++ Headers) All the headers of the module (Ex: Plugin.hpp) 
                    (Folder) Private
                        (C++ Source) All the source of the module (Ex: Plugin.cpp)
                (Folder) Lib
                (Folder) Configuration
                    (YAML) ConfigurationEntry.yml (All modules must have a ConfigurationEntry.yml file in this folder, so the engine can find information about the module itself) 
                (Folder) Cache
                (Folder) Version
        (Folder) Versions
            (Folder) 1.0.0-dev (The first version)
        (Folder) Cache
            (Folder) Resources
                (Folder) Textures
                    (METADATA) All textures cache are stored here
                (Folder) Sound
                    (METADATA) All sound cache are stored here
        (Folder) Templates
            (Folder) Custom
                (Folder) All custom templates the user want to create or download


### Linux Installation Archtecture 
Private Files and Folders, need the sudo privileges to be installed and managed.
These files and folder must not be changed!  
- /usr/share 
    (Folder) YeagerEngine
        (Folder) Binarie 
            (Bin) YeagerEngine 
            (Bin) YeagerCrashReport
            (Dll) YeagerEngine.so
            (Dll) All dynamic lib included
        (Folder) Utilities
            (Folder) Setup
                (Python) Repair.py
                (Python) Reinstall.py 
        (Folder) Configuration
            (Folder) Theme
                (Folder) Colorscheme
                    (YAML) Colorscheme.yml
                (Folder) Sound
                    (YAML) Sounds.yml 
            (Folder) Shader
                (YAML) Shaders default configuration
        (Folder) Resources
            (Folder) Shaders
                (GLSL) All shaders that the engine uses by default
            (Folder) Icons
                (Image) YeagerEngineIcon.png
                (Image) YeagerCrashReportIcon.png
            (Folder) Textures
                (Folder) Default
                    (Image) All textures included in the engine
            (Folder) Fonts
                (Folder) Editor
                    (TTF) All fonts included in the editor
            (Folder) Locales
                (YAML) Default languages installed 
            (Folder) Sound
                (WAV or MP3) All sounds that the engine uses
                (METADATA) All sounds stored in memory
        (Folder) Templates
            (Folder) Default
                (Folder) EmptyScene 
                    (Folder) Assets
                        (Folder) Skybox
                            (3D Model .OBJ) skybox.obj
                            (PNG or JPG) skybox.png
                            (MTL) skybox.mtl
                    (Folder) Configuration
                        (YAML) DefaultLighting.yml

In the binarie folder of linux, there will be a symlink to the binarie in the /usr/share/YeagerEngine/Binarie folder                
- /usr/bin
    (Symlink) YeagerEngine -> /usr/share/YeagerEngine/Binarie/YeagerEngine
    (Symlink) YeagerCrashReport -> /usr/share/YeagerEngine/Binarie/YeagerCrashReport

There also the desktop information about the engine, like the icon image and the desktop file for showing up upon search
- $HOME/.local/share/applications
    (DESKTOP) YeagerEngine.desktop

- $HOME/.local/share/icons/hicolor/256x256/apps
    (PNG) YeagerEngine.png

Public files and folder, that are stored in the user $HOME/.local/share folder, those can be changed and modify by the user
- $HOME/.local/share
    (Folder) YeagerEngine
        (Folder) Configuration
            (Folder) Projects
                (YAML) LoadedProjects.yml 
            (Folder) Variables 
                (YAML) EngineConfiguration.yml
                (YAML) Settings.yml
            (Folder) Themes
                (Folder) Colorscheme
                    (YAML) All custom colorscheme that the user created (Ex: ColorschemeTheme.yml)
                (Folder) Sound
                    (YAML) All custom sounds configuration the user want to use (Ex: CustomSounds.yml)
            (Folder) Shaders
                (Folder) Custom 
                    (YAML) CustomShader.yml
            (Folder) Interface
                (INI) imgui.ini 
        (Folder) Resources
            (Folder) Sound
                (WAV or MP3) All sounds that the user want to use
                (METADATA) All sounds that the user want to use, stored in memory 
            (Folder) Shaders
                (GLSL) Custom shaders goes here
            (Folder) Locales
                (Folder) Language Folder, with the format of "Locale_(language-region)" (Ex: Locale_en-us) 
                    (Folder) Editor
                        (YAML) All the language translanted, with the format of "Editor_(languange-region).yml" (Ex: Editor_en-us.yml) 
                    (Folder) Launcher 
                        (YAML) All the language translanted, with the format of "Launcher_(languange-region).yml" (Ex: Launcher_en-us.yml)
        (Folder) Extensions
            (Folder) All extensions folders
            (Folder) Module API (It is the template used to created extensions for the engine)  
                (Folder) Resources
                    (Folder) Locales
                (Folder) Source
                    (C++ Source) Entry.cpp (The entry point for the engine) 
                    (Folder) Public 
                        (C++ Headers) All the headers of the module (Ex: Plugin.hpp) 
                    (Folder) Private
                        (C++ Source) All the source of the module (Ex: Plugin.cpp)
                (Folder) Lib
                (Folder) Configuration
                    (YAML) ConfigurationEntry.yml (All modules must have a ConfigurationEntry.yml file in this folder, so the engine can find information about the module itself) 
                (Folder) Cache
                (Folder) Version
        (Folder) Versions
            (Folder) 1.0.0-dev (The first version)
        (Folder) Cache
            (Folder) Resources
                (Folder) Textures
                    (METADATA) All textures cache are stored here
                (Folder) Sound
                    (METADATA) All sound cache are stored here
        (Folder) Templates
            (Folder) Custom
                (Folder) All custom templates the user want to create or download

