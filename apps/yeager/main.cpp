#include "common/common.h"

#include "application.h"
#include "engine/renderer/renderer.h"
#include "engine/renderer/window.h"
#include <GLFW/glfw3.h>

int main(int argc, char *argv[]) {

    static Application app;

    VLOG_F(INFO, "START PROGRAM");

    glfwSetFramebufferSizeCallback(app.GetWindowManager()->getWindow(),
                                   app.GetWindowManager()->FramebufferSizeCallback);
    glfwSetInputMode(app.GetWindowManager()->getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(app.GetWindowManager()->getWindow(), app.GetInput()->MouseCallback);

    app.GetRendererEngine()->checkGLAD();

    app.GetRendererEngine()->Render();

    VLOG_F(INFO, "SHUTDOWN");
}