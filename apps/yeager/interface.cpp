#include "interface.h"
#include "common/common.h"
#include "engine/editor/editor_explorer.h"
#include "input.h"
using namespace ImGui;

uint Interface::m_frames = 0;

Interface::~Interface() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  DestroyContext();
}

void Interface::CenteredWindow(uint size_x, uint size_y) {
  SetNextWindowPos(
      ImVec2((kWindowX / 2) - (size_x / 2), (kWindowY / 2) - (size_y / 2)));
  SetNextWindowSize(ImVec2(size_x, size_y));
}

bool InterfaceButton::CenteredButton() {
  auto windowWidth = GetWindowSize().x;
  auto textWidth = CalcTextSize(m_text.c_str()).x;
  SetCursorPosX((windowWidth - textWidth) * 0.5f);
  return Button(m_text.c_str());
}

void Interface::DisplayWarningWindow() {
  if (m_current_warning.active) {
    Begin("Warning!");
    CenteredWindow(m_current_warning.size_x, m_current_warning.size_y);
    CenteredText(m_current_warning.warning);
    if (m_current_warning
            .first_log_warning) {  // Doesnt loop various warning forever, crashes if so
      YeagerLog(WARNING, kSystemLog, "{}",
                       m_current_warning.warning.c_str());
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

void Interface::AddWarningWindow(const String& warning, uint size_x,
                                 uint size_y) {
  m_current_warning.warning = warning;
  m_current_warning.size_x = size_x;
  m_current_warning.size_y = size_y;
  m_current_warning.active = true;
}

bool InterfaceButton::AddButton() {
  return Button(m_text.c_str());
}

float InterfaceButton::TextWidth() {
  return CalcTextSize(m_text.c_str()).x;
}

InterfaceImage::InterfaceImage(const char* path, Application* m_app) {
  bool ret = LoadTextureFromFile(path, &m_image_texture, &m_image_width,
                                 &m_image_height);
  YeagerLog(INFO, kSystemLog, "{}", path);
  IM_ASSERT(ret);
}

void InterfaceImage::LoadInterfaceImage() {
  Image((void*)(intptr_t)m_image_texture,
        ImVec2(m_image_width / 2, m_image_height / 2));
}

void InterfaceImage::LoadInterfaceCenteredImage() {
  auto windowWidth = GetWindowSize().x;
  SetCursorPosX((windowWidth - m_image_width) * 0.5f);
  Image((void*)(intptr_t)m_image_texture,
        ImVec2(m_image_width / 2, m_image_height / 2));
}

void Interface::AlignForWidth(float width, float alignment) {
  float avail = GetContentRegionAvail().x;
  float off = (avail - width) * alignment;
  if (off > 0.0f)
    SetCursorPosX(GetCursorPosX() + off);
}

void Interface::LaunchImGui(Window* window) {

  IMGUI_CHECKVERSION();
  CreateContext();

  ImGuiIO& m_imgui_io = GetIO();
  (void)m_imgui_io;

      ImGuiConfigFlags_NavEnableKeyboard |
      ImGuiConfigFlags_NavEnableGamepad;  // Enable Keyboard Controls

  StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window->getWindow(), true);
  ImGui_ImplOpenGL3_Init("#version 330");
}

Interface::Interface(Window* window, Application* app) : m_app(app) {

  LaunchImGui(window);

  YeagerLog(INFO, kSystemLog, "Initialize ImGui");
  YeagerLog(INFO, kSystemLog, "{}",
                   GetPath("/assets/fonts/big_blue_term.ttf").c_str());

  ImGuiIO& m_imgui_io = GetIO();
  m_imgui_io.Fonts->AddFontFromFileTTF(
      GetPath("/assets/fonts/big_blue_term.ttf").c_str(), size_pixels);

  StyleImGui();

  m_initialize = true;
}

void Interface::CreateSpaceX(uint count) {
  if (count > 0) {
    for (uint x = 0; x < count; x++) {
      NewLine();
    }
  }
}

void Interface::RenderUI() {
  m_frames++;

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  NewFrame();

  switch (m_current_mode) {
    case InterfaceMode::kLauncherMode:
      RenderLauncher();
      DisplayWarningWindow();
      break;
    case InterfaceMode::kEditorMode:
      RenderEditor();
      DisplayWarningWindow();
      break;
    default:
      RenderError();
  }

  Render();
  ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
}

void Interface::RenderAwait() {}

void Interface::DrawExplorer() {
  Begin("Explorer", NULL,
        m_dont_move_windows_editor ? kWindowStatic : kWindowMoveable);

  m_app->GetExplorer()->DrawExplorer();

  End();

  Begin("Console", NULL,
        m_dont_move_windows_editor ? kWindowStatic : kWindowMoveable);

  if (Button("Add Comment:")) {
    m_comment_window_open = true;
  }
  if (m_comment_window_open) {
    CenteredWindow(400, 50);
    Begin("Comment");
    InputText("Say: ", &comment);
    SameLine();
    m_app->GetEditorCamera()->SetShouldMove(false);
    m_app->GetInput()->SetCursorCanDisappear(false);

    if (Button("ADD") || m_app->EnterKeyPressed()) {
      m_comment_window_open = false;
      YeagerLog(INFO, ConsoleLogSender::kUserLog, "{}", comment.c_str());
      comment.clear();
      m_app->GetEditorCamera()->SetShouldMove(true);
      m_app->GetInput()->SetCursorCanDisappear(true);
    }
    End();
  }
  m_app->GetConsole()->ReadLog();

  End();
}

void Interface::DrawToolbox() {
  Begin("Toolbox", NULL,
        m_dont_move_windows_editor
            ? ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                  ImGuiWindowFlags_NoMove
            : ImGuiWindowFlags_None);

  if (m_app->GetExplorer()->toolbox_selected.m_type ==
      ExplorerObjectType::kNone) {
    Text("None");
  } else {
    Text(m_app->GetExplorer()->toolbox_selected.m_name.c_str());
    m_app->GetExplorer()->toolbox_selected.m_object_toolbox->DrawObject();
  }

  End();
}

void Interface::DrawEditorMenu() {
  InterfaceButton file_button("editor_file_button", "FILE");
  InterfaceButton edit_button("editor_edit_button", "EDIT");
  InterfaceButton help_button("editor_help_button", "HELP");
  InterfaceButton debug_button("editor_debug_button", "DEBUG");

  SetNextWindowPos(ImVec2(0, 0));
  SetNextWindowSize(ImVec2(kWindowX, 50.0f));
  Begin("Editor menu", NULL,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoScrollbar);

  if (file_button.AddButton()) {
    logButton(file_button);
  }
  SameLine();
  if (edit_button.AddButton()) {
    logButton(edit_button);
  }
  SameLine();
  if (help_button.AddButton()) {
    logButton(help_button);
  }
  SameLine();
  if (debug_button.AddButton()) {
    logButton(debug_button);
  }

  End();
}

void Interface::RenderEditor() {
  DrawExplorer();
  DrawToolbox();
  RenderDebugger();
  DrawEditorMenu();
}

void Interface::RenderLauncher() {

  InterfaceButton open_project("interface_open_project", "Open Project");
  InterfaceButton new_project("interface_new_project", "New Project");
  InterfaceButton settings_button("interface_settings_button", "Settings");
  InterfaceButton help_button("interface_help_button", "Help");

  SetNextWindowPos(ImVec2(0, 0));
  SetNextWindowSize(ImVec2(kWindowX, kWindowY));
  Begin("Yaeger Launcher", NULL,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

  CenteredText("Welcome to the Yaeger Engine!");
  CenteredText(
      "This is a small project, being build by one developer with the propose");
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
    m_app->SetCurrentMode(ApplicationCurrentMode::kEditorMode);
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

void Interface::CenteredText(String text) {
  auto windowWidth = GetWindowSize().x;
  auto textWidth = CalcTextSize(text.c_str()).x;

  SetCursorPosX((windowWidth - textWidth) * 0.5f);
  Text(text.c_str());
}

void Interface::logButton(InterfaceButton button) {
  YeagerLog(INFO, kSystemLog, "{} button was pressed",
                   button.m_name.c_str());
}
void Interface::StyleImGui() {
  ImGuiStyle& style = ImGui::GetStyle();
  style.WindowRounding = 5.3f;
  style.FrameRounding = 2.3f;
  style.ScrollbarRounding = 3;
  style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 0.90f);
  style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
  style.Colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
  style.Colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  style.Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.85f);
  style.Colors[ImGuiCol_Border] = ImVec4(0.70f, 0.70f, 0.70f, 0.65f);
  style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  style.Colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.01f, 1.00f);
  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.80f, 0.80f, 0.40f);
  style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.90f, 0.65f, 0.65f, 0.45f);
  style.Colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.83f);
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
  style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 0.87f);
  style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.01f, 0.01f, 0.02f, 0.80f);
  style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
  style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.55f, 0.53f, 0.55f, 0.51f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] =
      ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
  style.Colors[ImGuiCol_ScrollbarGrabActive] =
      ImVec4(0.56f, 0.56f, 0.56f, 0.91f);
  style.Colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 0.83f);
  style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.70f, 0.70f, 0.70f, 0.62f);
  style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.30f, 0.30f, 0.30f, 0.84f);
  style.Colors[ImGuiCol_Button] = ImVec4(0.48f, 0.72f, 0.89f, 0.49f);
  style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.50f, 0.69f, 0.99f, 0.68f);
  style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
  style.Colors[ImGuiCol_Header] = ImVec4(0.30f, 0.69f, 1.00f, 0.53f);
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.44f, 0.61f, 0.86f, 1.00f);
  style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.38f, 0.62f, 0.83f, 1.00f);

  style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.85f);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
  style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);

  style.Colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogramHovered] =
      ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
}

void Interface::RenderDebugger() {
  Begin("Application Debugger");

  Text("Frames %u", m_frames);
  Text("Camera should move: %s",
       m_app->GetEditorCamera()->GetShouldMove() ? "true" : "false");
  Vector3 cameraPos = m_app->GetEditorCamera()->GetPosition();
  Text("Camera world position: x: %f y: %f z: %f", cameraPos.x, cameraPos.y,
       cameraPos.z);
  Checkbox("Windows dont move", &m_dont_move_windows_editor);
  if (Button("Throw Console Error")) {
    YeagerLog(ERROR, kSystemLog, "Test Error");
  }
  SameLine();
  if (Button("Throw Console Warning")) {
    YeagerLog(WARNING, kSystemLog, "Test Warning");
  }

  End();
}