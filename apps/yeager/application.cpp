#include "application.h"
#include "input.h"
#include <GLFW/glfw3.h>

Application::Application() {
    m_window = new Window(WindowX, WindowY);
    m_engine = new RendererEngine(RendererEngineName::kOpenGL, this);
    m_input = new Input(this);
    m_interface = new Interface(m_window, this);
    m_camera = new EditorCamera(this);
}
Application::~Application() {
    delete m_engine;
    delete m_input;
    delete m_interface;
    delete m_window;
}
bool Application::ShouldRendererActive() {
    if (glfwWindowShouldClose(m_window->getWindow())) {
        return false;
    }
    return true;
}

const void Application::SetCurrentMode(ApplicationCurrentMode mode) {
    switch (mode) {
    case ApplicationCurrentMode::kEditorMode:
        m_input->SetCurrentMode(InputCurrentMode::kEditorMode);
        m_interface->SetCurrentMode(InterfaceMode::kEditorMode);
        break;
    case ApplicationCurrentMode::kAwaitMode:
        m_input->SetCurrentMode(InputCurrentMode::kAwaitMode);
        m_interface->SetCurrentMode(InterfaceMode::kAwaitMode);
        break;
    case ApplicationCurrentMode::kLauncherMode:
        m_input->SetCurrentMode(InputCurrentMode::kLauncherMode);
        m_interface->SetCurrentMode(InterfaceMode::kLauncherMode);
        break;
    case ApplicationCurrentMode::kSettingsMode:
        m_input->SetCurrentMode(InputCurrentMode::kSettingsMode);
        m_interface->SetCurrentMode(InterfaceMode::kSettingsMode);
        break;
    case ApplicationCurrentMode::kErrorMode:
        m_input->SetCurrentMode(InputCurrentMode::kErrorMode);
        m_interface->SetCurrentMode(InterfaceMode::kErrorMode);
        break;

        // TODO rest of modes
    }
}
