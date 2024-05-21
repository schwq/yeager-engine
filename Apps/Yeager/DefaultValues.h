//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine
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

#include "Common/Common.h"
#include "Common/LogEngine.h"
#include "Common/Utilities.h"

namespace Yeager {

class ApplicationCore;
class MaterialTexture2D;

class DefaultValues {
 public:
  DefaultValues(Yeager::ApplicationCore* application);

  void GenerateDefaults();

  std::shared_ptr<MaterialTexture2D> GetTexture();

 private:
  Yeager::ApplicationCore* m_Application = YEAGER_NULLPTR;

  std::shared_ptr<MaterialTexture2D> m_DefaultTexture = YEAGER_NULLPTR;
};

};  // namespace Yeager