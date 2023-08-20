#pragma once 

#include <cassert>
#include "../../common/common.h"
#include "../../application.h"
#include "editor_explorer.h"
#include "../renderer/texture.h"

enum class ExplorerObjectType;
class EngineTexture2D;

class ToolBoxObject {
public:
    ToolBoxObject() {};
    Vector3 *GetPosition()  noexcept { assert(m_position == nullptr); return m_position; }
    void SetPosition(Vector3* position) {m_position = position; }
    Vector3 *GetRotation() noexcept { assert(m_rotation == nullptr); return m_rotation; }
    void SetRotation(Vector3* rotation) { m_rotation = rotation; }
    EngineTexture2D* GetTexture() noexcept { assert(m_texture == nullptr); return m_texture; }
    void SetTexture(EngineTexture2D* texture) { m_texture = texture; }
    void DrawObject();
    void SetType(ExplorerObjectType type) { m_type = type; }

private:
    ExplorerObjectType m_type;
    Vector3 *m_position = nullptr;
    Vector3 *m_rotation = nullptr;
    EngineTexture2D* m_texture = nullptr;
    bool m_random_rotation_checkbox = false;
};