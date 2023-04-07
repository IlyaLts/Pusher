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

#ifndef __EPISODE_H__
#define __EPISODE_H__

#include "Main.h"
#include "Level.h"

/*
===========================================================

    Episode

===========================================================
*/
class Episode
{
public:

                        Episode();

    const Episode &     operator =(const Episode &other);

    void                Reset();

    static int          LoadNumberOfLevels(const char *path);
    static bool         LoadNumberOfUnlockedLevels(libHolder<libInt32> &unlocked, const char *path, int numOfLevels);
    static int          Load(Episode &episode, const char *path);
    static void         SaveNumberOfUnlockedLevels(const Episode &episode);
    static bool         Save(const Episode &episode, const char *path);

    libStr              name;
    libStr              filepath;
    libList<Level>      levels;
    libHolder<libInt32> unlocked;
};

#endif // !__EPISODE_H__
