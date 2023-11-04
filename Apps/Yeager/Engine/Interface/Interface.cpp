#include "Interface.h"
#include "../../Application.h"
#include "../../Common/Common.h"
#include "../../InputHandle.h"
#include "../Editor/Explorer.h"
using namespace ImGui;

yg_uint Interface::m_frames = 0;

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
  m_app->GetInput()->MakeCursorStaticAppear(true);
  m_app->GetCamera()->SetShouldMove(false);
  CenteredWindow(350, 100);
  Begin("Exit Program?", NULL, kWindowStatic);
  CenteredText("Do you really want to exit the program?");
  CenteredText(" Unsaved work wont been load next time!");
  if (Button("Exit")) {
    End();
    m_exit_program_window_open = false;
    return true;
  }
  if (Button("Cancel")) {
    m_exit_program_window_open = false;
    m_app->GetInput()->MakeCursorStaticAppear(false);
    m_app->GetCamera()->SetShouldMove(true);
  }
  End();
  return false;
}

void Interface::CenteredWindow(yg_uint size_x, yg_uint size_y)
{
  SetNextWindowPos(ImVec2((ygWindowWidth / 2) - (size_x / 2), (ygWindowHeight / 2) - (size_y / 2)));
  SetNextWindowSize(ImVec2(size_x, size_y));
}

bool InterfaceButton::CenteredButton()
{
  auto windowWidth = GetWindowSize().x;
  auto textWidth = CalcTextSize(m_text.c_str()).x;
  SetCursorPosX((windowWidth - textWidth) * 0.5f);
  return Button(m_text.c_str());
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
  if (m_current_warning.active) {
    CenteredWindow(m_current_warning.size_x, m_current_warning.size_y);
    Begin("Warning!");
    CenteredText(m_current_warning.warning);
    if (m_current_warning.first_log_warning) {  // Doesnt loop various warning forever, crashes if so
      Yeager::Log(WARNING, "{}", m_current_warning.warning.c_str());
      m_current_warning.first_log_warning = false;
    }
    Spacing();
    if (Button("Understood")) {  // Reset for next warning window
      m_current_warning.active = false;
      m_current_warning.warning.clear();
      m_current_warning.first_log_warning = true;
    }
    End();
  }
}

void Interface::AddWarningWindow(const yg_string& warning, yg_uint size_x, yg_uint size_y)
{
  m_current_warning.warning = warning;
  m_current_warning.size_x = size_x;
  m_current_warning.size_y = size_y;
  m_current_warning.active = true;
}

bool InterfaceButton::AddButton()
{
  return Button(m_text.c_str());
}

float InterfaceButton::TextWidth()
{
  return CalcTextSize(m_text.c_str()).x;
}

InterfaceImage::InterfaceImage(yg_cchar path)
{
  bool ret = LoadTextureFromFile(path, &m_image_texture, &m_image_width, &m_image_height);
  Yeager::Log(INFO, "{}", path);
  IM_ASSERT(ret);
}

void InterfaceImage::LoadInterfaceImage()
{
  Image((void*)(intptr_t)m_image_texture, ImVec2(m_image_width / 2, m_image_height / 2));
}

void InterfaceImage::LoadInterfaceCenteredImage()
{
  auto windowWidth = GetWindowSize().x;
  SetCursorPosX((windowWidth - m_image_width) * 0.5f);
  Image((void*)(intptr_t)m_image_texture, ImVec2(m_image_width / 2, m_image_height / 2));
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

Interface::Interface(Window* window, Yeager::ApplicationCore* app) : m_app(app)
{

  LaunchImGui(window);

  Yeager::Log(INFO, "Initialize ImGui");
  Yeager::Log(INFO, "{}", GetPath("/Assets/fonts/big_blue_term.ttf").c_str());

  ImGuiIO& m_imgui_io = GetIO();
  m_imgui_io.Fonts->AddFontDefault();
  ImFontConfig config;
  config.MergeMode = true;
  config.GlyphMinAdvanceX = 13.0f;
  static const ImWchar icon_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
  m_imgui_io.Fonts->AddFontFromFileTTF(GetPath("/Assets/fonts/JetBrainsMonoNerdFont-Bold.ttf").c_str(), size_pixels,
                                       &config, icon_ranges);

  LoadColorscheme();
  m_initialize = true;
}

void Interface::CreateSpaceX(yg_uint count)
{
  if (count > 0) {
    for (yg_uint x = 0; x < count; x++) {
      NewLine();
    }
  }
}

void Interface::RenderUI()
{
  m_frames++;

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  NewFrame();

  switch (m_app->GetMode()) {
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

  if (m_exit_program_window_open) {
    m_user_exit_program = WindowExitProgram();
  }

  Render();
  ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
}

void Interface::RenderAwait() {}

void Interface::DrawExplorer()
{
  Begin(ICON_FA_ADDRESS_BOOK " Explorer", NULL, m_dont_move_windows_editor ? kWindowStatic : kWindowMoveable);

  m_app->GetExplorer()->DrawExplorer();

  End();

  Begin(ICON_FA_TERMINAL " Console", NULL, m_dont_move_windows_editor ? kWindowStatic : kWindowMoveable);

  if (Button("Add Comment:")) {
    m_comment_window_open = true;
  }
  if (m_comment_window_open) {
    CenteredWindow(400, 50);
    Begin("Comment");
    InputText("Say: ", &comment);
    SameLine();
    m_app->GetCamera()->SetShouldMove(false);
    m_app->GetInput()->SetCursorCanDisappear(false);

    if (Button("ADD")) {
      m_comment_window_open = false;
      Yeager::Log(INFO, "{}", comment.c_str());
      comment.clear();
      m_app->GetCamera()->SetShouldMove(true);
      m_app->GetInput()->SetCursorCanDisappear(true);
    }
    End();
  }
  kConsole.ReadLog();

  End();
}

void Interface::DrawToolbox()
{
  Begin(ICON_FA_GEAR " Toolbox", NULL,
        m_dont_move_windows_editor ? ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove
                                   : ImGuiWindowFlags_None);

  if (m_app->GetExplorer()->toolbox_selected == nullptr) {
    Text("None");
  } else {
    Yeager::ToolBoxObject* obj = m_app->GetExplorer()->toolbox_selected;
    yg_cchar text = obj->GetEntity()->GetName().c_str();
    Text(text);
    obj->DrawObject();
  }

  End();
}

void Interface::DrawEditorMenu()
{
  InterfaceButton file_button("editor_file_button", "FILE");
  InterfaceButton edit_button("editor_edit_button", "EDIT");
  InterfaceButton help_button("editor_help_button", "HELP");
  InterfaceButton debug_button("editor_debug_button", "DEBUG");

  if (ImGui::BeginMainMenuBar()) {
    m_menu_bar_size = GetWindowSize();
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
  width += CalcTextSize(open_project.m_text.c_str()).x;
  width += m_imgui_style.ItemSpacing.x;
  width += CalcTextSize(new_project.m_text.c_str()).x;
  width += m_imgui_style.ItemSpacing.x;
  width += CalcTextSize(settings_button.m_text.c_str()).x;
  width += m_imgui_style.ItemSpacing.x;
  width += CalcTextSize(help_button.m_text.c_str()).x;
  AlignForWidth(width);

  if (open_project.AddButton()) {
    logButton(open_project);
    m_app->SetMode(Yeager::AppEditor);
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

void Interface::CenteredText(yg_string text)
{
  auto windowWidth = GetWindowSize().x;
  auto textWidth = CalcTextSize(text.c_str()).x;

  SetCursorPosX((windowWidth - textWidth) * 0.5f);
  Text(text.c_str());
}

void Interface::logButton(InterfaceButton button)
{
  Yeager::Log(INFO, "{} button was pressed", button.m_name.c_str());
}
void Interface::LoadColorscheme()
{
  ImGuiStyle& style = ImGui::GetStyle();
  style.WindowRounding = m_colorscheme.WindowRounding;
  style.FrameRounding = m_colorscheme.FrameRounding;
  style.ScrollbarRounding = m_colorscheme.ScrollbarRounding;
  style.Colors[ImGuiCol_Text] = m_colorscheme.Colors_ImGuiCol_Text;
  style.Colors[ImGuiCol_TextDisabled] = m_colorscheme.Colors_ImGuiCol_TextDisabled;
  style.Colors[ImGuiCol_WindowBg] = m_colorscheme.Colors_ImGuiCol_WindowBg;
  style.Colors[ImGuiCol_ChildBg] = m_colorscheme.Colors_ImGuiCol_ChildBg;
  style.Colors[ImGuiCol_PopupBg] = m_colorscheme.Colors_ImGuiCol_PopupBg;
  style.Colors[ImGuiCol_Border] = m_colorscheme.Colors_ImGuiCol_Border;
  style.Colors[ImGuiCol_BorderShadow] = m_colorscheme.Colors_ImGuiCol_BorderShadow;
  style.Colors[ImGuiCol_FrameBg] = m_colorscheme.Colors_ImGuiCol_FrameBg;
  style.Colors[ImGuiCol_FrameBgHovered] = m_colorscheme.Colors_ImGuiCol_FrameBgHovered;
  style.Colors[ImGuiCol_FrameBgActive] = m_colorscheme.Colors_ImGuiCol_FrameBgActive;
  style.Colors[ImGuiCol_TitleBg] = m_colorscheme.Colors_ImGuiCol_TitleBg;
  style.Colors[ImGuiCol_TitleBgCollapsed] = m_colorscheme.Colors_ImGuiCol_TitleBgCollapsed;
  style.Colors[ImGuiCol_TitleBgActive] = m_colorscheme.Colors_ImGuiCol_TitleBgActive;
  style.Colors[ImGuiCol_MenuBarBg] = m_colorscheme.Colors_ImGuiCol_MenuBarBg;
  style.Colors[ImGuiCol_ScrollbarBg] = m_colorscheme.Colors_ImGuiCol_ScrollbarBg;
  style.Colors[ImGuiCol_ScrollbarGrab] = m_colorscheme.Colors_ImGuiCol_ScrollbarGrab;
  style.Colors[ImGuiCol_ScrollbarGrabHovered] = m_colorscheme.Colors_ImGuiCol_ScrollbarGrabHovered;
  style.Colors[ImGuiCol_ScrollbarGrabActive] = m_colorscheme.Colors_ImGuiCol_ScrollbarGrabActive;
  style.Colors[ImGuiCol_CheckMark] = m_colorscheme.Colors_ImGuiCol_CheckMark;
  style.Colors[ImGuiCol_SliderGrab] = m_colorscheme.Colors_ImGuiCol_SliderGrab;
  style.Colors[ImGuiCol_SliderGrabActive] = m_colorscheme.Colors_ImGuiCol_SliderGrabActive;
  style.Colors[ImGuiCol_Button] = m_colorscheme.Colors_ImGuiCol_Button;
  style.Colors[ImGuiCol_ButtonHovered] = m_colorscheme.Colors_ImGuiCol_ButtonHovered;
  style.Colors[ImGuiCol_ButtonActive] = m_colorscheme.Colors_ImGuiCol_ButtonActive;
  style.Colors[ImGuiCol_Header] = m_colorscheme.Colors_ImGuiCol_Header;
  style.Colors[ImGuiCol_HeaderHovered] = m_colorscheme.Colors_ImGuiCol_HeaderHovered;
  style.Colors[ImGuiCol_HeaderActive] = m_colorscheme.Colors_ImGuiCol_HeaderActive;

  style.Colors[ImGuiCol_ResizeGrip] = m_colorscheme.Colors_ImGuiCol_ResizeGrip;
  style.Colors[ImGuiCol_ResizeGripHovered] = m_colorscheme.Colors_ImGuiCol_ResizeGripHovered;
  style.Colors[ImGuiCol_ResizeGripActive] = m_colorscheme.Colors_ImGuiCol_ResizeGripActive;
  style.Colors[ImGuiCol_PlotLines] = m_colorscheme.Colors_ImGuiCol_PlotLines;
  style.Colors[ImGuiCol_PlotLinesHovered] = m_colorscheme.Colors_ImGuiCol_PlotHistogramHovered;
  style.Colors[ImGuiCol_PlotHistogram] = m_colorscheme.Colors_ImGuiCol_PlotHistogram;
  style.Colors[ImGuiCol_PlotHistogramHovered] = m_colorscheme.Colors_ImGuiCol_PlotHistogramHovered;
  style.Colors[ImGuiCol_TextSelectedBg] = m_colorscheme.Colors_ImGuiCol_TextSelectedBg;
}

void Interface::RenderDebugger()
{
  Begin(ICON_FA_CODE " Application Debugger");

  Text("Frames %u", m_frames);
  Text("Camera should move: %s", m_app->GetCamera()->GetShouldMove() ? "true" : "false");
  yg_vec3 cameraPos = m_app->GetCamera()->GetPosition();
  Text("Camera world position: x: %f y: %f z: %f", cameraPos.x, cameraPos.y, cameraPos.z);
  Checkbox("Windows dont move", &m_dont_move_windows_editor);
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