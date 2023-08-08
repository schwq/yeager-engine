#include "renderer.h"
#include <GLFW/glfw3.h>

RendererEngine::RendererEngine(RendererEngineName name, Application *app) : m_app(app) {
    m_handle.name = name;
}

void RendererEngine::Render() {
    switch (m_handle.name) {
    case RendererEngineName::kOpenGL:
        RendererOpenGL();
        break;
    case RendererEngineName::kDirectX:
        break;
    case RendererEngineName::kVulkan:
        break;
    default:
        return;
    }
}

void RendererEngine::RendererOpenGL() {

    VLOG_F(INFO, "RENDER OPENGL");

    while (m_app->ShouldRendererActive()) {

        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        m_app->GetInput()->ProcessInputRender(m_app->GetWindowManager());
        m_app->GetInterface()->RenderUI();

        glfwSwapBuffers(m_app->GetWindowManager()->getWindow());
        glfwPollEvents();
    }
}

void ::RendererEngine::checkGLAD() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        VLOG_F(ERROR, "Cannot load GLAD!");
    }
}