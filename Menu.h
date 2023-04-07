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

#ifndef __MENU_H__
#define __MENU_H__

#include "Main.h"
#include "Episode.h"

#define BUTTON_COLOR                    libColor()
#define BUTTON_HOVER_COLOR              libColor(0.75f, 0.75f, 0.75f)
#define BUTTON_PRESSED_COLOR            libColor(0.5f, 0.5f, 0.5f)

#define FONT_TITLE_SCALE_RATIO          0.083f
#define FONT_H1_SCALE_RATIO             0.0416f
#define FONT_H2_SCALE_RATIO             0.03125f
#define FONT_TEXT_SCALE_RATIO           0.015625f

#define EPISODE_INDENT                  0.025f
#define EPISODES_HEIGHT                 0.53f
#define BUTTON_LEVEL_SCALE_RATIO        0.55f
#define BUTTON_MAIN_SCALE_RATIO         0.083f
#define BUTTON_MAIN_WIDTH_SCALE_RATIO   4.0f
#define BUTTON_TEXT_SCALE_RATIO         0.5f
#define BUTTON_LEVEL_TEXT_SCALE_RATIO   0.3f
#define BUTTON_SOUND_SCALE_RATIO        0.066f
#define BUTTON_HELP_SCALE_RATIO         0.04f
#define LOCKED_SCALE_RATIO              0.5f
#define CHECKMARK_SCALE_RATIO           0.5f

#define NUM_OF_EPISODES                 8
#define EPISODE_COLUMNS                 2
#define EPISODE_ROWS                    4
#define CREDITS_SPEED                   25.0f

class Game;

/*
===========================================================

    Menu

===========================================================
*/
class Menu
{
public:

    enum state_t
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
        HIDDEN
    };

                            Menu();
    
    bool                    Init();
    bool                    Draw();
    bool                    Update(Game *game = nullptr);

    void                    SetState(state_t state);
    state_t                 State() const { return state; }

private:

    void                    DrawMainMenu();
    void                    DrawEpisodes();
    void                    DrawLevels();
    void                    DrawEditor();
    void                    DrawCredits();
    void                    DrawHelp();
    void                    UpdateMainMenu();
    void                    UpdateEpisodes();
    void                    UpdateLevels();
    void                    UpdateEditor();
    void                    UpdateCredits();
    void                    UpdateHelp();

    Game *                  game;

    state_t                 state;
    int                     currentPage;

    Episode                 episode;
    libList<libStr>         episodePaths;
    libList<libStr>         episodeNames;
    libList<bool>           finishedEpisodeList;
    
    libVec2                 buttonMainSize;
    libVec2                 levelButtonIndent;
    libVec2                 levelButtonSize;
    libVec2                 creditsStart;
    float                   creditsPos;
    float                   creditsSize;
    size_t                  gameHelpLines;
    size_t                  editorHelpLines;
    size_t                  creditsLines;

    libPtr<libTexture>      tex_buttonSoundOn;
    libPtr<libTexture>      tex_buttonSoundOff;
    libPtr<libTexture>      tex_button;
    libPtr<libTexture>      tex_buttonWide;
    libPtr<libTexture>      tex_buttonSmall;
    libPtr<libTexture>      tex_checkMark;
    libPtr<libTexture>      tex_locked;
    libPtr<libSound>        snd_buttonClick;

    libPtr<libMesh>         mesh_locked;
    libPtr<libMesh>         mesh_checkMark;

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
