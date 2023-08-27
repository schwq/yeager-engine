#pragma once

#include "../../application.h"
#include "../../common/common.h"
#include "../renderer/shapes.h"
#include "toolbox_object.h"

class Application;
class Interface;
class ToolBoxObject;

enum class ExplorerObjectType {
  kShapes = 0,
  kScene,
  kShader,
  kTexture,
  kPlayer,
  kNetwork,
  kImage,
  kVideo,
  kAudio,
  kSkybox,
  kImportedObject,
  kNone,
  kNoChange
};

extern String ExplorerTypeToString(ExplorerObjectType type);

struct ExplorerObject {
  ExplorerObject(const char* name, ExplorerObjectType type,
                 ToolBoxObject* toolbox_object = nullptr);
  ExplorerObjectType m_type;
  String m_name;
  static uint m_object_count;
  uint m_id;
  bool m_selected = false;
  ToolBoxObject* m_object_toolbox;
};

class EditorExplorer {
 public:
  EditorExplorer(Application* app) : m_app(app){};
  void AddObject(ExplorerObject object);
  void AddObject(const char* name, ExplorerObjectType type,
                 ToolBoxObject* toolbox_object);
  void DrawExplorer();
  ExplorerObject toolbox_selected =
      ExplorerObject("NONE", ExplorerObjectType::kNone);

 private:
  void AddCubeWindow();

  std::vector<ExplorerObject> m_objects;
  bool m_first_time_toolbox = true;
  bool m_add_cube_window_open = false;
  bool m_everything_fine_to_create = true;
  String m_new_cube_name;
  String m_new_cube_texture_path;
  Application* m_app;
};