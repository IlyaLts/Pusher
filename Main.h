/*
===============================================================================
    Copyright (C) 2015 Ilya Lyakhovets

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
===============================================================================
*/

#ifndef __MAIN_H__
#define __MAIN_H__

#include "../libEngine/Include/Engine.h"

#ifdef LIB_RELEASE
    #define ENGINE_PATH "./"
    #define DATA_PACK "Data.lpk//"
#else
    #define ENGINE_PATH "../../libEngine/Binaries/"
    #define DATA_PACK "Data/"
#endif

//#define UNLOCK_EVERYTHING

#define PUSHER_VERSION              "1.0"
#define WINDOW_DEFAULT_WIDTH        800
#define WINDOW_DEFAULT_HEIGHT       680
#define WINDOW_MINIMAL_WIDTH        640
#define WINDOW_MINIMAL_HEIGHT       544
#define CUSTOM_LEVELS_PATH          "Levels/"

extern libVec2i screenSize;
extern libVec2i gameScreenPos;
extern libVec2i gameScreenSize;
extern libVec2 tileSize;

extern libFont *font;
extern libCfg cfg;

#endif // !__MAIN_H__
    