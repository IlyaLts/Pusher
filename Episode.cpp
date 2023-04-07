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

#include "Episode.h"

/*
===================
Episode::Episode
===================
*/
Episode::Episode()
{
    Clear();
}

/*
===================
Episode::operator =
===================
*/
const Episode &Episode::operator =(const Episode &other)
{
    name = other.name;
    filepath = other.filepath;
    levels = other.levels;
    unlocked = other.unlocked;

    return *this;
}

/*
===================
Episode::Clear
===================
*/
void Episode::Clear()
{
    name.Clear();
    filepath.Clear();
    levels.Clear();
    levels.Append(Level());
    unlocked = 1;
}

/*
===================
Episode::GetNumberOfLevels
===================
*/
int Episode::GetNumberOfLevels(const char *path)
{
    libFile lvl(path, libFile::Mode::ReadBin);
    int numOfLevels = 0;

    if (lvl.IsOpened())
    {
        lvl.Read(&numOfLevels, sizeof(int), 1);
        lvl.Close();
    }

    return numOfLevels;
}

/*
===================
Episode::GetNumberOfUnlockedLevels
===================
*/
bool Episode::GetNumberOfUnlockedLevels(libHolder &unlocked, const char *path, int numOfLevels)
{
    unlocked.Set(1);

    libStr baseName;
    libStr::ExtractBaseName(path, baseName);

    libStr filepath;
    libDir::GetLocalDataLocation(filepath);
    filepath.Append("/Pusher/Saves/");
    filepath.Append(baseName);
    filepath.Append(".sav");

#ifndef UNLOCK_EVERYTHING
    libFile save(filepath.Get(), libFile::Mode::ReadBin);
    if (save.IsOpened())
    {
        libHolder levelsSize;
        save.Read(&levelsSize, sizeof(libHolder), 1);
        if (levelsSize.Get() == numOfLevels) save.Read(&unlocked, sizeof(libHolder), 1);
        save.Close();

        return unlocked.Get() ? true : false;
    }
#else
    unlocked.Set(INT_MAX);
    return true;
#endif

    return false;
}

/*
===================
Episode::Load
===================
*/
int Episode::Load(Episode &episode, const char *path)
{
    episode.Clear();

    libFile lvl(path, libFile::Mode::ReadBin);
    if (!lvl.IsOpened()) return 0;

    if (lvl.IsOpened())
    {
        int numOfLevels = 0;
        lvl.Read(&numOfLevels, sizeof(int), 1);

        if (numOfLevels)
        {
            libStr baseName;
            libStr::ExtractBaseName(path, baseName);

            episode.Clear();
            episode.name = baseName;
            episode.name.Replace("_", " ");
            episode.filepath = path;
            episode.levels.Resize(numOfLevels);

            for (auto &level : episode.levels)
            {
                for (int i = 0; i < LEVEL_WIDTH; i++)
                {
                    for (int j = 0; j < LEVEL_HEIGHT; j++)
                    {
                        Block::Type type;
                        lvl.Read(&type, sizeof(Block::Type), 1);
                        level.Set(i, j, type, false);
                    }
                }

                level.CalculateFloor();
            }

            Episode::GetNumberOfUnlockedLevels(episode.unlocked, path, episode.levels.Size());
        }

        lvl.Close();
    }

    return episode.levels.Size();
}
