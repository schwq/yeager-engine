#include "interface.h"
#include "common/common.h"
#include "input.h"
using namespace ImGui;

uint Interface::m_frames = 0;

Interface::~Interface()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  DestroyContext();
}

bool InterfaceButton::CenteredButton()
{
  auto windowWidth = GetWindowSize().x;
  auto textWidth = CalcTextSize(m_text.c_str()).x;
  SetCursorPosX((windowWidth - textWidth) * 0.5f);
  return Button(m_text.c_str());
}

bool InterfaceButton::AddButton() { return Button(m_text.c_str()); }

float InterfaceButton::TextWidth() { return CalcTextSize(m_text.c_str()).x; }

InterfaceImage::InterfaceImage(const char *path)
{
  bool ret = LoadTextureFromFile(path, &m_image_texture, &m_image_width,
                                 &m_image_height);
  IM_ASSERT(ret);
}

void InterfaceImage::LoadInterfaceImage()
{
  Image((void *)(intptr_t)m_image_texture,
        ImVec2(m_image_width / 2, m_image_height / 2));
}

void InterfaceImage::LoadInterfaceCenteredImage()
{
  auto windowWidth = GetWindowSize().x;
  SetCursorPosX((windowWidth - m_image_width) * 0.5f);
  Image((void *)(intptr_t)m_image_texture,
        ImVec2(m_image_width / 2, m_image_height / 2));
}

void Interface::AlignForWidth(float width, float alignment)
{
  float avail = GetContentRegionAvail().x;
  float off = (avail - width) * alignment;
  if (off > 0.0f) SetCursorPosX(GetCursorPosX() + off);
}

Interface::Interface(Window *window, Application *app) : m_app(app)
{
  IMGUI_CHECKVERSION();
  CreateContext();
  m_imgui_io = GetIO();
  (void)m_imgui_io;
  m_imgui_io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  m_imgui_io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  StyleColorsDark();
  m_imgui_style = GetStyle();

  ImGui_ImplGlfw_InitForOpenGL(window->getWindow(), true);
  ImGui_ImplOpenGL3_Init("#version 330");
  VLOG_F(INFO, "Initialize ImGui");

  m_imgui_io.Fonts->AddFontFromFileTTF(
      "C:\\Users\\schwq\\OneDrive\\Documentos\\GitHub\\yeager-"
      "engine\\apps\\yeager\\assets\\fonts\\big_blue_term.ttf",
      size_pixels);
  StyleImGui();
  m_initialize = true;
}

void Interface::CreateSpaceX(uint count)
{
  if (count > 0) {
    for (uint x = 0; x < count; x++) {
      NewLine();
    }
  }
}

void Interface::RenderUI()
{
  m_frames++;
  switch (m_current_mode) {
    case InterfaceMode::kLauncherMode:
      RenderLauncher();
      break;
    case InterfaceMode::kEditorMode:
      RenderEditor();
      break;
    default:
      RenderError();
  }
}

void Interface::RenderAwait() {}

void Interface::RenderEditor()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  NewFrame();

  if (m_dont_move_windows_editor) {
    Begin("Explorer", NULL,
          ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
              ImGuiWindowFlags_NoMove);
  }
  else {
    Begin("Explorer");
  }
  End();

  if (m_dont_move_windows_editor) {
    Begin("Console", NULL,
          ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
              ImGuiWindowFlags_NoMove);
  }
  else {
    Begin("Console");
  }
  End();

  RenderDebugger();

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

  Render();
  ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
}

void Interface::RenderLauncher()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  NewFrame();
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
  Render();
  ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
}

void Interface::CenteredText(String text)
{
  auto windowWidth = GetWindowSize().x;
  auto textWidth = CalcTextSize(text.c_str()).x;

  SetCursorPosX((windowWidth - textWidth) * 0.5f);
  Text(text.c_str());
}

void Interface::logButton(InterfaceButton button)
{
  VLOG_F(INFO, "%s button was pressed", button.m_name.c_str());
}
void Interface::StyleImGui()
{
  ImGuiStyle &style = ImGui::GetStyle();
  style.WindowRounding = 5.3f;
  style.FrameRounding = 2.3f;
  style.ScrollbarRounding = 0;

  style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 0.90f);
  style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
  style.Colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.15f, 1.00f);
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

void Interface::RenderDebugger()
{
  Begin("Application Debugger");

  Text("Frames %u", m_frames);
  Text("Camera should move: %s",
       m_app->GetEditorCamera()->GetShouldMove() ? "true" : "false");
  Vector3 cameraPos = m_app->GetEditorCamera()->GetPosition();
  Text("Camera world position: x: %f y: %f z: %f", cameraPos.x, cameraPos.y,
       cameraPos.z);
  Checkbox("Windows dont move", &m_dont_move_windows_editor);

  End();
}