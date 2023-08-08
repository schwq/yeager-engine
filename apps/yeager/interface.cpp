#include "interface.h"
#include "common/common.h"
#include "input.h"
using namespace ImGui;

Interface::Interface(Window *window, Application *app) : m_app(app) {
    IMGUI_CHECKVERSION();
    CreateContext();
    m_imgui_io = GetIO();
    (void)m_imgui_io;
    StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window->getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
    VLOG_F(INFO, "Initialize ImGui");
    m_initialize = true;
}

void Interface::RenderUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    NewFrame();

    switch (m_current_mode) {
    case InterfaceMode::kAwaitMode:
        RenderAwait();
        break;
    case InterfaceMode::kEditorMode:
        RenderEditor();
        break;
    case InterfaceMode::kLauncherMode:
        RenderLauncher();
        break;
    case InterfaceMode::kSettingsMode:
        RenderSettings();
        break;
    case InterfaceMode::kErrorMode:
    default:
        RenderError();
    }

    Render();
    ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
}

void Interface::RenderAwait() {
    SetNextWindowPos(ImVec2(0, 0));
    SetNextWindowSize(ImVec2(WindowX, WindowY));
    Begin("WAITING...", NULL,
          ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
              ImGuiWindowFlags_NoScrollbar);
    Text("Hello test");
    if (Button("EDITOR MODE")) {
        m_app->SetCurrentMode(ApplicationCurrentMode::kEditorMode);
    }
    End();
}

void Interface::RenderEditor() {
    SetNextWindowPos(ImVec2(0, 0));
    SetNextWindowSize(ImVec2(std::round(WindowX / 3.5), WindowY));
    Begin("WAITING...", NULL,
          ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
              ImGuiWindowFlags_NoScrollbar);
    Text("EDITORRRRRR");
    if (Button("AWAIT MODE")) {
        m_app->SetCurrentMode(ApplicationCurrentMode::kAwaitMode);
    }
    End();
}