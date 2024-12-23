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

#ifndef __GAME_H__
#define __GAME_H__

#include "Main.h"
#include "Level.h"
#include "Episode.h"

#define ROBOT_ANIMATION_DURATION 720.0f

#define MOVE_DELAY 50.0f
#define KEY_DELAY 200.0f

enum moveType_t
{
    MOVE_BLOCKED = -1,
    MOVE_NONE,
    MOVE,
    MOVE_BOX,
};

/*
===========================================================

    Game

===========================================================
*/
class Game
{
public:

    enum state_t
    {
        NOT_STARTED,
        IN_GAME,
        EDITOR,
        EDITOR_PLAYTEST,
        EDITOR_NEW_NAME
    };

                        Game();

    bool                Init();
    void                Draw();
    void                Update();

    void                SetState(state_t state);
    void                SetEpisode(const Episode &episode);
    void                SaveEpisode(const char *path = nullptr);

    void                Reset();
    void                RestartLevel();
    void                SetLevel(int level);
    void                PreviousLevel();
    void                NextLevel();
    void                AddLevel();
    void                DuplicateLevel();
    void                MoveLevelForward();
    void                MoveLevelBackward();
    void                DeleteLevel();
    void                SaveHistory();
    void                UndoLatestChange();

    bool                IsCompleted() const;
    bool                IsCursorInField() const;
    state_t             GetState();
    Level &             GetLevel();

private:

    enum moveDirection_t
    {
        NEUTRAL,
        UP,
        DOWN,
        LEFT,
        RIGHT
    };

    void                GameUpdate();
    void                InputUpdate();
    void                MoveRobot(const libVec2i &direction);
    void                EditorUpdate();
    void                EditorNewNameUpdate();

    moveType_t          FindPath(int(&path)[LEVEL_WIDTH][LEVEL_HEIGHT], int x, int y);

    state_t             state;
    Episode             episode;
    Level               gameLevel;
    libStack<Level>     history;
    int                 currentLevel;
    int                 currentBlock;
    int                 path[LEVEL_WIDTH][LEVEL_HEIGHT];
    libVec2i            robot;
    libVec2i            mouse;
    bool                episodeAlreadyExists;
    bool                gameCompleted;
    bool                movedSaved = false;
    float               moveMouseDelay;
    float               moveKeyDelay;
    bool                canMoveByMouse;
    moveDirection_t     moveKeyDir;

    libTexture *        t_moveBlocked = nullptr;
    libTexture *        t_movePath = nullptr;
    libTexture *        t_move = nullptr;
    libTexture *        t_moveBox = nullptr;
    libTexture *        t_box = nullptr;
    libTexture *        t_endpoint = nullptr;
    libTexture *        t_wall = nullptr;
    libTexture *        t_floor = nullptr;
    libTexture *        t_backgrounds = nullptr;
    libSprite *         t_robot = nullptr;
    libSound *          s_move = nullptr;
    libSound *          s_levelComplete = nullptr;

    libTimer            moveTimer;
};

#endif // !__GAME_H__
