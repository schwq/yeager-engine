//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine
//
//    Copyright (C) 2023 - Present
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "../../Common/Common.h"
#include "../../Settings.h"
#include "../Editor/Explorer.h"
#include "../Lighting/LightHandle.h"
#include "../Media/AudioHandle.h"
#include "../Renderer/ImageUtilities.h"
#include "../Renderer/TextureHandle.h"
#include "../Renderer/Window.h"
#include "FolderExplorer.h"
#include "IconsFontAwesome6.h"

namespace Yeager {
class ApplicationCore;
class Launcher;
struct LauncherProjectPicker;

#define YEAGER_LAUNCHER_WINDOW_FLAGS                                                                 \
  ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus | \
      ImGuiWindowFlags_NoTitleBar

#define YEAGER_IMGUI_OPENGL_VERSION "#version 330"

struct TemplateHandle {
  std::filesystem::path FolderPath;
  std::filesystem::path TemplateConfigurationPath;
  std::filesystem::path AssetsConfigurationPath;
  String Name = YEAGER_NULL_LITERAL;
  String Description = YEAGER_NULL_LITERAL;
};

/// @brief Struct of the Imgui Colorscheme, the defaults values are for the dark mode,
/// @brief colorschemes can been readed from the colorscheme configuration folder
struct ColorschemeConfig {
  float WindowRounding = 5.3f;
  float FrameRounding = 2.3f;
  float ScrollbarRounding = 3;
  ImVec4 Colors_ImGuiCol_Text = ImVec4(0.90f, 0.90f, 0.90f, 0.90f);
  ImVec4 Colors_ImGuiCol_TextDisabled = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
  ImVec4 Colors_ImGuiCol_WindowBg = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
  ImVec4 Colors_ImGuiCol_ChildBg = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  ImVec4 Colors_ImGuiCol_PopupBg = ImVec4(0.05f, 0.05f, 0.10f, 0.85f);
  ImVec4 Colors_ImGuiCol_Border = ImVec4(0.70f, 0.70f, 0.70f, 0.65f);
  ImVec4 Colors_ImGuiCol_BorderShadow = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  ImVec4 Colors_ImGuiCol_FrameBg = ImVec4(0.00f, 0.00f, 0.01f, 1.00f);
  ImVec4 Colors_ImGuiCol_FrameBgHovered = ImVec4(0.90f, 0.80f, 0.80f, 0.40f);
  ImVec4 Colors_ImGuiCol_FrameBgActive = ImVec4(0.90f, 0.65f, 0.65f, 0.45f);
  ImVec4 Colors_ImGuiCol_TitleBg = ImVec4(0.00f, 0.00f, 0.00f, 0.83f);
  ImVec4 Colors_ImGuiCol_TitleBgCollapsed = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
  ImVec4 Colors_ImGuiCol_TitleBgActive = ImVec4(0.00f, 0.00f, 0.00f, 0.87f);
  ImVec4 Colors_ImGuiCol_MenuBarBg = ImVec4(0.01f, 0.01f, 0.02f, 0.80f);
  ImVec4 Colors_ImGuiCol_ScrollbarBg = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
  ImVec4 Colors_ImGuiCol_ScrollbarGrab = ImVec4(0.55f, 0.53f, 0.55f, 0.51f);
  ImVec4 Colors_ImGuiCol_ScrollbarGrabHovered = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
  ImVec4 Colors_ImGuiCol_ScrollbarGrabActive = ImVec4(0.56f, 0.56f, 0.56f, 0.91f);
  ImVec4 Colors_ImGuiCol_CheckMark = ImVec4(0.90f, 0.90f, 0.90f, 0.83f);
  ImVec4 Colors_ImGuiCol_SliderGrab = ImVec4(0.70f, 0.70f, 0.70f, 0.62f);
  ImVec4 Colors_ImGuiCol_SliderGrabActive = ImVec4(0.30f, 0.30f, 0.30f, 0.84f);
  ImVec4 Colors_ImGuiCol_Button = ImVec4(0.48f, 0.72f, 0.89f, 0.49f);
  ImVec4 Colors_ImGuiCol_ButtonHovered = ImVec4(0.50f, 0.69f, 0.99f, 0.68f);
  ImVec4 Colors_ImGuiCol_ButtonActive = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
  ImVec4 Colors_ImGuiCol_Header = ImVec4(0.30f, 0.69f, 1.00f, 0.53f);
  ImVec4 Colors_ImGuiCol_HeaderHovered = ImVec4(0.44f, 0.61f, 0.86f, 1.00f);
  ImVec4 Colors_ImGuiCol_HeaderActive = ImVec4(0.38f, 0.62f, 0.83f, 1.00f);
  ImVec4 Colors_ImGuiCol_ResizeGrip = ImVec4(1.00f, 1.00f, 1.00f, 0.85f);
  ImVec4 Colors_ImGuiCol_ResizeGripHovered = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
  ImVec4 Colors_ImGuiCol_ResizeGripActive = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
  ImVec4 Colors_ImGuiCol_PlotLines = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  ImVec4 Colors_ImGuiCol_PlotLinesHovered = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
  ImVec4 Colors_ImGuiCol_PlotHistogram = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
  ImVec4 Colors_ImGuiCol_PlotHistogramHovered = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
  ImVec4 Colors_ImGuiCol_TextSelectedBg = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
};

/// @brief  Struct of the Warning window, its takes a string for the message, x and y sizes,
/// @brief  its also have two booleans, active for holding just one warning at the time, and first log
/// @brief  so it doesnt keep logging the same thing
struct InterfaceWarningWindow {
  String Warning;
  Uint SizeWidth = 0, SizeHeight = 0;
  bool Active = false;
  bool FirstLogWarning = true;
};

/// @brief Struct representing a Imgui Button, used for logging and debugging the pressed buttons
struct InterfaceButton {
  InterfaceButton(String name, String text) : Text(text), Name(name){};
  /// @brief Centered a button in the ImGui Window
  /// @return True if the button have been clicked
  bool CenteredButton();
  /// @brief Adds a button to the screen, this is the same as the ImGui one, but is from the custom struct
  /// @return True if the button have been clicked
  bool AddButton();
  /// @brief Calculate the button label text width
  /// @return The text width in float notation
  float TextWidth();
  String Text;
  String Name;
};

/// @brief  Holds the Imgui Image, and makes quick loading, without having to make the same boring process, over and over
struct InterfaceImage {
  InterfaceImage(Cchar path);
  InterfaceImage() {}
  void LoadInterfaceImage();
  void LoadInterfaceCenteredImage();
  void LoadObjectTextureImage(Yeager::Texture2D* texture);
  int ImageWidth = 0;
  int ImageHeight = 0;
  GLuint ImageTexture = 0;
};

struct InterfaceControl {
  bool Initialize = false;
  bool DontMoveWindowsEditor = true;
  bool CommentWindowIsOpen = false;
  bool ExitProgramWindowIsOpen = false;
  bool UserExitProgram = false;
  bool LauncherDone = false;
};

struct InterfaceFonts {
  float PixelSize = 14.0f;
};

struct LauncherProjectPicker;

struct OpenProjectsDisplay {
  OpenProjectsDisplay() {}
  OpenProjectsDisplay(const LauncherProjectPicker& other);
  void ConstructorFrom(const LauncherProjectPicker& other);
  String Name;
  String Author;
  String SceneType;
  String RendererType;
  String Path;
  String FolderPath;
  TimePointType TimeOfCreation;
  InterfaceImage ScreenShot;
  bool SelectedToDelete = false;
};

enum class ScreenShotMode { EFullScreen, EMiddleFixedSized, ECustomSizedAndPosition };

extern bool InputVector3(const char* label, Vector3* v, const char* format = "%.3f", ImGuiInputTextFlags flags = 0);

/**
 * @brief SpaceFitText is constructed with some big text and a maximum size by line interger, 
 * when the Display text is called, the function breaks the bigger text in smaller pieces that fit the maximum size and jumps a line
 */
struct SpaceFitText {
  SpaceFitText(const String& text, size_t size);
  SpaceFitText() = default;

  String Text = YEAGER_NULL_LITERAL;
  size_t Size = 0;
  std::vector<String> StringBlocks;

  void Build();
};

/// @brief Class that holds the interface management, use ImGui as library, the current program must be running just one instance of this class
/// @attention In the future, we need to write a self-custom GUI library in C for this Engine
/// @attention so we can have more control over the drawing methods and increase performance
class Interface {
 public:
  /// @brief Interface Constructor, loads fonts and the colorscheme readed from the configuration file
  /// @param window The GLFW window pointer the program is current using
  /// @param app The ApplicationCore pointer for management
  Interface(Window* window, Yeager::ApplicationCore* app);
  ~Interface();
  Interface() {}

  void Terminate();

  void InitRenderFrame();
  void TerminateRenderFrame();

  /// @brief Return a boolean representing if the program have been initialize with success
  /// @return True if initialize, false if not
  YEAGER_FORCE_INLINE bool getInitStatus() const { return m_Control.Initialize; }

  void ScreenShotWindow();
  /**
   * @brief When the user had choice the screenshot to be made, and is all set, the interface goes thrown a another loop
   * and makes all the interface windows to dissapear, so they doesnt appear at the final screenshot
   */
  void PrepareAndMakeScreenShot();
  /**
   * @brief Request a restart of imgui for a new window pointer to be created, it calls all destrory functions of imgui
   * @param window The old window pointer to the destrory
   */
  void RequestRestartInterface(Window* window);

  /// @brief The main function to be called, it check switch current mode the program is at, and pass to the correct function for the GUI rendering
  void RenderUI(Yeager::Launcher* launcher = YEAGER_NULLPTR);
  bool RenderLauncher(Yeager::Launcher* launcher = YEAGER_NULLPTR);
  /// @brief ImGui have a function that let the user choice the next window position, this function does the same
  /// @param size_x Window width
  /// @param size_y Window height
  void CenteredWindow(Uint size_x, Uint size_y);
  /// @brief Displays the current warning window stored in the Interface class
  void DisplayWarningWindow();
  /// @brief  Add a warning window to the Interface class, and waits to be display when the DisplayWarningWindow is called
  void AddWarningWindow(const String& warning, Uint size_x = 400, Uint size_y = 100);
  /// @brief Self explain, apply the colorscheme given to the interface UI by calling LoadColorscheme
  /// @param colorscheme The colorscheme to be apply
  YEAGER_FORCE_INLINE void ApplyColorscheme(ColorschemeConfig colorscheme)
  {
    m_Colorscheme = colorscheme;
    LoadColorscheme();
  }
  /// @brief Set a boolean value to the current interface, representing if the Exit Program window need to the rendered to the screen
  /// @param exit Boolean representing if the window should render
  YEAGER_FORCE_INLINE void SetExitProgramWindowOpen(bool exit) { m_Control.ExitProgramWindowIsOpen = exit; }
  /// @brief Get a boolean value representing if the current interface have the Exit Program window set to be rendered or already been render
  /// @return True if the window is set to render, false if not
  YEAGER_FORCE_INLINE bool GetExitProgramWindowOpen() const { return m_Control.ExitProgramWindowIsOpen; }
  /// @brief In the Exit Program window, the user can choice to cancel the operation or exit, this function return a boolean representing if the user choice to exit the program or not
  /// @return True if the user set to Exit the Program, false if not
  YEAGER_FORCE_INLINE bool GetUserChoiceExitProgram() const { return m_Control.UserExitProgram; }
  /// @brief Display the Exit Program Window
  /// @return The user choice to exit the program or not
  bool WindowExitProgram();
  /**
   * @brief Get the launcher information that the user had selected, in this case, if is done using the launcher
   * @return True if the user is done, False if not
   */
  YEAGER_FORCE_INLINE bool GetLauncherDone() const { return m_Control.LauncherDone; }

  /**
   * @brief Makes the screenshot window appear to the user
   * @param appear if the window must appear to the user
   */
  YEAGER_FORCE_INLINE void MakeScreenShotAppear(bool appear) { m_MakeScreenShotWindowShouldAppear = appear; }
  /**
   * @brief Returns a boolean representing if the screenshot window is appearing to the user
   * @return True if is appearing, false if not
   */
  YEAGER_FORCE_INLINE bool MakeScreenShotIsAppearing() const { return m_MakeScreenShotWindowShouldAppear; }

  void LightHandleControlWindow();
  void PhysXHandleControlWindow();

  bool GetDebugControlWindowOpen() const { return m_DebugControlWindowOpen; }
  void SetDebugControlWindowOpen(const bool open) { m_DebugControlWindowOpen = open; }
  void DebugControlWindow();

  /**
     * @brief Return a boolean represeting if in the current time the render frame is being drawn to the screen. 
     * That means that imgui is drawing the ui, and the terminate function have not been called yet!
     * @return True if is being drawn, false if not. 
     */
  YEAGER_FORCE_INLINE bool IsRenderFrameBeingDraw() const { return m_DrawingRenderFrame; }

  static void CenteredText(String text);
  static void AlignForWidth(float width, float alignment = 0.5f);
  static void CreateSpaceX(Uint count);

  void SetSceneFileText(const String& text) { m_SceneFileText = text; }

  Vector3 GetOpenGLDebugClearColor() const { return m_OpenGLDebugClearScreenColor; }

 private:
  String m_NewProjectCurrentRenderer = "Default";
  String m_NewProjectAuthorName = "Default";
  String m_NewProjectCurrentSceneType = "Default";
  String m_NewProjectCurrentPlataformTarget = "Default";
  String m_NewProjectFolder = "NONE";
  String m_NewScreenShootName;

  FolderExplorer m_FolderExplorer;

  bool bTerminated = false;

  /* The Drawing render frame is set to true when the imgui ui is being draw between the calls InitRenderFrame and TerminateRenderFrame*/
  bool m_DrawingRenderFrame = false;

  bool m_ConsoleShowMessages = true;
  bool m_ConsoleShowWarnings = true;
  bool m_ConsoleShowErrors = true;

  bool m_OpenProjectWindowOpen = false;
  bool m_NewProjectWindowOpen = false;
  bool m_NewProjectIsOkayToCreate = true;
  bool m_MakeScreenShotWindowShouldAppear = false;
  bool m_ReadyToScreenShot = false;
  bool m_WindowUserIsSureDeletingProject = false;
  bool m_NewProjectCreateDirectory = false;
  bool m_LauncherSettingsWindowOpen = false;
  bool m_EditorSettingsWindowOpen = false;
  bool m_DebugControlWindowOpen = false;

  Vector3 m_OpenGLDebugClearScreenColor = Vector3(0.0f);

  InterfaceControl m_Control;
  InterfaceFonts m_Fonts;
  ColorschemeConfig m_Colorscheme;
  InterfaceWarningWindow m_CurrentWarning;
  ScreenShotMode m_ScreenShotMode = ScreenShotMode::EFullScreen;

  Yeager::ApplicationCore* m_Application;
  Yeager::InterfaceWindow m_ScreenShotWindow;
  Yeager::LauncherProjectPicker* m_NewProjectHandle = YEAGER_NULLPTR;

  std::vector<OpenProjectsDisplay> m_OpenProjectToDisplay;
  std::vector<String> m_ProjectsNamesAlreadyTaken;
  std::pair<OpenProjectsDisplay, int> m_ProjectSelectedToDelete;

  std::vector<TemplateHandle> m_TemplatesAvaliable;
  TemplateHandle m_TemplateSelected;  // The template selected in the m_TemplateAvaliable
  bool m_TemplateWasSelected;         // The user want to use a template for the creation of the project

  static Uint m_Frames;
  int m_ScreenShotPosition[2] = {0};
  int m_ScreenShotSize[2] = {0};

  SpaceFitText m_LauncherInformation;

  std::shared_ptr<String> m_ImGuiConfigurationPath;

  void OpenProjectWindow(Yeager::Launcher* launcher, InterfaceButton& button, Yeager::InterfaceWindow& window);
  void NewProjectWindow(Yeager::Launcher* launcher, InterfaceButton& button, Yeager::InterfaceWindow& window);

  void LauncherSettingsWindow(InterfaceButton& button, Yeager::InterfaceWindow& window);
  void InitializeSettingsValues();
  void LauncherSettingsVideoSettings();
  InterfaceSettings m_InterfaceSettingsValues;
  VideoSettings m_VideoSettingsValues;
  /* Lets abreviate Video Settings to VS for the sake of the name of the variable */
  String m_VSAntiAliasingOptionSelected = YEAGER_NULL_LITERAL;
  String m_VSAspectRatioOptionSelected = YEAGER_NULL_LITERAL;

  String m_SceneFileText = YEAGER_NULL_LITERAL;

  void CreateImGuiContext();

  void LaunchImGui(Window* window);
  void DrawExplorer();
  void DrawToolbox();
  void DrawEditorMenu();
  void DrawConsole();

  void RenderEditor();
  void RenderDebugger();
  void LoadColorscheme();
  void WindowUserIsSureDeletingProject();

  String NewProjectCreateDirectory(String folder_path);
  /**
    @brief When deleting a project handle from the LoadedProject.yml configuration of the engine (the file that handles all the projects 
    name and path to directories) the vector that handles the projects been displayed in the interface isnt the same as the application 
    project handle, so, when the user deletes a project handle, this function is called with the project given path, to delete it in the application
    vector 
  */
  bool FindProjectHandleInApplicationAndDelete(String path);

  String TranslatableWithIcon(String icon, String translatable) const;
  String TranslatableWithID(String id, String translatable) const;

  void DisplayDirectionalLightControl(Yeager::PhysicalLightHandle* source);
  void DisplaySpotLightControl(Yeager::PhysicalLightHandle* source);
  void DisplayPointLightControl(Yeager::ObjectPointLight* source);
};
}  // namespace Yeager
