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

#ifndef __BLOCK_H__
#define __BLOCK_H__

#include "Main.h"

class Block
{
public:

    enum type_t : libInt8
    {
        EMPTY,
        ROBOT,
        BOX,
        WALL,
        ENDPOINT,
        BOX_ON_ENDPOINT,
        ROBOT_ON_ENDPOINT,
        TYPE_COUNT
    } type;

    bool IsBox() const { return type == BOX || type == BOX_ON_ENDPOINT; }
    bool CanMoveBoxes() const { return type == EMPTY || type == ENDPOINT; }

    bool floor;
};

#endif // !__BLOCK_H__
