//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine
//    Copyright (C) 2023 - Present
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
#include "../../Common.h"
#include "../../Utilities.h"

namespace Yeager {

/**
 * @brief Implementation of the brute force pattern search algorithm. Search a pattern in the given string, 
 * and returns a zero-based index of the begin position of the pattern inside the string.
 * Ex. The pattern "zero" returns a vector of indexes [0, 11] given a string "zero, one, zero, one".
 * @returns If the pattern has not found inside the string or the pattern is larger that the string, a empty vector is returned. 
 */
YEAGER_NODISCARD extern std::vector<int> BruteForceStringSearch(const String& pattern, const String& str);

}  // namespace Yeager