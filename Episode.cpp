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

#include "Episode.h"

/*
===================
Episode::Episode
===================
*/
Episode::Episode()
{
    Reset();
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
Episode::Reset
===================
*/
void Episode::Reset()
{
    name.Clear();
    filepath.Clear();
    levels.Clear();
    levels.Append(Level());
    unlocked = 1;
}

/*
===================
Episode::LoadNumberOfLevels
===================
*/
int Episode::LoadNumberOfLevels(const char *path)
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
Episode::LoadNumberOfUnlockedLevels
===================
*/
bool Episode::LoadNumberOfUnlockedLevels(libHolder<libInt32> &unlocked, const char *path, int numOfLevels)
{
    unlocked.Set(0);

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
        libHolder<int> levelsSize;
        save.Read(&levelsSize, sizeof(libHolder<libInt32>), 1);

        if (levelsSize.Get() == numOfLevels)
            save.Read(&unlocked, sizeof(libHolder<libInt32>), 1);

        save.Close();

        return unlocked.Get() ? true : false;
    }
#else
    unlocked.Set(INT_MAX);
    return true;
#endif

    unlocked.Set(1);
    return false;
}

/*
===================
Episode::Load
===================
*/
int Episode::Load(Episode &episode, const char *path)
{
    episode.Reset();

    libFile lvl(path, libFile::Mode::ReadBin);
    if (!lvl.IsOpened())
        return 0;

    libInt32 numOfLevels = 0;
    lvl.Read(&numOfLevels, sizeof(libInt32), 1);

    if (numOfLevels)
    {
        libStr baseName;
        libStr::ExtractBaseName(path, baseName);

        episode.Reset();
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
                    Block::type_t type;
                    lvl.Read(&type, sizeof(Block::type_t), 1);
                    level.SetBlockType(i, j, type);
                }
            }

            level.CalculateFloor();
        }

        Episode::LoadNumberOfUnlockedLevels(episode.unlocked, path, libCast<int>(episode.levels.Size()));
    }

    lvl.Close();
    return libCast<int>(episode.levels.Size());
}

/*
===================
Episode::SaveNumberOfUnlockedLevels
===================
*/
void Episode::SaveNumberOfUnlockedLevels(const Episode &episode)
{
    libStr folderPath;
    libDir::GetLocalDataLocation(folderPath);
    folderPath.Append("/Pusher/Saves");

    libStr baseName;
    episode.filepath.ExtractBaseName(baseName);

    libStr filepath(folderPath);
    filepath.Append("/");
    filepath.Append(baseName);
    filepath.Append(".sav");

    libDir::Create(folderPath.Get());

    libFile save(filepath.Get(), libFile::Mode::WriteBin);
    libHolder<libInt32> levelsSize(libCast<int>(episode.levels.Size()));
    libHolder<libInt32> temp(episode.unlocked.Get());
    save.Write(&levelsSize, sizeof(libHolder<libInt32>), 1);
    save.Write(&temp, sizeof(libHolder<libInt32>), 1);
    save.Close();
}

/*
===================
Episode::Save
===================
*/
bool Episode::Save(const Episode &episode, const char *path)
{
    libFile lvl(path, libFile::Mode::WriteBin);
    if (!lvl.IsOpened())
        return false;

    int numOfLevels = libCast<int>(episode.levels.Size());
    lvl.Write(&numOfLevels, sizeof(int), 1);

    for (auto &level : episode.levels)
    {
        for (int i = 0; i < LEVEL_WIDTH; i++)
        {
            for (int j = 0; j < LEVEL_HEIGHT; j++)
            {
                lvl.Write(&level.GetBlock(i, j), sizeof(Block::type_t), 1);
            }
        }
    }

    lvl.Close();
    return true;
}
