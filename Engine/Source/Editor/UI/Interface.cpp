#include "Interface.h"
#include "Common/Math/Mathematics.h"
#include "Common/Utils/Common.h"
#include "Explorer.h"
#include "Main/Core/Application.h"
#include "Main/IO/InputHandle.h"
#include "Main/IO/Serialization.h"
#include "Main/Window/Launcher.h"

using namespace ImGui;
using namespace Yeager;

Uint Yeager::Interface::m_Frames = 0;

OpenProjectsDisplay::OpenProjectsDisplay(const LauncherProjectPicker& other)
{
  ConstructorFrom(other);
}

void OpenProjectsDisplay::ConstructorFrom(const LauncherProjectPicker& other)
{
  Name = other.m_Name;
  Author = other.m_AuthorName;
  SceneType = SceneTypeToString(other.m_SceneType);
  RendererType = SceneRendererToString(other.m_SceneRenderer);
  Path = other.m_ProjectConfigurationPath;
  FolderPath = other.m_ProjectFolderPath;
  TimeOfCreation = other.m_ProjectDateOfCreation;
}

SpaceFitText::SpaceFitText(const String& text, size_t size)
{
  Text = text;
  Size = size;
  Build();
}

void SpaceFitText::Build()
{
  if (Size <= 0)
    return;
  /* bulding string is the string that occups a entire line */
  String buildingString;
  std::istringstream ss(Text);
  String word;
  bool firstWord = true;

  while (ss >> word) {
    /* When the word is bigger that the size, it will get a full line for itself, it also flush the current build string that waas being created */
    if (word.length() >= Size) {
      StringBlocks.push_back(buildingString);
      buildingString.clear();
      StringBlocks.push_back(word);
    }
    /* If the word plus the current string being build isnt bigger that the size, it appends and adds a space between, if it isnt the first word */
    if (buildingString.length() + word.length() + 1 <= Size) {
      if (!firstWord) {
        buildingString.append(String(" " + word));
      } else {
        buildingString.append(word);
        firstWord = false;
      }
      /* If the word plus the current string is bigger that the size, it flush the string being build and appends the word without space */
    } else {
      StringBlocks.push_back(buildingString);
      buildingString.clear();
      buildingString.append(String(word));
    }
  }
  /* The last string being built is forced to be flush at the end */
  StringBlocks.push_back(buildingString);
}

bool Yeager::InputVector3(const char* label, Vector3* v, const char* format, ImGuiInputTextFlags flags)
{
  float* ver[3] = {&v->x, &v->y, &v->z};
  return InputFloat3(label, *ver, format, flags);
}

void Interface::DisplayDirectionalLightControl(Yeager::PhysicalLightHandle* source)
{
  InputVector3(TranslatableWithID("Directional", "Common.Direction").c_str(),
               &source->GetDirectionalLight()->Direction);
  InputVector3(TranslatableWithID("Directional", "Editor.Panel.LS.Ambient").c_str(),
               &source->GetDirectionalLight()->Ambient);
  InputVector3(TranslatableWithID("Directional", "Editor.Panel.LS.Diffuse").c_str(),
               &source->GetDirectionalLight()->Diffuse);
  InputVector3(TranslatableWithID("Directional", "Editor.Panel.LS.Specular").c_str(),
               &source->GetDirectionalLight()->Specular);
  InputVector3(TranslatableWithID("Directional", "Common.Color").c_str(), &source->GetDirectionalLight()->Color);
}

void Interface::DisplaySpotLightControl(Yeager::PhysicalLightHandle* source)
{
  InputVector3(TranslatableWithID("SpotLight", "Common.Position").c_str(), &source->GetSpotLight()->Position);
  InputVector3(TranslatableWithID("SpotLight", "Common.Direction").c_str(), &source->GetSpotLight()->Direction);
  InputFloat(TranslatableWithID("SpotLight", "Editor.Panel.LS.CutOff").c_str(), &source->GetSpotLight()->CutOff);
  InputFloat(TranslatableWithID("SpotLight", "Editor.Panel.LS.OuterCutOff").c_str(),
             &source->GetSpotLight()->OuterCutOff);
  InputFloat(TranslatableWithID("SpotLight", "Editor.Panel.LS.Constant").c_str(), &source->GetSpotLight()->Constant);
  InputFloat(TranslatableWithID("SpotLight", "Editor.Panel.LS.Linear").c_str(), &source->GetSpotLight()->Linear);
  InputFloat(TranslatableWithID("SpotLight", "Editor.Panel.LS.Quadratic").c_str(), &source->GetSpotLight()->Quadratic);
  InputVector3(TranslatableWithID("SpotLight", "Editor.Panel.LS.Ambient").c_str(), &source->GetSpotLight()->Ambient);
  InputVector3(TranslatableWithID("SpotLight", "Editor.Panel.LS.Diffuse").c_str(), &source->GetSpotLight()->Diffuse);
  InputVector3(TranslatableWithID("SpotLight", "Editor.Panel.LS.Specular").c_str(), &source->GetSpotLight()->Specular);
  Checkbox(TranslatableWithID("SpotLight", "Common.Active").c_str(), &source->GetSpotLight()->Active);
}

void Interface::DisplayPointLightControl(Yeager::ObjectPointLight* source)
{
  InputVector3(TranslatableWithID("PointLight", "Common.Position").c_str(),
               &source->ObjSource->GetTransformationPtr()->position);
  InputVector3(TranslatableWithID("PointLight", "Common.Scale").c_str(),
               &source->ObjSource->GetTransformationPtr()->scale);
  InputVector3(TranslatableWithID("PointLight", "Common.Rotation").c_str(),
               &source->ObjSource->GetTransformationPtr()->rotation);
  InputFloat(TranslatableWithID("PointLight", "Editor.Panel.LS.Constant").c_str(), &source->Constant);
  InputFloat(TranslatableWithID("PointLight", "Editor.Panel.LS.Linear").c_str(), &source->Linear);
  InputFloat(TranslatableWithID("PointLight", "Editor.Panel.LS.Quadratic").c_str(), &source->Quadratic);
  InputVector3(TranslatableWithID("PointLight", "Editor.Panel.LS.Ambient").c_str(), &source->Ambient);
  InputVector3(TranslatableWithID("PointLight", "Editor.Panel.LS.Specular").c_str(), &source->Specular);
  InputVector3(TranslatableWithID("PointLight", "Editor.Panel.LS.Diffuse").c_str(), &source->Diffuse);
  InputVector3(TranslatableWithID("PointLight", "Common.Color").c_str(), &source->Color);
  Checkbox(TranslatableWithID("PointLight", "Common.Active").c_str(), &source->Active);
}

void Interface::LightHandleControlWindow()
{
  Locale locale = m_Application->GetLocale();
  Begin(TranslatableWithIcon(ICON_FA_LIGHTBULB, "Window.LS.Label").c_str());
  int label_id = 0;
  for (auto& lightsources : *m_Application->GetScene()->GetLightSources()) {
    PushID(label_id++);
    if (Button(locale.Translate("Editor.Panel.LS.Add.LS.Btn").c_str())) {
      ObjectPointLight light;
      Transformation3D trans(m_Application->GetCamera()->InFront(), Vector3(0.0f), Vector3(0.1f));
      lightsources->AddObjectPointLight(light, trans);
    }

    SameLine();
    if (Button(locale.Translate("Editor.Panel.LS.Delete.Btn").c_str())) {
      lightsources->SetScheduleDeletion(true);
    }

    if (CollapsingHeader(locale.Translate("Editor.Panel.LS.Linked.Shaders.Label").c_str())) {
      for (const auto& shader : *lightsources->GetLinkedShaders()) {
        Text("%s", shader->GetName().c_str());
      }
    }

    if (TreeNode(locale.Translate("Editor.Panel.LS.Dir.Light.Label").c_str())) {
      PushID(label_id++);
      DisplayDirectionalLightControl(lightsources.get());
      PopID();
      TreePop();
    }

    if (TreeNode(locale.Translate("Editor.Panel.LS.Spot.Light.Label").c_str())) {
      PushID(label_id++);
      DisplaySpotLightControl(lightsources.get());
      PopID();
      TreePop();
    }

    if (TreeNode(locale.Translate("Editor.Panel.LS.Point.Lights.Label").c_str())) {
      for (auto& obj_light : *lightsources->GetObjectPointLights()) {
        PushID(label_id++);
        DisplayPointLightControl(&obj_light);
        PopID();
      }
      TreePop();
    }
    PopID();
  }
  End();
}

Interface::~Interface()
{
  if (!bTerminated)
    Terminate();
}

void Interface::Terminate()
{
  SaveIniSettingsToDisk(m_ImGuiConfigurationPath->c_str());
  Yeager::Log(INFO, "Destrorying Interface!");
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImPlot::DestroyContext();
  DestroyContext();
  m_ImGuiConfigurationPath.reset();
  bTerminated = true;
}

bool Interface::FindProjectHandleInApplicationAndDelete(String path)
{
  for (Uint index = 0; index < m_Application->GetLoadedProjectsHandles()->size(); index++) {
    Yeager::LoadedProjectHandle* project = &m_Application->GetLoadedProjectsHandles()->at(index);
    if (project->ProjectFolderPath == path) {
      m_Application->GetLoadedProjectsHandles()->erase(m_Application->GetLoadedProjectsHandles()->begin() + index);
      return true;
    }
  }
  return false;
}

void Interface::DebugControlWindow()
{
  CenteredWindow(500, 500);
  Begin("Debug control window");

  if (Button("Spawn PhysX cube")) {
    Yeager::BaseCamera* camera = m_Application->GetCamera();
    SpawnCubeObject(m_Application, "DEBUG_DEFAULT", camera->GetPosition() + camera->GetDirection(), Vector3(0.0f),
                    Vector3(1.0f), ObjectPhysicsType::eDYNAMIC_BODY);
  }

  End();
}

bool Interface::WindowExitProgram()
{
  Locale locale = m_Application->GetLocale();
  m_Application->GetInput()->SetCursorCanDisappear(false);
  m_Application->GetCamera()->SetShouldMove(false);
  CenteredWindow(350, 140);
  Begin(locale.Translate("Exit.Dia.Title").c_str(), NULL, YEAGER_WINDOW_STATIC);

  CenteredText(locale.Translate("Exit.Dia.Msg1"));
  CenteredText(locale.Translate("Exit.Dia.Msg2"));

  if (Button(locale.Translate("Common.Exit").c_str())) {
    End();
    m_Control.ExitProgramWindowIsOpen = false;
    return true;
  }
  if (Button(locale.Translate("Common.Cancel").c_str())) {
    m_Control.ExitProgramWindowIsOpen = false;
    m_Application->GetInput()->SetCursorCanDisappear(true);
    m_Application->GetCamera()->SetShouldMove(true);
  }
  End();
  return false;
}

void Interface::CenteredWindow(Uint size_x, Uint size_y)
{
  const Vector2 wndSize = m_Application->GetWindow()->GetWindowSize();
  SetNextWindowPos(ImVec2((wndSize.x / 2) - (size_x / 2), (wndSize.y / 2) - (size_y / 2)));
  SetNextWindowSize(ImVec2(size_x, size_y));
}

bool InterfaceButton::CenteredButton()
{
  auto windowWidth = GetWindowSize().x;
  auto textWidth = CalcTextSize(Text.c_str()).x;
  SetCursorPosX((windowWidth - textWidth) * 0.5f);
  return Button(Text.c_str());
}

void Interface::DisplayWarningWindow()
{
  if (m_CurrentWarning.Active) {
    CenteredWindow(m_CurrentWarning.SizeWidth, m_CurrentWarning.SizeHeight);
    Begin("Warning!");
    CenteredText(m_CurrentWarning.Warning);
    if (m_CurrentWarning.FirstLogWarning) {  // Doesnt loop various warning forever, crashes if so
      Yeager::Log(WARNING, "{}", m_CurrentWarning.Warning.c_str());
      m_CurrentWarning.FirstLogWarning = false;
    }
    Spacing();
    if (Button("Understood")) {  // Reset for next warning window
      m_CurrentWarning.Active = false;
      m_CurrentWarning.Warning.clear();
      m_CurrentWarning.FirstLogWarning = true;
    }
    End();
  }
}

void Interface::AddWarningWindow(const String& warning, Uint size_x, Uint size_y)
{
  m_CurrentWarning.Warning = warning;
  m_CurrentWarning.SizeWidth = size_x;
  m_CurrentWarning.SizeHeight = size_y;
  m_CurrentWarning.Active = true;
}

bool InterfaceButton::AddButton()
{
  return Button(Text.c_str());
}

float InterfaceButton::TextWidth()
{
  return CalcTextSize(Text.c_str()).x;
}

void InterfaceImage::LoadInterfaceImage()
{
  Image((void*)(intptr_t)ImageTexture, ImVec2(ImageWidth / 2, ImageHeight / 2));
}

void InterfaceImage::LoadInterfaceCenteredImage()
{
  auto windowWidth = GetWindowSize().x;
  SetCursorPosX((windowWidth - ImageWidth) * 0.5f);
  Image((void*)(intptr_t)ImageTexture, ImVec2(ImageWidth / 2, ImageHeight / 2));
}

void Interface::AlignForWidth(float width, float alignment)
{
  float avail = GetContentRegionAvail().x;
  float off = (avail - width) * alignment;
  if (off > 0.0f)
    SetCursorPosX(GetCursorPosX() + off);
}

void Interface::LaunchImGui(Window* window)
{

  IMGUI_CHECKVERSION();
  CreateContext();
  ImPlot::CreateContext();

  ImGuiIO& m_imgui_io = GetIO();
  (void)m_imgui_io;
  m_imgui_io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  m_imgui_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  m_imgui_io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Keyboard Controls

  if (GetPathFromLocal("/Configuration/Interface/imgui.ini").has_value()) {
    m_ImGuiConfigurationPath = std::make_shared<String>(GetPathFromLocal("/Configuration/Interface/imgui.ini").value());
    m_imgui_io.IniFilename = m_ImGuiConfigurationPath->c_str();
    LoadIniSettingsFromDisk(m_ImGuiConfigurationPath->c_str());
  } else {
    m_ImGuiConfigurationPath =
        std::make_shared<String>(GetPathFromSourceCode("/Assets/Configuration/Editor/Public/Interface/imgui.ini"));
    m_imgui_io.IniFilename = m_ImGuiConfigurationPath->c_str();
  }

  StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window->GetGLFWwindow(), true);
  ImGui_ImplOpenGL3_Init(YEAGER_IMGUI_OPENGL_VERSION);
}

void Interface::RequestRestartInterface(Window* window)
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();

  ImGui_ImplGlfw_InitForOpenGL(window->GetGLFWwindow(), true);
  ImGui_ImplOpenGL3_Init(YEAGER_IMGUI_OPENGL_VERSION);
}

void Interface::CreateImGuiContext()
{
  ImGuiIO& m_imgui_io = GetIO();
  ImFontConfig config;
  m_imgui_io.Fonts->AddFontDefault();
  config.MergeMode = true;
  config.GlyphMinAdvanceX = 13.0f;
  static const ImWchar icon_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};

  m_imgui_io.Fonts->AddFontFromFileTTF(
      GetPathFromShared("/Resources/Fonts/Editor/IosevkaNerdFont-Medium.ttf").value().c_str(), m_Fonts.PixelSize,
      &config, icon_ranges);
  m_imgui_io.Fonts->Build();
}

Interface::Interface(Window* window, Yeager::ApplicationCore* app)
    : m_Application(app),
      m_ScreenShotWindow(Yeager::InterfaceWindow(m_Application, "ScreenShot", ImVec2(400, 400), ImVec2(0, 0), true,
                                                 Yeager::WindowRelativePos::MIDDLE)),
      m_FolderExplorer(FolderExplorer(app))
{
  LaunchImGui(window);
  CreateImGuiContext();
  LoadColorscheme();

  m_Control.Initialize = true;
  Yeager::EngineEditorMenuBar.HasMenuBar = true;
  m_LauncherInformation = SpaceFitText(m_Application->GetLocale().Translate("Launcher.About.Msg"), 80);
}

void Interface::CreateSpaceX(Uint count)
{
  if (count > 0) {
    for (Uint x = 0; x < count; x++) {
      NewLine();
    }
  }
}

void Interface::InitRenderFrame()
{
  /** @note Changes in the window system (glfw) cannot happen during the drawing render frame! 
  Request are made to the engine to perform such changes after the frame has been draw! */
  m_DrawingRenderFrame = true;
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  NewFrame();
}

void Interface::TerminateRenderFrame()
{
  Render();
  ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
  m_DrawingRenderFrame = false;
}

void Interface::RenderUI(Yeager::Launcher* launcher)
{
  m_Frames++;

  switch (m_Application->GetMode()) {
    case ApplicationMode::eAPPLICATION_LAUNCHER:
      RenderLauncher(launcher);
      DisplayWarningWindow();
      break;
    case ApplicationMode::eAPPLICATION_EDITOR:
      if (m_MakeScreenShotWindowShouldAppear) {
        ScreenShotWindow();
      }
      RenderEditor();
      DisplayWarningWindow();
      break;
    default:
      RenderEditor();
  }

  if (m_Control.ExitProgramWindowIsOpen) {
    m_Control.UserExitProgram = WindowExitProgram();
  }
}

void Interface::DrawConsole()
{
  const String consoleStr = String(ICON_FA_TERMINAL " " + m_Application->GetLocale().Translate("Window.Console.Label"));
  Begin(consoleStr.c_str());

  Checkbox(ICON_FA_INFO " Messages", &m_ConsoleShowMessages);
  SameLine();
  Checkbox(ICON_FA_SQUARE " Warnings", &m_ConsoleShowWarnings);
  SameLine();
  Checkbox(ICON_FA_XMARK " Errors", &m_ConsoleShowErrors);
  Separator();

  BeginChild("Message");

  for (const auto& msg : *gGlobalConsole.GetLogs()) {
    switch (msg.type) {
      case MessageTypeVerbosity::Info_Message:
        if (m_ConsoleShowMessages)
          TextColored(msg.text_color, "%s", msg.message.c_str());
        break;
      case MessageTypeVerbosity::Warning_Message:
        if (m_ConsoleShowWarnings)
          TextColored(msg.text_color, "%s", msg.message.c_str());
        break;
      case MessageTypeVerbosity::Error_Message:
        if (m_ConsoleShowErrors)
          TextColored(msg.text_color, "%s", msg.message.c_str());
        break;
      default:
        TextColored(msg.text_color, "%s", msg.message.c_str());
    }
  }

  EndChild();

  End();
}

void Interface::DrawExplorer()
{
  //m_ExplorerWindow.Begin(m_Control.DontMoveWindowsEditor ? kWindowStatic : kWindowMoveable);
  Begin(ICON_FA_ADDRESS_BOOK " Explorer", NULL, ImGuiWindowFlags_NoResize);
  m_Application->GetExplorer()->DrawExplorer();
  End();
}

void Interface::DrawToolbox()
{
  Begin(ICON_FA_GEAR " Toolbox");

  // TODO clean this mess
  Yeager::ToolboxHandle* toolbox = m_Application->GetExplorer()->GetSelectedToolbox();
  if (toolbox != YEAGER_NULLPTR && toolbox->GetEntity() != YEAGER_NULLPTR) {
    Yeager::ToolboxHandle* obj = m_Application->GetExplorer()->GetSelectedToolbox();
    Text("%s", obj->GetEntity()->GetName().c_str());
    obj->DrawObject();
  } else {
    CenteredText(ICON_FA_EXCLAMATION " [No selected toolbox!]");
  }

  End();
}

void Interface::DrawEditorMenu()
{
  if (ImGui::BeginMainMenuBar()) {
    Yeager::EngineEditorMenuBar.MenuBarSize = GetWindowSize();
    if (ImGui::BeginMenu(ICON_FA_FILE " File")) {

      if (ImGui::MenuItem("Create")) {}
      if (ImGui::MenuItem("Open", "Ctrl+O")) {}
      if (ImGui::MenuItem("Save", "Ctrl+S")) {
        m_Application->GetScene()->Save();
      }
      if (ImGui::MenuItem("Save as..")) {}
      if (ImGui::MenuItem("Project Settings")) {
        m_LauncherSettingsWindowOpen = true;
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu(ICON_FA_GEAR " Build")) {
      if (ImGui::MenuItem("Build project")) {}
      if (ImGui::MenuItem("Link C++ code")) {}
      if (ImGui::MenuItem("Open project in editor")) {}
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu(ICON_FA_BOXES_PACKING " Plugins")) {
      if (ImGui::MenuItem("Plugins loaded")) {}
      if (ImGui::MenuItem("Load plugin")) {}
      if (ImGui::MenuItem("Unload plugin")) {}
      if (ImGui::MenuItem("Install plugin")) {}
      if (ImGui::MenuItem("Create plugin")) {}
      if (ImGui::MenuItem("Help")) {}
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu(ICON_FA_EXCLAMATION " Settings")) {
      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }
}

void Interface::RenderEditor()
{
  if (!Yeager::EngineEditorWindowShouldVanish) {
    DrawExplorer();
    DrawToolbox();
#ifdef YEAGER_DEBUG
    RenderDebugger();
#endif
    DrawEditorMenu();
    DrawConsole();
    m_FolderExplorer.DrawWindow();

    PhysXHandleControlWindow();

    LightHandleControlWindow();
  }
  if (m_DebugControlWindowOpen) {
    // TODO debug control window must be programmed to troubleshoot the engine too!
    DebugControlWindow();  // This window is not affect by the affect of the vanish windows of the editor
  }
}

void Interface::WindowUserIsSureDeletingProject()
{
  const int height = 600, width = 600;
  const Vector2 wndSize = m_Application->GetWindow()->GetWindowSize();
  SetNextWindowPos(ImVec2((wndSize.x / 2) - (width / 2), (wndSize.y / 2) - (height / 2)));
  SetNextWindowSize(ImVec2(width, height));
  Begin("Are you sure you want to delete this project?", NULL, YEAGER_WINDOW_STATIC);

  if (Button("Yes")) {
    if (!FindProjectHandleInApplicationAndDelete(m_ProjectSelectedToDelete.first.FolderPath)) {
      Yeager::Log(ERROR, "Interface cannot find the project handle associated with the path {} to delete",
                  m_ProjectSelectedToDelete.first.FolderPath);
    }
    m_OpenProjectToDisplay.erase(m_OpenProjectToDisplay.begin() + m_ProjectSelectedToDelete.second);
    m_WindowUserIsSureDeletingProject = false;
  }

  End();
}

void Interface::OpenProjectWindow(Yeager::Launcher* launcher, InterfaceButton& button, Yeager::InterfaceWindow& window)
{
  if (button.AddButton()) {
    m_OpenProjectWindowOpen = true;
    m_OpenProjectToDisplay = Yeager::ReadProjectsToDisplay(
        GetPathFromLocal("/Configuration/Projects/LoadedProjectsPath.yml").value(), m_Application);
  }

  if (m_OpenProjectWindowOpen && !m_NewProjectWindowOpen) {
    window.Begin(YEAGER_WINDOW_STATIC);

    if (m_OpenProjectToDisplay.empty())
      CenteredText("No projects have been created!");

    for (Uint index = 0; index < m_OpenProjectToDisplay.size(); index++) {

      OpenProjectsDisplay project = m_OpenProjectToDisplay.at(index);

      Text("[%s] By: %s", project.Name.c_str(), project.Author.c_str());
      Text("[%s] [%s]", project.RendererType.c_str(), project.SceneType.c_str());
      Text("[Folder path]: %s", project.FolderPath.c_str());
      Text("[Created at]: %s", TimePointType::FormatTimeToString(project.TimeOfCreation).c_str());

      if (Button(String("Open##" + project.Name).c_str())) {
        launcher->SetUserHasSelect(true);
        launcher->GetCurrentProjectPicked()->ConstructorFrom(project);
        m_SceneFileText = FileContentToString(project.Path);
      }

      SameLine();

      if (Button(String("Delete##" + project.Name).c_str())) {
        m_WindowUserIsSureDeletingProject = true;
        project.SelectedToDelete = true;
        m_ProjectSelectedToDelete.first = project;
        m_ProjectSelectedToDelete.second = index;
      }

      Separator();
    }

    if (Button("Cancel"))
      m_OpenProjectWindowOpen = false;

    if (m_WindowUserIsSureDeletingProject)
      WindowUserIsSureDeletingProject();

    window.End();
  }
}

void Interface::NewProjectWindow(Yeager::Launcher* launcher, InterfaceButton& button, Yeager::InterfaceWindow& window)
{
  if (button.AddButton()) {
    m_NewProjectWindowOpen = true;
    m_NewProjectHandle = new Yeager::LauncherProjectPicker();
    m_TemplatesAvaliable =
        m_Application->GetSerial()->FindTemplatesFromSharedFolder(GetPathFromShared("/Templates").value());
    m_TemplateWasSelected = false;
  }

  if (m_NewProjectWindowOpen && !m_OpenProjectWindowOpen) {

    window.Begin(YEAGER_WINDOW_STATIC);

    const std::vector<Cchar> RendererType = {"OpenGL3_3", "OpenGL4"};
    const std::vector<Cchar> SceneType = {"Scene2D", "Scene3D"};
    const std::vector<Cchar> PlataformTarget = {"Linux System", "Windows 10/11", "Android SDK",
                                                "MacOS",        "IOS",           "Universal"};

    InputText("Project`s Name", &m_NewProjectHandle->m_Name);

    for (const auto& str : m_ProjectsNamesAlreadyTaken) {
      if (m_NewProjectHandle->m_Name == str) {
        TextColored(IMGUI_RED_ERROR_COLOR, "Name already taken! Please input another one");
        m_NewProjectIsOkayToCreate = false;
      } else {
        m_NewProjectIsOkayToCreate = true;
      }
    }
    if (Button("Select folder")) {
      auto selection = pfd::select_folder("Select project folder", GetExternalLocalFolderPath()).result();
      if (!selection.empty()) {
        /* This should always return true, but we are making sure the user or the machine deletes the folder selected during the midtime */
        if (Yeager::ValidatesPath(selection)) {
          m_NewProjectHandle->m_ProjectFolderPath = selection + YG_PS;
        }
      }
    }

    SameLine();
    Text("Project Folder: %s", m_NewProjectHandle->m_ProjectFolderPath.c_str());
    SameLine();
    Checkbox("Create Directory", &m_NewProjectCreateDirectory);

    if (m_NewProjectCreateDirectory &&
        ValidatesPath(m_NewProjectHandle->m_ProjectFolderPath + m_NewProjectHandle->m_Name)) {
      TextColored(IMGUI_RED_ERROR_COLOR, "Directory already exists!");
      m_NewProjectIsOkayToCreate = false;
    } else {
      m_NewProjectIsOkayToCreate = true;
    }

    InputText("Author`s Name", &m_NewProjectAuthorName);

    if (BeginCombo("Renderer Type", m_NewProjectCurrentRenderer.c_str())) {
      for (Uint x = 0; x < RendererType.size(); x++) {
        bool is_selected = (m_NewProjectCurrentRenderer.c_str() == RendererType[x]);
        if (Selectable(RendererType[x], is_selected)) {
          m_NewProjectCurrentRenderer = RendererType[x];
          m_NewProjectHandle->m_SceneRenderer = Yeager::StringToSceneRenderer(m_NewProjectCurrentRenderer);
        }
        if (is_selected)
          SetItemDefaultFocus();
      }
      EndCombo();
    }

    if (BeginCombo("Scene Type", m_NewProjectCurrentSceneType.c_str())) {
      for (Uint x = 0; x < SceneType.size(); x++) {
        bool is_selected = (m_NewProjectCurrentSceneType.c_str() == SceneType[x]);
        if (Selectable(SceneType[x], is_selected)) {
          m_NewProjectCurrentSceneType = SceneType[x];
          m_NewProjectHandle->m_SceneType = Yeager::StringToSceneType(m_NewProjectCurrentSceneType);
        }
        if (is_selected)
          SetItemDefaultFocus();
      }
      EndCombo();
    }

    if (BeginCombo("Plataform Target", m_NewProjectCurrentPlataformTarget.c_str())) {
      for (Uint x = 0; x < PlataformTarget.size(); x++) {
        bool is_selected = (m_NewProjectCurrentPlataformTarget.c_str() == PlataformTarget[x]);
        if (Selectable(PlataformTarget[x], is_selected))
          m_NewProjectCurrentPlataformTarget = PlataformTarget[x];
        m_NewProjectHandle->m_PlataformTarget = m_NewProjectCurrentPlataformTarget;
        if (is_selected)
          SetItemDefaultFocus();
      }
      EndCombo();
    }

    CenteredText("Templates Avaliable");
    for (const auto& t : m_TemplatesAvaliable) {
      Text("Name: %s. Description: %s ", t.Name.c_str(), t.Description.c_str());
      SameLine();
      if (Button(String("Select##" + t.Name).c_str())) {
        m_TemplateSelected = t;
        m_TemplateWasSelected = true;
      }
    }
    if (m_TemplateWasSelected) {
      Text(m_TemplateSelected.Name.c_str());
    }

    if (Button("Create")) {
      if (m_NewProjectIsOkayToCreate) {
        TimePointType current_time = TimePointType::CurrentTimeToTimeType();
        m_NewProjectHandle->m_ProjectDateOfCreation = current_time;
        if (m_NewProjectCreateDirectory) {
          m_NewProjectHandle->m_ProjectFolderPath = NewProjectCreateDirectory(m_NewProjectHandle->m_ProjectFolderPath);
        }
        m_NewProjectHandle->m_ProjectConfigurationPath =
            m_NewProjectHandle->m_ProjectFolderPath + m_NewProjectHandle->m_Name + ".yml";
        m_NewProjectHandle->m_AuthorName = m_NewProjectAuthorName;

        if (m_TemplateWasSelected)
          m_NewProjectHandle->m_TemplateHandle = m_TemplateSelected;

        launcher->BuildNewProject(*m_NewProjectHandle);
        launcher->SetUserHasSelect(true);
        launcher->SetNewProjectLoaded(true);
        LoadedProjectHandle handle;
        handle.ProjectName = m_NewProjectHandle->m_Name;
        handle.ProjectFolderPath = m_NewProjectHandle->m_ProjectFolderPath;
        handle.ProjectConfigurationPath = m_NewProjectHandle->m_ProjectConfigurationPath;
        m_Application->GetLoadedProjectsHandles()->push_back(handle);
        delete m_NewProjectHandle;
      }
    }

    SameLine();

    if (Button("Cancel")) {
      delete m_NewProjectHandle;
      m_NewProjectWindowOpen = false;
    }

    window.End();
  }
}

void Interface::LauncherSettingsVideoSettings()
{
  const std::vector<const char*> antiAliasingOptions = {"MSAA x2", "MSAA x4"};

  if (BeginCombo("Anti Aliasing *", m_VSAntiAliasingOptionSelected.c_str())) {
    for (Uint x = 0; x < antiAliasingOptions.size(); x++) {
      bool is_selected = (m_VSAntiAliasingOptionSelected.c_str() == antiAliasingOptions[x]);
      if (Selectable(antiAliasingOptions[x], is_selected))
        m_VSAntiAliasingOptionSelected = antiAliasingOptions[x];
      m_VideoSettingsValues.AntiAliasingType = AntiAliasingOption::toEnum(m_VSAntiAliasingOptionSelected);
      if (is_selected)
        SetItemDefaultFocus();
    }
    EndCombo();
  }

  const std::vector<const char*> aspectRatioOptions = {"4:3", "21:9"};

  if (BeginCombo("Aspect Ratio ", m_VSAspectRatioOptionSelected.c_str())) {
    for (Uint x = 0; x < aspectRatioOptions.size(); x++) {
      bool is_selected = (m_VSAspectRatioOptionSelected.c_str() == aspectRatioOptions[x]);
      if (Selectable(aspectRatioOptions[x], is_selected))
        m_VSAspectRatioOptionSelected = aspectRatioOptions[x];
      m_VideoSettingsValues.WindowDesireAspectRatio = AspectRatio::ToEnum(m_VSAspectRatioOptionSelected);
      if (is_selected)
        SetItemDefaultFocus();
    }
    EndCombo();
  }

  Checkbox("Framebuffer obeys aspect ratio", &m_VideoSettingsValues.WindowObeysAspectRatio);
}

void Interface::InitializeSettingsValues()
{
  m_LauncherSettingsWindowOpen = true;
  m_InterfaceSettingsValues = m_Application->GetSettings()->GetInterfaceSettingsStruct();
  m_VideoSettingsValues = m_Application->GetSettings()->GetVideoSettingsStruct();
  m_VSAntiAliasingOptionSelected = AntiAliasingOption::toString(m_VideoSettingsValues.AntiAliasingType);
  m_VSAspectRatioOptionSelected = AspectRatio::ToString(m_VideoSettingsValues.WindowDesireAspectRatio);
}

void Interface::LauncherSettingsWindow(InterfaceButton& button, Yeager::InterfaceWindow& window)
{
  if (button.AddButton()) {
    InitializeSettingsValues();
  }

  if (m_LauncherSettingsWindowOpen) {
    window.Begin(YEAGER_WINDOW_STATIC);

    if (CollapsingHeader("Video Settings")) {
      LauncherSettingsVideoSettings();
    }

    if (CollapsingHeader("Inteface Settings")) {
      InputFloat("Global on screen text scale", &m_InterfaceSettingsValues.GlobalOnScreenTextScale);
    }

    if (Button("Apply")) {
      Settings* settings = m_Application->GetSettings();
      m_LauncherSettingsWindowOpen = false;
      if (settings->GetVideoSettingsStruct().AntiAliasingType != m_VideoSettingsValues.AntiAliasingType)
        settings->ChangeVideoSettingsAntiAliasingType(m_VideoSettingsValues.AntiAliasingType);
      if (settings->GetVideoSettingsStruct().WindowObeysAspectRatio != m_VideoSettingsValues.WindowObeysAspectRatio)
        settings->GetVideoSettingsStructPtr()->WindowObeysAspectRatio = m_VideoSettingsValues.WindowObeysAspectRatio;
      if (settings->GetVideoSettingsStruct().WindowDesireAspectRatio != m_VideoSettingsValues.WindowDesireAspectRatio)
        settings->GetVideoSettingsStructPtr()->WindowDesireAspectRatio = m_VideoSettingsValues.WindowDesireAspectRatio;

      if (settings->GetInterfaceSettingsStruct().GlobalOnScreenTextScale !=
          m_InterfaceSettingsValues.GlobalOnScreenTextScale)
        settings->GetInterfaceSettingsStructPtr()->GlobalOnScreenTextScale =
            m_InterfaceSettingsValues.GlobalOnScreenTextScale;
    }

    SameLine();

    if (Button("Cancel")) {
      m_LauncherSettingsWindowOpen = false;
    }

    SameLine();

    Text("* Applying such changes will make the window to reload itself to take effect!");

    window.End();
  }
}

String Interface::TranslatableWithIcon(String icon, String translatable) const
{
  return String(icon + " " + m_Application->GetLocale().Translate(translatable));
}

String Interface::TranslatableWithID(String id, String translatable) const
{
  return String(m_Application->GetLocale().Translate(translatable) + "##" + id);
}

bool Interface::RenderLauncher(Yeager::Launcher* launcher)
{
  Locale locale = m_Application->GetLocale();
  InterfaceButton open_project("interface_open_project",
                               ICON_FA_FOLDER_OPEN " " + locale.Translate("Launcher.OpenProject.Btn"));
  InterfaceButton new_project("interface_new_project",
                              ICON_FA_FOLDER " " + locale.Translate("Launcher.NewProject.Btn"));
  InterfaceButton settings_button("interface_settings_button",
                                  ICON_FA_GEARS " " + locale.Translate("Launcher.Settings.Btn"));
  InterfaceButton help_button("interface_help_button", ICON_FA_CIRCLE_QUESTION " Help");

  Yeager::WindowInfo* wnd = m_Application->GetWindow()->GetWindowInformationPtr();
  Yeager::InterfaceWindow open_project_window(m_Application, "Open Project", ImVec2(1000, 400),
                                              ImVec2(wnd->LauncherSize.x / 2, wnd->LauncherSize.y / 2), true,
                                              Yeager::WindowRelativePos::MIDDLE);
  Yeager::InterfaceWindow new_project_window(m_Application, "Create New Project", ImVec2(1000, 400),
                                             ImVec2(wnd->LauncherSize.x / 2, wnd->LauncherSize.y / 2), true,
                                             Yeager::WindowRelativePos::MIDDLE);

  Yeager::InterfaceWindow settings_window(m_Application, "Launcher Settings", ImVec2(1000, 400),
                                          ImVec2(wnd->LauncherSize.x / 2, wnd->LauncherSize.y / 2), true,
                                          Yeager::WindowRelativePos::MIDDLE);

  const Vector2 windowSize = m_Application->GetWindow()->GetWindowSize();
  SetNextWindowPos(ImVec2(0, 0));
  SetNextWindowSize(ImVec2(windowSize.x, windowSize.y));
  Begin(locale.Translate("Window.Launcher.Label").c_str(), NULL, YEAGER_LAUNCHER_WINDOW_FLAGS);

  CenteredText(locale.Translate("Launcher.Greetings.Msg"));
  for (const auto& str : m_LauncherInformation.StringBlocks) {
    if (!str.empty())
      CenteredText(str);
  }
  CenteredText(m_Application->GetLocale().Translate("Launcher.About.GithubLink"));

  CreateSpaceX(2);

  ImGuiStyle& style = GetStyle();

  float width = 0.0f;
  width += CalcTextSize(open_project.Text.c_str()).x;
  width += style.ItemSpacing.x;
  width += CalcTextSize(new_project.Text.c_str()).x;
  width += style.ItemSpacing.x;
  width += CalcTextSize(settings_button.Text.c_str()).x;
  width += style.ItemSpacing.x;
  width += CalcTextSize(help_button.Text.c_str()).x;
  AlignForWidth(width);

  OpenProjectWindow(launcher, open_project, open_project_window);

  SameLine();

  NewProjectWindow(launcher, new_project, new_project_window);

  SameLine();

  LauncherSettingsWindow(settings_button, settings_window);

  SameLine();

  End();

  RenderDebugger();

  return true;
}

String Interface::NewProjectCreateDirectory(String folder_path)
{
  String str = folder_path + m_NewProjectHandle->m_Name;
  if (!ValidatesPath(str)) {
    std::filesystem::create_directory(str);
    return String(str + YG_PS);
  }
  return folder_path;
}

void Interface::CenteredText(String text)
{
  auto windowWidth = GetWindowSize().x;
  auto textWidth = CalcTextSize(text.c_str()).x;
  SetCursorPosX((windowWidth - textWidth) * 0.5f);
  Text(text.c_str());
}

void Interface::LoadColorscheme()
{
  ImGuiStyle& style = ImGui::GetStyle();
  style.WindowRounding = m_Colorscheme.WindowRounding;
  style.FrameRounding = m_Colorscheme.FrameRounding;
  style.ScrollbarRounding = m_Colorscheme.ScrollbarRounding;
  style.Colors[ImGuiCol_Text] = m_Colorscheme.Colors_ImGuiCol_Text;
  style.Colors[ImGuiCol_TextDisabled] = m_Colorscheme.Colors_ImGuiCol_TextDisabled;
  style.Colors[ImGuiCol_WindowBg] = m_Colorscheme.Colors_ImGuiCol_WindowBg;
  style.Colors[ImGuiCol_ChildBg] = m_Colorscheme.Colors_ImGuiCol_ChildBg;
  style.Colors[ImGuiCol_PopupBg] = m_Colorscheme.Colors_ImGuiCol_PopupBg;
  style.Colors[ImGuiCol_Border] = m_Colorscheme.Colors_ImGuiCol_Border;
  style.Colors[ImGuiCol_BorderShadow] = m_Colorscheme.Colors_ImGuiCol_BorderShadow;
  style.Colors[ImGuiCol_FrameBg] = m_Colorscheme.Colors_ImGuiCol_FrameBg;
  style.Colors[ImGuiCol_FrameBgHovered] = m_Colorscheme.Colors_ImGuiCol_FrameBgHovered;
  style.Colors[ImGuiCol_FrameBgActive] = m_Colorscheme.Colors_ImGuiCol_FrameBgActive;
  style.Colors[ImGuiCol_TitleBg] = m_Colorscheme.Colors_ImGuiCol_TitleBg;
  style.Colors[ImGuiCol_TitleBgCollapsed] = m_Colorscheme.Colors_ImGuiCol_TitleBgCollapsed;
  style.Colors[ImGuiCol_TitleBgActive] = m_Colorscheme.Colors_ImGuiCol_TitleBgActive;
  style.Colors[ImGuiCol_MenuBarBg] = m_Colorscheme.Colors_ImGuiCol_MenuBarBg;
  style.Colors[ImGuiCol_ScrollbarBg] = m_Colorscheme.Colors_ImGuiCol_ScrollbarBg;
  style.Colors[ImGuiCol_ScrollbarGrab] = m_Colorscheme.Colors_ImGuiCol_ScrollbarGrab;
  style.Colors[ImGuiCol_ScrollbarGrabHovered] = m_Colorscheme.Colors_ImGuiCol_ScrollbarGrabHovered;
  style.Colors[ImGuiCol_ScrollbarGrabActive] = m_Colorscheme.Colors_ImGuiCol_ScrollbarGrabActive;
  style.Colors[ImGuiCol_CheckMark] = m_Colorscheme.Colors_ImGuiCol_CheckMark;
  style.Colors[ImGuiCol_SliderGrab] = m_Colorscheme.Colors_ImGuiCol_SliderGrab;
  style.Colors[ImGuiCol_SliderGrabActive] = m_Colorscheme.Colors_ImGuiCol_SliderGrabActive;
  style.Colors[ImGuiCol_Button] = m_Colorscheme.Colors_ImGuiCol_Button;
  style.Colors[ImGuiCol_ButtonHovered] = m_Colorscheme.Colors_ImGuiCol_ButtonHovered;
  style.Colors[ImGuiCol_ButtonActive] = m_Colorscheme.Colors_ImGuiCol_ButtonActive;
  style.Colors[ImGuiCol_Header] = m_Colorscheme.Colors_ImGuiCol_Header;
  style.Colors[ImGuiCol_HeaderHovered] = m_Colorscheme.Colors_ImGuiCol_HeaderHovered;
  style.Colors[ImGuiCol_HeaderActive] = m_Colorscheme.Colors_ImGuiCol_HeaderActive;

  style.Colors[ImGuiCol_ResizeGrip] = m_Colorscheme.Colors_ImGuiCol_ResizeGrip;

  style.Colors[ImGuiCol_ResizeGripHovered] = m_Colorscheme.Colors_ImGuiCol_ResizeGripHovered;
  style.Colors[ImGuiCol_ResizeGripActive] = m_Colorscheme.Colors_ImGuiCol_ResizeGripActive;
  style.Colors[ImGuiCol_PlotLines] = m_Colorscheme.Colors_ImGuiCol_PlotLines;
  style.Colors[ImGuiCol_PlotLinesHovered] = m_Colorscheme.Colors_ImGuiCol_PlotHistogramHovered;
  style.Colors[ImGuiCol_PlotHistogram] = m_Colorscheme.Colors_ImGuiCol_PlotHistogram;
  style.Colors[ImGuiCol_PlotHistogramHovered] = m_Colorscheme.Colors_ImGuiCol_PlotHistogramHovered;
  style.Colors[ImGuiCol_TextSelectedBg] = m_Colorscheme.Colors_ImGuiCol_TextSelectedBg;
}

void Interface::RenderDebugger()
{
  Locale locale = m_Application->GetLocale();
  Scene* scene = m_Application->GetScene();

  const String debugLabel = String(ICON_FA_LAPTOP " " + locale.Translate("Window.Debug.Dev.Label"));
  Begin(debugLabel.c_str());
  if (Button(locale.Translate("Debug.Dev.Reset.Cam.Pos.Btn").c_str())) {
    m_Application->GetCamera()->SetPosition(Vector3(0));
  }

  SameLine();

  if (Button(locale.Translate("Debug.Dev.Detach.Player.Cam.Btn").c_str())) {
    m_Application->DetachPlayerCamera();
  }

  SameLine();

  if (Button(locale.Translate("Debug.Dev.Attach.Scene.Player.Cam.Btn").c_str())) {
    m_Application->AttachPlayerCamera(scene->GetPlayerCamera());
  }

  Separator();

  if (scene != YEAGER_NULLPTR) {
    Text("%s %u", locale.Translate("Debug.Dev.Toolboxes.Loaded.Txt").c_str(), scene->GetToolboxs()->size());
    Text("%s %u", locale.Translate("Debug.Dev.Objs.Scene.Txt").c_str(), scene->GetObjects()->size());
    Text("%s %u", locale.Translate("Debug.Dev.Anim.Objs.Scene.Txt").c_str(), scene->GetAnimatedObject()->size());
  }

  Separator();

  Text("%s %u", locale.Translate("Debug.Dev.Frames.Txt").c_str(), m_Frames);
  if (m_Application->GetMode() == ApplicationMode::eAPPLICATION_EDITOR) {
    /* Prevent the calculation of dividing itself by 0 */
    Text("%s : %u", locale.Translate("Debug.Dev.Time.Elapsed.Since.Start.Txt").c_str(),
         (unsigned int)m_Application->GetSecondsElapsedSinceStart());
    if ((unsigned int)m_Application->GetSecondsElapsedSinceStart() > 0) {
      Uint fps = m_Application->GetFrameCurrentCount() / (unsigned int)m_Application->GetSecondsElapsedSinceStart();
      Text("FPS: %u", fps);
    }
  }

  Separator();

  Text("%s: %s", locale.Translate("Debug.Dev.Cam.Should.Move.Txt").c_str(),
       m_Application->GetCamera()->GetShouldMove() ? locale.Translate("Common.Boolean.True.Txt").c_str()
                                                   : locale.Translate("Common.Boolean.False.Txt").c_str());
  Vector3 cameraPos = m_Application->GetCamera()->GetPosition();
  Text("%s: x: %f y: %f z: %f", locale.Translate("Debug.Dev.Cam.World.Pos.Txt").c_str(), cameraPos.x, cameraPos.y,
       cameraPos.z);
  Checkbox(locale.Translate("Debug.Dev.Wnd.Dont.Move.Txt").c_str(), &m_Control.DontMoveWindowsEditor);

  Separator();

  /* Display the vector of the direction of the camera */
  if (m_Application->GetMode() == ApplicationMode::eAPPLICATION_EDITOR) {
    const Vector3 cameraDirection = m_Application->GetCamera()->GetFront();
    const Vector3 rDirection = Vector3(lround(cameraDirection.x), lround(cameraDirection.y), lround(cameraDirection.z));
    Text("%s: x: %i, y: %i, z: %i", locale.Translate("Debug.Dev.Cam.Direction.Txt").c_str(), rDirection.x, rDirection.y,
         rDirection.z);
    Text("%s: x: %.4f, y: %.4f, z: %.4f", locale.Translate("Debug.Dev.Cam.Direction.Float.Txt").c_str(),
         cameraDirection.x, cameraDirection.y, cameraDirection.z);
    InputVector3(locale.Translate("Debug.Dev.Forced.Change.Cam.Direction.Txt").c_str(),
                 m_Application->GetCamera()->GetFrontPtr());
  }

  Separator();

  const String throwConsoleErrStr =
      String(ICON_FA_EXCLAMATION " " + locale.Translate("Debug.Dev.Throw.Console.Err.Btn"));
  if (Button(throwConsoleErrStr.c_str())) {
    m_Application->GetAudioFromEngine()->PlaySound("Click_01");
    Yeager::Log(ERROR, "Test Error");
  }
  SameLine();

  const String throwConsoleWarnStr =
      String(ICON_FA_QUESTION " " + locale.Translate("Debug.Dev.Throw.Console.Warn.Btn"));
  if (Button(throwConsoleWarnStr.c_str())) {
    Yeager::Log(WARNING, "Test Warning");
  }

  Separator();
  const String polygonModePointStr =
      String(ICON_FA_ARROW_POINTER " " + locale.Translate("Debug.Dev.Polygon.Mode.Point.Btn"));
  if (Button(polygonModePointStr.c_str())) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
  }
  SameLine();

  const String polygonModeLinesStr =
      String(ICON_FA_ARROW_ROTATE_LEFT " " + locale.Translate("Debug.Dev.Polygon.Mode.Lines.Btn"));
  if (Button(polygonModeLinesStr.c_str())) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }

  SameLine();

  const String polygonModeFillStr =
      String(ICON_FA_ARROW_ROTATE_LEFT " " + locale.Translate("Debug.Dev.Polygon.Mode.Lines.Btn"));
  if (Button(polygonModeFillStr.c_str())) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  Separator();

  InputVector3(locale.Translate("Debug.Dev.OpenGL.Clear.Color.Txt").c_str(), &m_OpenGLDebugClearScreenColor);

  WindowInfo* wnd = m_Application->GetWindow()->GetWindowInformationPtr();
  Text("%s: x: %f y: %f", locale.Translate("Debug.Dev.Wnd.Editor.Size.Txt").c_str(), wnd->EditorSize.x,
       wnd->EditorSize.y);
  Text("%s: x: %f y: %f", locale.Translate("Debug.Dev.Wnd.Launcher.Size.Txt").c_str(), wnd->LauncherSize.x,
       wnd->LauncherSize.y);
  Text("%s: x: %f y: %f", locale.Translate("Debug.Dev.Wnd.Framebuffer.Size.Txt").c_str(), wnd->FrameBufferSize.x,
       wnd->FrameBufferSize.y);

  if (CollapsingHeader(locale.Translate("Debug.Dev.Engine.Sounds.Test.Txt").c_str())) {
    for (const auto& sound : *m_Application->GetAudioFromEngine()->GetSounds()) {
      if (Button(sound.Name.c_str())) {
        m_Application->GetAudioFromEngine()->PlaySound(sound.Name);
      }
    }
  }

  if (CollapsingHeader(locale.Translate("Debug.Dev.Scene.File.Content.Txt").c_str())) {
    Text(m_SceneFileText.c_str());
  }

  End();
}

void Interface::ScreenShotWindow()
{
  if (m_ReadyToScreenShot) {
    PrepareAndMakeScreenShot();
  }

  m_Application->GetInput()->SetCursorCanDisappear(false);
  m_Application->GetCamera()->SetShouldMove(false);

  m_ScreenShotWindow.Begin(YEAGER_WINDOW_STATIC);

  InputText("Name", &m_NewScreenShootName);
  if (m_NewScreenShootName.empty()) {
    TextColored(IMGUI_RED_ERROR_COLOR, "Image must have a name!");
  }
  InputInt2("Position", m_ScreenShotPosition);
  InputInt2("Size", m_ScreenShotSize);

  if (Button("Fullscreen ScreenShot")) {
    if (!m_NewScreenShootName.empty()) {
      m_ScreenShotMode = ScreenShotMode::EFullScreen;
      m_ReadyToScreenShot = true;
      Yeager::EngineEditorWindowShouldVanish = true;
    }
  }

  if (Button("Middle 800x800 ScreenShot")) {
    if (!m_NewScreenShootName.empty()) {
      m_ScreenShotMode = ScreenShotMode::EMiddleFixedSized;
      m_ReadyToScreenShot = true;
      Yeager::EngineEditorWindowShouldVanish = true;
    }
  }

  if (Button("Custom sized and position Screenshot")) {
    if (!m_NewScreenShootName.empty()) {
      m_ScreenShotMode = ScreenShotMode::ECustomSizedAndPosition;
      m_ReadyToScreenShot = true;
      Yeager::EngineEditorWindowShouldVanish = true;
    }
  }

  m_ScreenShotWindow.End();
}
void Interface::PrepareAndMakeScreenShot()
{

  /**
   * TODO: Change the path of the screenshot to somewhere the user want to save it !!!
   * Crash can happens during runtime of the installed binarie!
   */

  String output = GetPathFromSourceCode("/Configuration/") + m_NewScreenShootName + ".jpg";
  ProcessedImageInfo image;
  switch (m_ScreenShotMode) {
    case ScreenShotMode::ECustomSizedAndPosition:
      image =
          MakeScreenShotInPosition(m_Application, output, UVector2(m_ScreenShotPosition[0], m_ScreenShotPosition[1]),
                                   UVector2(m_ScreenShotSize[0], m_ScreenShotSize[1]));
      break;
    case ScreenShotMode::EFullScreen:
      image = MakeScreenShot(m_Application, output);
      break;
    case ScreenShotMode::EMiddleFixedSized:
      image = MakeScreenShotMiddle(m_Application, output, UVector2(800));
      break;
  }
  Yeager::LogDebug(INFO, "Screenshot {}, Size x: {} y: {}, Path: {}", image.bSucceeded ? "success" : "fail",
                   image.mWidth, image.mHeight, image.mPathWrittenTo);
  m_Application->GetInput()->SetCursorCanDisappear(true);
  m_Application->GetCamera()->SetShouldMove(true);
  Yeager::EngineEditorWindowShouldVanish = false;
  m_ReadyToScreenShot = false;
  m_MakeScreenShotWindowShouldAppear = false;
}

void Interface::DebugTimeInterval()
{
  Begin("Time Intervals", NULL, YEAGER_WINDOW_MOVEABLE);

  int x = 0;

  for (const auto& it : *IntervalElapsedTimeManager::GetIntervals()) {
    Text("%d. %s : %d microseconds", ++x, it.mProcessName.c_str(), it.mDiff.count());
  }

  End();
}

void Interface::PhysXHandleControlWindow()
{
  Begin(ICON_FA_DATABASE " PhysX Debug", NULL, YEAGER_WINDOW_MOVEABLE);

  PhysXHandle* handle = m_Application->GetPhysXHandle();
  Uint actorNum =
      handle->GetPxScene()->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC);
  Text("Count of actors in PhysX Scene %d", actorNum);
  Text("Count of actors loaded in Yeager Engine %d", handle->GetActorsHandle()->size());

  Separator();

  std::vector<physx::PxRigidActor*> actors(actorNum);
  handle->GetPxScene()->getActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC,
                                  reinterpret_cast<physx::PxActor**>(&actors[0]), actorNum);

  for (const auto& actor : actors) {

    switch (actor->getType()) {
      case physx::PxActorType::eRIGID_DYNAMIC: {
        physx::PxRigidDynamic* dynamic = (physx::PxRigidDynamic*)actor;
        Text(ICON_FA_CUBE " Dynamic Actor");
        Text(ICON_FA_ARROW_LEFT " Linear Velocity  %.2f, %.2f, %.2f", dynamic->getLinearVelocity().x,
             dynamic->getLinearVelocity().y, dynamic->getLinearVelocity().z);

        break;
      }
      case physx::PxActorType::eRIGID_STATIC:
        Text(ICON_FA_CUBE " Static Actor");
        break;
    }

    Vector3 pos = PxVec3ToVector3(actor->getGlobalPose().p);
    Text(ICON_FA_COMPASS " Position %.2f, %.2f, %.2f", pos.x, pos.y, pos.z);
    handle->GetGeometryHandle()->RenderShapeInformation(actor);

    Separator();
  }
  End();
}
