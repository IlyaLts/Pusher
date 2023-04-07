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

#ifndef __MENU_H__
#define __MENU_H__

#include "Main.h"
#include "Episode.h"

#define EPISODE_INDENT              0.025f
#define EPISODES_HEIGHT             0.53f
#define BUTTON_LEVEL_SIZE           0.55f
#define BUTTON_MAIN_SIZE            0.083f
#define BUTTON_MAIN_WIDTH_RATIO     4.0f
#define BUTTON_TEXT_SCALE           0.50f
#define BUTTON_SOUND_SIZE           0.066f
#define BUTTON_HELP_SIZE            0.04f

#define LOCKED_SIZE                 0.5f
#define CHECKMARK_SIZE              0.5f

#define NUM_OF_EPISODES             8
#define EPISODE_COLUMNS             2
#define EPISODE_ROWS                4
#define CREDITS_SPEED               25.0f

class Game;

/*
===========================================================

    Menu

===========================================================
*/
class Menu
{
public:

    enum class State
    {
        MAIN_MENU,
        PLAY_EPISODES,
        PLAY_LEVELS,
        CUSTOM_EPISODES,
        CUSTOM_LEVELS,
        EDITOR,
        EDITOR_EPISODES,
        EDITOR_LEVELS,
        CREDITS,
        HELP,
        IN_GAME
    };

                            Menu();
    
    bool                    Init();
    bool                    Draw();
    bool                    Update(Game *game = nullptr);

    void                    SetState(State state) { this->state = state; }
    State                   GetState() const { return state; }

private:

    State                   state;
    int                     currentPage;

    Episode                 episode;
    libList<libStr>         episodePaths;
    libList<libStr>         episodeNames;
    libList<bool>           finishedEpisodeList;

    libVec2                 levelButtonIndent;
    libVec2                 levelButtonSize;

    libVec2                 creditsStart;
    float                   creditsPos;
    float                   creditsSize;
    int                     gameHelpLines;
    int                     editorHelpLines;
    int                     creditsLines;

    libColorState_t         buttonColor = { libColor(), libColor(0.75f, 0.75f, 0.75f), libColor(0.5f, 0.5f, 0.5f) };

    libTexture *            t_buttonSoundOn = nullptr;
    libTexture *            t_buttonSoundOff = nullptr;
    libTexture *            t_button = nullptr;
    libTexture *            t_buttonWide = nullptr;
    libTexture *            t_buttonSmall = nullptr;
    libTexture *            t_checkMark = nullptr;
    libTexture *            t_locked = nullptr;
    libSound *              s_buttonClick = nullptr;

    libButton               buttonSound;
    libButton               buttonHelp;
    libButton               buttonPlay;
    libButton               buttonCustom;
    libButton               buttonEditor;
    libButton               buttonCredits;
    libButton               buttonExit;
    libButton               buttonNew;
    libButton               buttonLoad;
    libButton               buttonBack;
    libButton               buttonPrev;
    libButton               buttonNext;

    libList<libButton>      episodeButtons;
    libList<libButton>      levelButtons;
};

#endif // !__MENU_H__
