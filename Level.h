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

#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "Main.h"
#include "Block.h"

#define LEVEL_WIDTH 20
#define LEVEL_HEIGHT 17

/*
===========================================================

    Level

===========================================================
*/
class Level
{
public:

                        Level();
                        explicit Level(const Level &other);

    const Level &       operator =(const Level &other);

    void                SetBlockType(int x, int y, Block::type_t type);
    const Block &       GetBlock(int x, int y) const;

    void                ShiftUp();
    void                ShiftDown();
    void                ShiftLeft();
    void                ShiftRight();
    void                CalculateFloor();

    void                Reset();

    bool                IsCompleted() const;

private:

    Block               blocks[LEVEL_WIDTH][LEVEL_HEIGHT];
};

#endif // !__LEVEL_H__
