//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/yeager-engine
//    Copyright (C) 2023
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "../../Common/Common.h"
#include "../../Common/LogEngine.h"
#include "../Renderer/Entity.h"

class Yeager::GameEntity;

#define PI 3.14f

namespace Yeager {
class EntityPhysics {
 public:
  EntityPhysics(Yeager::GameEntity* entity, float weight = 1.0f, float gravity_const = 1.0f);
  ~EntityPhysics();

  void ApplyGravity();
  void ChangeGravity(float gravity_const, float weight)
  {
    m_weight = weight;
    m_gravity_const = gravity_const;
  }
  void GravityEnabled(bool enabled) { m_gravity_enabled = enabled; }
  void GravityReversed(bool reversed) { m_reverse_gravity = reversed; }

 private:
  float m_weight = 1.0f;
  float m_gravity_const = 1.0f;
  // If we let this two constant running alone to apply gravity to entities, it would fall
  // so quick because of the numbers of frames per second
  float m_delta_brake = 0.03f;
  float m_air_resistance = 0.01f;
  bool m_gravity_enabled = false;
  bool m_reverse_gravity = false;
  Yeager::GameEntity* m_GameEntityPtr = YEAGER_NULLPTR;
};

}  // namespace Yeager