#include "Interface.h"
#include "../../Application.h"
#include "../../Common/Common.h"
#include "../../InputHandle.h"
#include "../Editor/Explorer.h"
using namespace ImGui;

unsigned int Interface::m_Frames = 0;

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

Interface::Interface(Window* window, Yeager::ApplicationCore* app) : m_Application(app)
{

  LaunchImGui(window);

  Yeager::Log(INFO, "Initialize ImGui");
  Yeager::Log(INFO, "{}", GetPath("/Assets/fonts/big_blue_term.ttf").c_str());

  ImGuiIO& m_imgui_io = GetIO();

  ImFontConfig config;
  m_imgui_io.Fonts->AddFontDefault();
  config.MergeMode = true;
  config.GlyphMinAdvanceX = 13.0f;
  static const ImWchar icon_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};

  m_imgui_io.Fonts->AddFontFromFileTTF(GetPath("/Assets/fonts/im_writing.ttf").c_str(), m_Fonts.PixelSize, &config,
                                       icon_ranges);
  m_imgui_io.Fonts->AddFontFromFileTTF(GetPath("/Assets/fonts/im_writing.ttf").c_str(), m_Fonts.PixelSize, nullptr,
                                       m_imgui_io.Fonts->GetGlyphRangesDefault());
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
}

void Interface::CreateSpaceX(unsigned int count)
{
  if (count > 0) {
    for (unsigned int x = 0; x < count; x++) {
      NewLine();
    }
  }
}

void Interface::RenderUI()
{
  m_Frames++;

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  NewFrame();

  switch (m_Application->GetMode()) {
    case Yeager::AppLauncher:
      RenderLauncher();
      DisplayWarningWindow();
      break;
    case Yeager::AppEditor:
      RenderEditor();
      DisplayWarningWindow();
      break;
    default:
      RenderError();
  }

  if (m_Control.ExitProgramWindowIsOpen) {
    m_Control.UserExitProgram = WindowExitProgram();
  }

  Render();
  ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
}

void Interface::RenderAwait() {}

void Interface::DrawConsole()
{
  m_ConsoleWindow.Begin(m_Control.DontMoveWindowsEditor ? kWindowStatic : kWindowMoveable);
  //Begin(ICON_FA_TERMINAL " Console", NULL, m_Control.DontMoveWindowsEditor ? kWindowStatic : kWindowMoveable);

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

  m_ConsoleWindow.End();
}

void Interface::DrawExplorer()
{
  m_ExplorerWindow.Begin(m_Control.DontMoveWindowsEditor ? kWindowStatic : kWindowMoveable);
  //Begin(ICON_FA_ADDRESS_BOOK " Explorer", NULL, m_Control.DontMoveWindowsEditor ? kWindowStatic : kWindowMoveable);

  m_Application->GetExplorer()->DrawExplorer();

  m_ExplorerWindow.End();
}

void Interface::DrawToolbox()
{
  m_ToolboxWindow.Begin(m_Control.DontMoveWindowsEditor ? kWindowStatic : kWindowMoveable);
  //Begin(ICON_FA_GEAR " Toolbox", NULL, m_Control.DontMoveWindowsEditor ? kWindowStatic : kWindowMoveable);

  if (m_Application->GetExplorer()->toolbox_selected != nullptr) {
    Yeager::ToolBoxObject* obj = m_Application->GetExplorer()->toolbox_selected;
    YgCchar text = obj->GetEntity()->GetName().c_str();
    Text(text);
    obj->DrawObject();
  }

  m_ToolboxWindow.End();
}

void Interface::DrawEditorMenu()
{
  if (ImGui::BeginMainMenuBar()) {
    Yeager::EngineEditorMenuBar.MenuBarSize = GetWindowSize();
    if (ImGui::BeginMenu(ICON_FA_FILE_IMPORT " File")) {

      if (ImGui::MenuItem("Create")) {}
      if (ImGui::MenuItem("Open", "Ctrl+O")) {}
      if (ImGui::MenuItem("Save", "Ctrl+S")) {}
      if (ImGui::MenuItem("Save as..")) {}
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
}

void Interface::RenderEditor()
{
  DrawExplorer();
  DrawToolbox();
  RenderDebugger();
  DrawEditorMenu();
  DrawConsole();
}

void Interface::RenderLauncher()
{

  InterfaceButton open_project("interface_open_project", ICON_FA_FOLDER_OPEN " Open Project");
  InterfaceButton new_project("interface_new_project", ICON_FA_FOLDER " New Project");
  InterfaceButton settings_button("interface_settings_button", ICON_FA_GEARS " Settings");
  InterfaceButton help_button("interface_help_button", ICON_FA_CIRCLE_QUESTION " Help");

  SetNextWindowPos(ImVec2(0, 0));
  SetNextWindowSize(ImVec2(ygWindowWidth, ygWindowHeight));
  Begin("Yeager Launcher", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

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
    logButton(open_project);
    m_Application->SetMode(Yeager::AppEditor);
  }

  SameLine();

  if (new_project.AddButton()) {
    logButton(new_project);
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
  m_DebuggerWindow.Begin();

  Text("Frames %u", m_Frames);
  Text("Camera should move: %s", m_Application->GetCamera()->GetShouldMove() ? "true" : "false");
  YgVector3 cameraPos = m_Application->GetCamera()->GetPosition();
  Text("Camera world position: x: %f y: %f z: %f", cameraPos.x, cameraPos.y, cameraPos.z);
  Checkbox("Windows dont move", &m_Control.DontMoveWindowsEditor);
  unsigned int memory_usage_mb = Yeager::s_MemoryManagement.GetMemortUsage() / 1000000;
  unsigned int memory_usage_by = Yeager::s_MemoryManagement.GetMemortUsage() % 1000000;
  Text("Memory Usage in MB: %u.%u", memory_usage_mb, memory_usage_by);
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
  m_DebuggerWindow.End();
}