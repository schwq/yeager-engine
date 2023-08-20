#include "shapes.h"
#include <glm/ext/matrix_transform.hpp>

YaegerCube::~YaegerCube() {
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

YaegerCube::YaegerCube(String name, Application* app, EngineTexture2D* texture,
       const Vector3 position) : m_name(name), m_texture(texture), m_position(position), m_app(app){
        m_toolbox = new ToolBoxObject();
        m_toolbox->SetType(ExplorerObjectType::kShapes);
        m_toolbox->SetPosition(&m_position);
        m_toolbox->SetRotation(&m_rotation);
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);

        glBindVertexArray(m_vao);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        
        glBindVertexArray(0);
        m_app->explorer->AddObject(m_name.c_str(), ExplorerObjectType::kShapes, m_toolbox);
}

void YaegerCube::Draw(Shader *shader) {
    shader->UseShader();
    glBindVertexArray(m_vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture->GetID());
    Matrix4 model = Matrix4(1.0f);
    model = glm::translate(model, m_position);
    model = glm::rotate(model, glm::radians(m_rotation.x), Vector3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(m_rotation.y), Vector3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(m_rotation.z), Vector3(0.0f, 0.0f, 1.0f));
    shader->SetMat4("model", model);

    glDrawArrays(GL_TRIANGLES, 0, 36);
}