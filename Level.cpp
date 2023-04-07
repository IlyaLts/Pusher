/*
===============================================================================
    Copyright (C) 2015-2023 Ilya Lyakhovets

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

#include "Level.h"

/*
===================
Level::Level
===================
*/
Level::Level()
{
    Clear();
}

/*
===================
Level::Level
===================
*/
Level::Level(const Level &other)
{
    *this = other;
}

/*
===================
Level::operator =
===================
*/
const Level &Level::operator =(const Level &other)
{
    memcpy(blocks, other.blocks, sizeof(blocks));
    return *this;
}

/*
===================
Level::Set
===================
*/
void Level::Set(int x, int y, Block::Type type, bool calculateFloor)
{
    blocks[x][y].type = type;
    
    if (calculateFloor)
        CalculateFloor();
}

/*
===================
Level::Get
===================
*/
const Block & Level::Get(int x, int y) const
{
    return blocks[x][y];
}

/*
===================
Level::ShiftUp
===================
*/
void Level::ShiftUp()
{
    Level temp(*this);

    for (int i = 0; i < LEVEL_WIDTH; i++)
    {
        for (int j = 0; j < LEVEL_HEIGHT; j++)
        {
            blocks[i][j] = j < LEVEL_HEIGHT - 1 ? temp.blocks[i][j + 1] : temp.blocks[i][0];
        }
    }

    CalculateFloor();
}

/*
===================
Level::ShiftDown
===================
*/
void Level::ShiftDown()
{
    Level temp(*this);

    for (int i = 0; i < LEVEL_WIDTH; i++)
    {
        for (int j = 0; j < LEVEL_HEIGHT; j++)
        {
            blocks[i][j] = j > 0 ? temp.blocks[i][j - 1] : temp.blocks[i][LEVEL_HEIGHT - 1];
        }
    }

    CalculateFloor();
}

/*
===================
Level::ShiftLeft
===================
*/
void Level::ShiftLeft()
{
    Level temp(*this);

    for (int i = 0; i < LEVEL_WIDTH; i++)
    {
        for (int j = 0; j < LEVEL_HEIGHT; j++)
        {
            blocks[i][j] = i < LEVEL_WIDTH - 1 ? temp.blocks[i + 1][j] : temp.blocks[0][j];
        }
    }

    CalculateFloor();
}

/*
===================
Level::ShiftRight
===================
*/
void Level::ShiftRight()
{
    Level temp(*this);

    for (int i = 0; i < LEVEL_WIDTH; i++)
    {
        for (int j = 0; j < LEVEL_HEIGHT; j++)
        {
            blocks[i][j] = i > 0 ? temp.blocks[i - 1][j] : temp.blocks[LEVEL_WIDTH - 1][j];
        }
    }

    CalculateFloor();
}

/*
===================
Level::CalculateFloor
===================
*/
void Level::CalculateFloor()
{
    bool done = false;
    int wave[LEVEL_WIDTH][LEVEL_HEIGHT];

    // Initiates a wave at the boundaries
    for (int i = 0; i < LEVEL_WIDTH; i++)
    {
        for (int j = 0; j < LEVEL_HEIGHT; j++)
        {
            blocks[i][j].floor = false;
            wave[i][j] = 0;

            if (blocks[i][j].type == Block::WALL)
                wave[i][j] = -1;
            else if (!i || !j || i == LEVEL_WIDTH - 1 || j == LEVEL_HEIGHT - 1)
                wave[i][j] = 1;
        }
    }

    // Performs a wave motion starting from the boundaries
    while (!done)
    {
        done = true;

        for (int i = 0; i < LEVEL_WIDTH; i++)
        {
            for (int j = 0; j < LEVEL_HEIGHT; j++)
            {
                if (wave[i][j])
                    continue;

                // Up
                if (j - 1 >= 0 && wave[i][j - 1] == 1)
                {
                    wave[i][j] = 1;
                    done = false;
                }
                // Down
                if (j + 1 < LEVEL_HEIGHT && wave[i][j + 1] == 1)
                {
                    wave[i][j] = 1;
                    done = false;
                }
                // Left
                if (i - 1 >= 0 && wave[i - 1][j] == 1)
                {
                    wave[i][j] = 1;
                    done = false;
                }
                // Right
                if (i + 1 < LEVEL_WIDTH && wave[i + 1][j] == 1)
                {
                    wave[i][j] = 1;
                    done = false;
                }
            }
        }
    }

    for (int i = 0; i < LEVEL_WIDTH; i++)
        for (int j = 0; j < LEVEL_HEIGHT; j++)
            blocks[i][j].floor = wave[i][j] ? false : true;
}

/*
===================
Level::Clear
===================
*/
void Level::Clear()
{
    memset(blocks, 0, sizeof(blocks));
}
