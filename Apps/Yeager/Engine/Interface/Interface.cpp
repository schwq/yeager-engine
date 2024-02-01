#include "Interface.h"
#include "../../Application.h"
#include "../../Common/Common.h"
#include "../../InputHandle.h"
#include "../../Launcher.h"
#include "../../Serialization.h"
#include "../Editor/Explorer.h"

using namespace ImGui;
using namespace Yeager;

unsigned int Yeager::Interface::m_Frames = 0;

void Yeager::InputVector3(const char* label, YgVector3* v, const char* format, ImGuiInputTextFlags flags)
{
  float* ver[3] = {&v->x, &v->y, &v->z};
  InputFloat3(label, *ver, format, flags);
}

void Yeager::DisplayDirectionalLightControl(Yeager::PhysicalLightHandle* source)
{
  InputVector3("Directions##Directional", &source->GetDirectionalLight()->Direction);
  InputVector3("Ambients##Directional", &source->GetDirectionalLight()->Ambient);
  InputVector3("Diffuses##Directional", &source->GetDirectionalLight()->Diffuse);
  InputVector3("Speculars##Directional", &source->GetDirectionalLight()->Specular);
  InputVector3("Colors##Directional", &source->GetDirectionalLight()->Color);
}

void Yeager::DisplaySpotLightControl(Yeager::PhysicalLightHandle* source)
{
  InputVector3("Position##SpotLight", &source->GetSpotLight()->Position);
  InputVector3("Direction##SpotLight", &source->GetSpotLight()->Direction);
  InputFloat("CutOff##SpotLight", &source->GetSpotLight()->CutOff);
  InputFloat("OuterCutOff##SpotLight", &source->GetSpotLight()->OuterCutOff);
  InputFloat("Constant##SpotLight", &source->GetSpotLight()->Constant);
  InputFloat("Linear##SpotLight", &source->GetSpotLight()->Linear);
  InputFloat("Quadratic##SpotLight", &source->GetSpotLight()->Quadratic);
  InputVector3("Ambient##SpotLight", &source->GetSpotLight()->Ambient);
  InputVector3("Diffuse##SpotLight", &source->GetSpotLight()->Diffuse);
  InputVector3("Specular##SpotLight", &source->GetSpotLight()->Specular);
  Checkbox("Active##SpotLight", &source->GetSpotLight()->Active);
}

void Yeager::DisplayPointLightControl(Yeager::ObjectPointLight* source)
{
  InputVector3("Position##PointLight", &source->ObjSource->GetTransformationPtr()->position);
  InputVector3("Scale##PointLight", &source->ObjSource->GetTransformationPtr()->scale);
  InputVector3("Rotation##PointLight", &source->ObjSource->GetTransformationPtr()->rotation);
  InputFloat("Constant##PointLight", &source->Constant);
  InputFloat("Linear##PointLight", &source->Linear);
  InputFloat("Quadratic##PointLight", &source->Quadratic);
  InputVector3("Ambient##PointLight", &source->Ambient);
  InputVector3("Specular##PointLight", &source->Specular);
  InputVector3("Diffuse##PointLight", &source->Diffuse);
  InputVector3("Color##PointLight", &source->Color);
  Checkbox("Active##PointLight", &source->Active);
}

void Interface::LightHandleControlWindow()
{
  /**
   * NOTE Imgui button`s label problem explained where: https://github.com/ocornut/imgui/issues/74
   */

  Begin(ICON_FA_LIGHTBULB " Light Control");
  int label_id = 0;
  for (auto& lightsources : *m_Application->GetScene()->GetLightSources()) {
    PushID(label_id++);
    if (Button("Add Light Source")) {
      ObjectPointLight light;
      Transformation trans;
      trans.position = m_Application->GetCamera()->GetPosition() + m_Application->GetCamera()->GetDirection();
      trans.scale = YgVector3(0.1f);
      lightsources->AddObjectPointLight(light, trans);
    }

    SameLine();
    if (Button("Delete")) {
      lightsources->SetScheduleDeletion(true);
    }

    if (CollapsingHeader("Linked Shaders")) {
      for (const auto& shader : *lightsources->GetLinkedShaders()) {
        Text("%s", shader->GetName().c_str());
      }
    }

    if (TreeNode("Directional Light")) {
      PushID(label_id++);
      DisplayDirectionalLightControl(lightsources.get());
      PopID();
      TreePop();
    }

    if (TreeNode("Spot Light")) {
      PushID(label_id++);
      DisplaySpotLightControl(lightsources.get());
      PopID();
      TreePop();
    }

    if (TreeNode("Point Lights")) {
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
  Yeager::Log(INFO, "Destrorying Interface!");
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  DestroyContext();
  // This function termination was placed in the Window.h file, but it was causing a error, because interface class that managers imgui and the window class
  // that manage the glfw window are smart pointers, and in the wrong calling sequence, the glfw windows was destroryed before imgui can terminate properly
  glfwTerminate();
}

bool Interface::WindowExitProgram()
{
  m_Application->GetInput()->SetCursorCanDisappear(false);
  m_Application->GetCamera()->SetShouldMove(false);
  CenteredWindow(350, 140);
  Begin("Exit Program?", NULL, kWindowStatic);
  CenteredText("Do you really want to exit the program?");
  CenteredText(" Unsaved work wont been load next time!");
  if (Button("Exit")) {
    End();
    m_Control.ExitProgramWindowIsOpen = false;
    return true;
  }
  if (Button("Cancel")) {
    m_Control.ExitProgramWindowIsOpen = false;
    m_Application->GetInput()->SetCursorCanDisappear(true);
    m_Application->GetCamera()->SetShouldMove(true);
  }
  End();
  return false;
}

void Interface::CenteredWindow(unsigned int size_x, unsigned int size_y)
{
  SetNextWindowPos(ImVec2((ygWindowWidth / 2) - (size_x / 2), (ygWindowHeight / 2) - (size_y / 2)));
  SetNextWindowSize(ImVec2(size_x, size_y));
}

bool InterfaceButton::CenteredButton()
{
  auto windowWidth = GetWindowSize().x;
  auto textWidth = CalcTextSize(Text.c_str()).x;
  SetCursorPosX((windowWidth - textWidth) * 0.5f);
  return Button(Text.c_str());
}

void Interface::NewProjectWindow()
{

  Yeager::SceneContext context;

  CenteredWindow(500, 200);
  Begin("New Project", NULL, kWindowStatic);
  InputText("Project Name", &context.m_name);
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

void Interface::AddWarningWindow(const YgString& warning, unsigned int size_x, unsigned int size_y)
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

InterfaceImage::InterfaceImage(YgCchar path)
{
  bool ret = LoadTextureFromFile(path, &ImageTexture, &ImageWidth, &ImageHeight);
  IM_ASSERT(ret);
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

void InterfaceImage::LoadObjectTextureImage(Yeager::Texture2D* texture)
{
  Image((void*)(intptr_t)texture->GetID(), ImVec2(texture->GetWidth() / 2, texture->GetHeight() / 2));
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

  ImGuiIO& m_imgui_io = GetIO();
  (void)m_imgui_io;
  m_imgui_io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  m_imgui_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  m_imgui_io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Keyboard Controls

  StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window->getWindow(), true);
  ImGui_ImplOpenGL3_Init("#version 330");
}

void Interface::RequestRestartInterface(Window* window)
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();

  ImGui_ImplGlfw_InitForOpenGL(window->getWindow(), true);
  ImGui_ImplOpenGL3_Init("#version 330");
}

Interface::Interface(Window* window, Yeager::ApplicationCore* app) : m_Application(app)
{

  LaunchImGui(window);

  Yeager::Log(INFO, "Initialize ImGui");

  ImGuiIO& m_imgui_io = GetIO();

  ImFontConfig config;
  m_imgui_io.Fonts->AddFontDefault();
  config.MergeMode = true;
  config.GlyphMinAdvanceX = 13.0f;
  static const ImWchar icon_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};

  m_imgui_io.Fonts->AddFontFromFileTTF(GetPath("/Assets/Fonts/IosevkaNerdFont-Medium.ttf").c_str(), m_Fonts.PixelSize,
                                       &config, icon_ranges);

  m_imgui_io.Fonts->Build();
  LoadColorscheme();

  m_Control.Initialize = true;
  Yeager::EngineEditorMenuBar.HasMenuBar = true;

  m_ToolboxWindow =
      Yeager::InterfaceWindow("Toolbox", ImVec2(450, 600), ImVec2(0, 0), true, Yeager::WindowRelativePos::LEFT_ABOVE);
  m_ConsoleWindow = Yeager::InterfaceWindow("Console", ImVec2(1000, 300), &m_ExplorerWindow, ImVec2(0, 100),
                                            Yeager::WindowRelativePos::RIGHT);
  m_ExplorerWindow = Yeager::InterfaceWindow("Explorer", ImVec2(450, 450), &m_ToolboxWindow, ImVec2(0, 0),
                                             Yeager::WindowRelativePos::UNDER);
  m_DebuggerWindow =
      Yeager::InterfaceWindow("Debugger", ImVec2(400, 300), ImVec2(0, 0), true, Yeager::WindowRelativePos::RIGHT_UNDER);
  m_ExplorerWindow.SetRule(Yeager::EWindowFollowUnder, true);
  m_ConsoleWindow.SetRule(Yeager::EWindowFollowUnder, true);

  m_ScreenShotWindow =
      Yeager::InterfaceWindow("ScreenShot", ImVec2(400, 400), ImVec2(0, 0), true, Yeager::WindowRelativePos::MIDDLE);
}

void Interface::CreateSpaceX(unsigned int count)
{
  if (count > 0) {
    for (unsigned int x = 0; x < count; x++) {
      NewLine();
    }
  }
}

void Interface::InitRenderFrame()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  NewFrame();
}

void Interface::TerminateRenderFrame()
{
  Render();
  ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
}

void Interface::RenderUI(Yeager::Launcher* launcher)
{
  m_Frames++;

  switch (m_Application->GetMode()) {
    case YgApplicationMode::eAPPLICATION_LAUNCHER:
      RenderLauncher(launcher);
      DisplayWarningWindow();
      break;
    case YgApplicationMode::eAPPLICATION_EDITOR:
      if (m_MakeScreenShotWindowShouldAppear) {
        ScreenShotWindow();
      }
      RenderEditor();
      DisplayWarningWindow();
      break;
    default:
      RenderError();
  }

  if (m_Control.ExitProgramWindowIsOpen) {
    m_Control.UserExitProgram = WindowExitProgram();
  }
}

void Interface::RenderAwait() {}

void Interface::DrawConsole()
{
  //m_ConsoleWindow.Begin(m_Control.DontMoveWindowsEditor ? kWindowStatic : kWindowMoveable);
  Begin(ICON_FA_TERMINAL " Console");

  kConsole.ReadLog();

  if (Button("Add Comment:")) {
    m_Control.CommentWindowIsOpen = true;
  }
  if (m_Control.CommentWindowIsOpen) {
    CenteredWindow(400, 50);
    Begin("Comment");
    InputText("Say: ", &m_Comment);
    SameLine();
    m_Application->GetCamera()->SetShouldMove(false);
    m_Application->GetInput()->SetCursorCanDisappear(false);

    if (Button("ADD")) {
      m_Control.CommentWindowIsOpen = false;
      Yeager::Log(INFO, "{}", m_Comment.c_str());
      m_Comment.clear();
      m_Application->GetCamera()->SetShouldMove(true);
      m_Application->GetInput()->SetCursorCanDisappear(true);
    }
    End();
  }

  End();
}

void Interface::DrawExplorer()
{
  //m_ExplorerWindow.Begin(m_Control.DontMoveWindowsEditor ? kWindowStatic : kWindowMoveable);
  Begin(ICON_FA_ADDRESS_BOOK " Explorer");

  m_Application->GetExplorer()->DrawExplorer();

  End();
}

void Interface::DrawToolbox()
{
  //m_ToolboxWindow.Begin(m_Control.DontMoveWindowsEditor ? kWindowStatic : kWindowMoveable);
  Begin(ICON_FA_GEAR " Toolbox");

  // TODO clean this mess
  if (m_Application->GetExplorer()->GetSelectedToolbox() != YEAGER_NULLPTR &&
      m_Application->GetExplorer()->GetSelectedToolbox()->IsValid() &&
      m_Application->GetExplorer()->GetSelectedToolbox()->GetEntity() != YEAGER_NULLPTR &&
      m_Application->GetExplorer()->GetSelectedToolbox()->GetEntity()->IsValid()) {
    Yeager::ToolBoxObject* obj = m_Application->GetExplorer()->GetSelectedToolbox();
    Text("%s", obj->GetEntity()->GetName().c_str());
    obj->DrawObject();
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

    PhysXHandleControlWindow();

    LightHandleControlWindow();
  }
}

bool Interface::RenderLauncher(Yeager::Launcher* launcher)
{

  InterfaceButton open_project("interface_open_project", ICON_FA_FOLDER_OPEN " Open Project");
  InterfaceButton new_project("interface_new_project", ICON_FA_FOLDER " New Project");
  InterfaceButton settings_button("interface_settings_button", ICON_FA_GEARS " Settings");
  InterfaceButton help_button("interface_help_button", ICON_FA_CIRCLE_QUESTION " Help");
  Yeager::InterfaceWindow open_project_window("Open Project", ImVec2(1000, 400),
                                              ImVec2(ygWindowWidth / 2, ygWindowHeight / 2), true,
                                              Yeager::WindowRelativePos::MIDDLE);
  Yeager::InterfaceWindow new_project_window("Create New Project", ImVec2(1000, 400),
                                             ImVec2(ygWindowWidth / 2, ygWindowHeight / 2), true,
                                             Yeager::WindowRelativePos::MIDDLE);

  SetNextWindowPos(ImVec2(0, 0));
  SetNextWindowSize(ImVec2(ygWindowWidth, ygWindowHeight));
  Begin("Yeager Launcher", NULL,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoTitleBar);

  CenteredText("Welcome to the Yeager Engine!");
  CenteredText("This is a small project, being build by one developer with the propose");
  CenteredText("of being a great render / game engine program.");
  CenteredText(
      "This engine is open source and free, check out the github "
      "repository:");
  CenteredText("https://github.com/schwq/yeager-engine");

  CreateSpaceX(2);

  ImGuiStyle& m_imgui_style = GetStyle();

  float width = 0.0f;
  width += CalcTextSize(open_project.Text.c_str()).x;
  width += m_imgui_style.ItemSpacing.x;
  width += CalcTextSize(new_project.Text.c_str()).x;
  width += m_imgui_style.ItemSpacing.x;
  width += CalcTextSize(settings_button.Text.c_str()).x;
  width += m_imgui_style.ItemSpacing.x;
  width += CalcTextSize(help_button.Text.c_str()).x;
  AlignForWidth(width);

  if (open_project.AddButton()) {
    m_OpenProjectWindowOpen = true;
#ifdef YEAGER_SYSTEM_LINUX
    m_OpenProjectToDisplay =
        Yeager::ReadProjectsToDisplay(GetLinuxHomeDirectory() + "/.YeagerEngine/External/LoadedProjectsPath.yml");
#elif defined(YEAGER_SYSTEM_WINDOWS_x64)
    m_OpenProjectToDisplay =
        Yeager::ReadProjectsToDisplay(GetExternalFolderPath() + "\\YeagerEngine\\External\\LoadedProjectsPath.yml");
#endif
  }

  if (m_OpenProjectWindowOpen && !m_NewProjectWindowOpen) {
    open_project_window.Begin(kWindowStatic);

    if (Button("Cancel")) {
      m_OpenProjectWindowOpen = false;
    }

    if (m_OpenProjectToDisplay.empty()) {
      Text("No projects :(");
    }
    for (const auto& project : m_OpenProjectToDisplay) {
      Text("%s", project.Author.c_str());
      Text("%s", project.Name.c_str());
      Text("%s", project.RendererType.c_str());
      Text("%s", project.SceneType.c_str());
      Text("%s", project.FolderPath.c_str());
      Text("%s", project.Path.c_str());
      Text("%s", CurrentTimeFormatToString().c_str());
      YgString labelWithID = "Open##" + project.Name;
      if (Button(labelWithID.c_str())) {
        launcher->SetUserHasSelect(true);
        launcher->GetCurrentProjectPicked()->m_Name = project.Name;
        launcher->GetCurrentProjectPicked()->m_SceneRenderer = StringToSceneRenderer(project.RendererType);
        launcher->GetCurrentProjectPicked()->m_SceneType = StringToSceneType(project.SceneType);
        launcher->GetCurrentProjectPicked()->m_ProjectFolderPath = project.FolderPath;
        launcher->GetCurrentProjectPicked()->m_ProjectConfigurationPath = project.Path;
      }
    }
    open_project_window.End();
  }

  SameLine();

  if (new_project.AddButton()) {
    m_NewProjectWindowOpen = true;
    m_NewProjectHandle = new Yeager::LauncherProjectPicker();
    /*
    for (const auto& entry : std::filesystem::directory_iterator(GetPath("/Configuration/Scenes"))) {
      // Removes the extension of the file name, like (file.yaml) becomes (file.)
      YgString str = RemoveSuffixUntilCharacter(entry.path().filename().string(), '.');
      // Removes the point at reimans at the end of the file name (file.) becomes (file)
      str.erase(str.length() - 1);
      m_ProjectsNamesAlreadyTaken.push_back(str);
    }
    */
  }

  if (m_NewProjectWindowOpen && !m_OpenProjectWindowOpen) {

    new_project_window.Begin(kWindowStatic);

    const char* RendererType[] = {"OpenGL3_3", "OpenGL4"};
    const char* SceneType[] = {"Scene2D", "Scene3D"};
    const char* PlataformTarget[] = {"Linux System", "Windows 10/11", "Android SDK", "MacOS", "IOS", "Universal"};

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
      auto selection = pfd::select_folder("Select project folder", GetExternalFolderPath()).result();
      if (!selection.empty()) {
        /* This should always return true, but we are making sure the user or the machine deletes the folder selected during the midtime */
        if (Yeager::ValidatesPath(selection)) {
          m_NewProjectHandle->m_ProjectFolderPath = selection + YG_PS;
        }
      }
    }
    SameLine();
    Text("Project Folder: %s", m_NewProjectHandle->m_ProjectFolderPath.c_str());

    InputText("Author`s Name", &m_NewProjectAuthorName);
    m_NewProjectHandle->m_AuthorName = m_NewProjectAuthorName;

    if (BeginCombo("Renderer Type", m_NewProjectCurrentRenderer.c_str())) {
      for (unsigned int x = 0; x < 2; x++) {
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
      for (unsigned int x = 0; x < 2; x++) {
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
      for (unsigned int x = 0; x < 6; x++) {
        bool is_selected = (m_NewProjectCurrentPlataformTarget.c_str() == PlataformTarget[x]);
        if (Selectable(PlataformTarget[x], is_selected))
          m_NewProjectCurrentPlataformTarget = PlataformTarget[x];
        m_NewProjectHandle->m_PlataformTarget = m_NewProjectCurrentPlataformTarget;
        if (is_selected)
          SetItemDefaultFocus();
      }
      EndCombo();
    }

    if (Button("Create")) {
      if (m_NewProjectIsOkayToCreate) {
        YgTime_t current_time = CurrentTimeToTimeType();
        m_NewProjectHandle->m_ProjectDateOfCreation = current_time.Date;
        m_NewProjectHandle->m_ProjectConfigurationPath =
            m_NewProjectHandle->m_ProjectFolderPath + m_NewProjectHandle->m_Name + ".yml";

        launcher->BuildNewProject(*m_NewProjectHandle);
        launcher->SetUserHasSelect(true);
        launcher->SetNewProjectLoaded(true);
        LoadedProjectHandle handle;
        handle.m_ProjectName = m_NewProjectHandle->m_Name;
        handle.m_ProjectFolderPath = m_NewProjectHandle->m_ProjectFolderPath;
        handle.m_ProjectConfigurationPath = m_NewProjectHandle->m_ProjectConfigurationPath;
        m_Application->GetLoadedProjectsHandles()->push_back(handle);
        delete m_NewProjectHandle;
      }
    }

    SameLine();

    if (Button("Cancel")) {
      delete m_NewProjectHandle;
      m_NewProjectWindowOpen = false;
    }

    new_project_window.End();
  }

  SameLine();

  if (settings_button.AddButton()) {
    logButton(settings_button);
  }

  SameLine();

  if (help_button.AddButton()) {
    logButton(help_button);
  }
  End();

  RenderDebugger();
  return true;
}

void Interface::CenteredText(YgString text)
{
  auto windowWidth = GetWindowSize().x;
  auto textWidth = CalcTextSize(text.c_str()).x;

  SetCursorPosX((windowWidth - textWidth) * 0.5f);
  Text(text.c_str());
}

void Interface::logButton(InterfaceButton button)
{
  Yeager::Log(INFO, "{} button was pressed", button.Name.c_str());
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
  Begin(ICON_FA_LAPTOP " Debug Development");
  if (Button("Reset camera position")) {
    m_Application->GetCamera()->SetPosition(YgVector3(0));
  }

  if (m_Application->GetScene() != YEAGER_NULLPTR) {
    Text("Toolboxes Loaded %u", m_Application->GetScene()->GetToolboxs()->size());
    Text("Objects in Scene %u", m_Application->GetScene()->GetObjects()->size());
    Text("Animated Objects in Scene %u", m_Application->GetScene()->GetAnimatedObject()->size());
    Text("Instanced Objects in Scene %u", m_Application->GetScene()->GetInstancedObjects()->size());
    Text("Instanced Animated Objects in Scene %u", m_Application->GetScene()->GetInstancedAnimatedObjects()->size());
  }

  Text("Frames %u", m_Frames);
  Text("Camera should move: %s", m_Application->GetCamera()->GetShouldMove() ? "true" : "false");
  YgVector3 cameraPos = m_Application->GetCamera()->GetPosition();
  Text("Camera world position: x: %f y: %f z: %f", cameraPos.x, cameraPos.y, cameraPos.z);
  Checkbox("Windows dont move", &m_Control.DontMoveWindowsEditor);
  unsigned int memory_usage_mb = Yeager::s_MemoryManagement.GetMemortUsage() / 1000000;
  unsigned int memory_usage_by = Yeager::s_MemoryManagement.GetMemortUsage() % 1000000;
  Text("Memory Usage in MB: %u.%u", memory_usage_mb, memory_usage_by);
  Text("New calls: %u", Yeager::s_MemoryManagement.m_NumberNewCalls);
  Text("Delete calls %u", Yeager::s_MemoryManagement.m_NumberDeleteCalls);
  Text("New ahead of Delete calls %u",
       Yeager::s_MemoryManagement.m_NumberNewCalls - Yeager::s_MemoryManagement.m_NumberDeleteCalls);
  Text("Memory Usage in Bytes: %u", Yeager::s_MemoryManagement.GetMemortUsage());
  Text("Memort Allocated in Bytes %u", Yeager::s_MemoryManagement.m_MemoryAllocatedSize);
  Text("Memory Freed in Bytes: %u", Yeager::s_MemoryManagement.m_MemoryFreedSize);

  if (Button(ICON_FA_EXCLAMATION " Throw Console Error")) {
    Yeager::Log(ERROR, "Test Error");
  }
  SameLine();
  if (Button(ICON_FA_QUESTION " Throw Console Warning")) {
    Yeager::Log(WARNING, "Test Warning");
  }

  if (Button(ICON_FA_ARROW_POINTER " Polygon mode: POINT")) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
  }
  SameLine();
  if (Button(ICON_FA_ARROW_ROTATE_LEFT " Polygon mode: LINES")) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  if (Button(ICON_FA_ARROW_ROTATE_LEFT " Polygon mode: FILL")) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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

  m_ScreenShotWindow.Begin(kWindowStatic);

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
  YgString output = GetPath("/Configuration/") + m_NewScreenShootName + ".jpg";
  switch (m_ScreenShotMode) {
    case ScreenShotMode::ECustomSizedAndPosition:
      MakeScreenShotInPosition(output.c_str(), m_ScreenShotPosition[0], m_ScreenShotPosition[1], m_ScreenShotSize[0],
                               m_ScreenShotSize[1]);
      break;
    case ScreenShotMode::EFullScreen:
      MakeScreenShot(output.c_str());
      break;
    case ScreenShotMode::EMiddleFixedSized:
      MakeScreenShotMiddle(output.c_str());
      break;
  }
  m_Application->GetInput()->SetCursorCanDisappear(true);
  m_Application->GetCamera()->SetShouldMove(true);
  Yeager::EngineEditorWindowShouldVanish = false;
  m_ReadyToScreenShot = false;
  m_MakeScreenShotWindowShouldAppear = false;
}

void Interface::PhysXHandleControlWindow()
{
  PhysXHandle* handle = m_Application->GetPhysXHandle();
  unsigned int actorNum =
      handle->GetPxScene()->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC);
  Text("Count of actors in PhysX Scene %d", actorNum);
  Text("Count of actors loaded in Yeager Engine %d", handle->GetActorsHandle()->size());
  std::vector<physx::PxRigidActor*> actors(actorNum);
  handle->GetPxScene()->getActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC,
                                  reinterpret_cast<physx::PxActor**>(&actors[0]), actorNum);

  for (const auto& actor : actors) {

    switch (actor->getType()) {
      case physx::PxActorType::eRIGID_DYNAMIC: {
        physx::PxRigidDynamic* dynamic = (physx::PxRigidDynamic*)actor;
        Text("Dynamic Actor");
        Text("Linear Velocity  %.2f, %.2f, %.2f", dynamic->getLinearVelocity().x, dynamic->getLinearVelocity().y,
             dynamic->getLinearVelocity().z);

        break;
      }
      case physx::PxActorType::eRIGID_STATIC:
        Text("Static Actor");
        break;
    }

    YgVector3 pos = PxVec3ToYgVector3(actor->getGlobalPose().p);
    Text("Position %.2f, %.2f, %.2f", pos.x, pos.y, pos.z);
    handle->GetGeometryHandle()->RenderShapeInformation(actor);
  }
}
