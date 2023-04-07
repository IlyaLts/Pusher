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

#include "Main.h"
#include "Menu.h"
#include "Game.h"

const char *builtInEpisodePaths[NUM_OF_EPISODES] =
{
    DATA_PACK "Levels/Thinking_Rabbit.lvl",
    DATA_PACK "Levels/Boxxle_I.lvl",
    DATA_PACK "Levels/Boxxle_II.lvl",
    DATA_PACK "Levels/Cosmos.lvl",
    DATA_PACK "Levels/Sharpen.lvl",
    DATA_PACK "Levels/Sokomania.lvl",
    DATA_PACK "Levels/Monde.lvl",
    DATA_PACK "Levels/Atlas.lvl"
};

const char *gameHelp = "LMB         - MoveRobot the robot to mouse cursor position.\n"
                       "W / Up      - MoveRobot the robot up.\n"
                       "A / Left    - MoveRobot the robot left.\n"
                       "S / Down    - MoveRobot the robot down.\n"
                       "D / Right   - MoveRobot the robot right.\n"
                       "CTRL + Z    - Undo the latest move.\n"
                       "CTRL + R    - Restart the current level.\n"
                       "CTRL + [    - Previous level.\n"
                       "CTRL + ]    - Next level.\n"
                       "F12         - Take a screenshot.";

const char *editorHelp = "LMB       - Add a block.\n"
                         "RMB       - Remove a block.\n"
                         "SCROLL    - Change a block type of the brush.\n"
                         "UP        - Shift blocks to the up.\n"
                         "DOWN      - Shift blocks to the down.\n"
                         "LEFT      - Shift blocks to the left.\n"
                         "RIGHT     - Shift blocks to the right.\n"
                         "CTRL + A  - Add a new level.\n"
                         "CTRL + S  - Save the current episode.\n"
                         "CTRL + D  - Delete the current level.\n"
                         "CTRL + Q  - Shift the current level in the current episode to the start.\n"
                         "CTRL + E  - Shift the current level in the current episode to the end.\n"
                         "CTRL + C  - Make a copy of the current level.\n"
                         "CTRL + Z  - Undo the latest change.\n"
                         "CTRL + G  - Switch to game/editor.\n"
                         "CTRL + [  - Previous level.\n"
                         "CTRL + ]  - Next level.";

const char *credits = "Game Developer\n"
                      "Ilya Lyakhovets\n"
                      "\n"
                      "Thinking Rabbit episode\n"
                      "by Thinking Rabbit, Inc.\n"
                      "\n"
                      "Boxxle episodes\n"
                      "by Thinking Rabbit, Inc.\n"
                      "\n"
                      "Cosmos episode\n"
                      "by Aymeric du Peloux\n"
                      "\n"
                      "Sharpen episode\n"
                      "by Sven Egevad\n"
                      "\n"
                      "Sokomania episode\n"
                      "by Thomas Reinke\n"
                      "\n"
                      "Monde episode\n"
                      "by Ghislain Martin\n"
                      "\n"
                      "Atlas episode\n"
                      "by Eric F. Tchong\n";

/*
===================
Menu::Menu
===================
*/
Menu::Menu()
{
    state = MAIN_MENU;

    gameHelpLines = libStr::Lines(gameHelp);
    editorHelpLines = libStr::Lines(editorHelp);
    creditsLines = libStr::Lines(credits);
}

/*
===================
Menu::Init
===================
*/
bool Menu::Init()
{
    LIB_CHECK(engine->Get(tex_buttonSoundOn, DATA_PACK "Textures/SoundOn.tga"));
    LIB_CHECK(engine->Get(tex_buttonSoundOff, DATA_PACK "Textures/SoundOff.tga"));
    LIB_CHECK(engine->Get(tex_button, DATA_PACK "Textures/Button.tga"));
    LIB_CHECK(engine->Get(tex_buttonWide, DATA_PACK "Textures/ButtonWide.tga"));
    LIB_CHECK(engine->Get(tex_buttonSmall, DATA_PACK "Textures/ButtonSmall.tga"));
    LIB_CHECK(engine->Get(tex_checkMark, DATA_PACK "Textures/CheckMark.tga"));
    LIB_CHECK(engine->Get(tex_locked, DATA_PACK "Textures/Locked.tga"));
    LIB_CHECK(engine->Get(snd_buttonClick, DATA_PACK "Sounds/ButtonClick.wav"));

    engine->Get(mesh_locked);
    engine->Get(mesh_checkMark);

    libColorState_t buttonColor = { BUTTON_COLOR, BUTTON_HOVER_COLOR, BUTTON_PRESSED_COLOR };

    buttonSound.SetColor(buttonColor);
    buttonSound.SetTexture(engine->State(LIB_AUDIO_VOLUME) ? tex_buttonSoundOn : tex_buttonSoundOff);
    buttonSound.SetClickSound(snd_buttonClick);

    buttonHelp.SetColor(buttonColor);
    buttonHelp.SetTexture(tex_buttonSmall);
    buttonHelp.SetFont(font);
    buttonHelp.SetText(L"?");
    buttonHelp.SetTextScale(BUTTON_TEXT_SCALE);
    buttonHelp.SetClickSound(snd_buttonClick);

    buttonPrev.SetColor(buttonColor);
    buttonPrev.SetTexture(tex_button);
    buttonPrev.SetFont(font);
    buttonPrev.SetText(L"<");
    buttonPrev.SetTextScale(BUTTON_TEXT_SCALE);
    buttonPrev.SetClickSound(snd_buttonClick);
    buttonPrev.SetBordersInside(true);

    buttonNext.SetColor(buttonColor);
    buttonNext.SetTexture(tex_button);
    buttonNext.SetFont(font);
    buttonNext.SetText(L">");
    buttonNext.SetTextScale(BUTTON_TEXT_SCALE);
    buttonNext.SetClickSound(snd_buttonClick);
    buttonNext.SetBordersInside(true);

    buttonBack.SetColor(buttonColor);
    buttonBack.SetTexture(tex_button);
    buttonBack.SetFont(font);
    buttonBack.SetText(L"Back");
    buttonBack.SetTextScale(BUTTON_TEXT_SCALE);
    buttonBack.SetClickSound(snd_buttonClick);
    buttonBack.SetBordersInside(true);

    buttonPlay.SetColor(buttonColor);
    buttonPlay.SetTexture(tex_button);
    buttonPlay.SetFont(font);
    buttonPlay.SetText(L"Play");
    buttonPlay.SetClickSound(snd_buttonClick);
    buttonPlay.SetTextScale(BUTTON_TEXT_SCALE);
    buttonPlay.SetBordersInside(true);

    buttonCustom.SetColor(buttonColor);
    buttonCustom.SetTexture(tex_button);
    buttonCustom.SetFont(font);
    buttonCustom.SetText(L"Custom");
    buttonCustom.SetClickSound(snd_buttonClick);
    buttonCustom.SetTextScale(BUTTON_TEXT_SCALE);
    buttonCustom.SetBordersInside(true);

    buttonEditor.SetColor(buttonColor);
    buttonEditor.SetTexture(tex_button);
    buttonEditor.SetFont(font);
    buttonEditor.SetText(L"Editor");
    buttonEditor.SetTextScale(BUTTON_TEXT_SCALE);
    buttonEditor.SetClickSound(snd_buttonClick);
    buttonEditor.SetBordersInside(true);

    buttonNew.SetColor(buttonColor);
    buttonNew.SetTexture(tex_button);
    buttonNew.SetFont(font);
    buttonNew.SetText(L"New");
    buttonNew.SetTextScale(BUTTON_TEXT_SCALE);
    buttonNew.SetClickSound(snd_buttonClick);
    buttonNew.SetBordersInside(true);

    buttonLoad.SetColor(buttonColor);
    buttonLoad.SetTexture(tex_button);
    buttonLoad.SetFont(font);
    buttonLoad.SetText(L"Load");
    buttonLoad.SetTextScale(BUTTON_TEXT_SCALE);
    buttonLoad.SetClickSound(snd_buttonClick);
    buttonLoad.SetBordersInside(true);

    buttonCredits.SetColor(buttonColor);
    buttonCredits.SetTexture(tex_button);
    buttonCredits.SetFont(font);
    buttonCredits.SetText(L"Credits");
    buttonCredits.SetTextScale(BUTTON_TEXT_SCALE);
    buttonCredits.SetClickSound(snd_buttonClick);
    buttonCredits.SetBordersInside(true);

    buttonExit.SetColor(buttonColor);
    buttonExit.SetTexture(tex_button);
    buttonExit.SetFont(font);
    buttonExit.SetText(L"Exit");
    buttonExit.SetTextScale(BUTTON_TEXT_SCALE);
    buttonExit.SetClickSound(snd_buttonClick);
    buttonExit.SetBordersInside(true);

    return true;
}

/*
===================
Menu::Draw
===================
*/
bool Menu::Draw()
{
    mesh_locked->Clear();
    mesh_checkMark->Clear();

    libVec2 buttonMainSize;
    buttonMainSize.y = libMath::Floor(screenSize.y * BUTTON_MAIN_SIZE);
    buttonMainSize.x = libMath::Floor(buttonMainSize.y * BUTTON_MAIN_WIDTH_RATIO);

    buttonPrev.SetSize(libMath::Floor(buttonMainSize.x / 2.0f), buttonMainSize.y);
    buttonPrev.SetPosition(libMath::Floor(screenSize.x / 2.0f - buttonPrev.size.x / 2.0f), screenSize.y - buttonMainSize.y * 2.5f);
    buttonNext.SetSize(libMath::Floor(buttonMainSize.x / 2.0f), buttonMainSize.y);
    buttonNext.SetPosition(libMath::Floor(screenSize.x / 2.0f + buttonNext.size.x / 2.0f), screenSize.y - buttonMainSize.y * 2.5f);
    buttonBack.SetSize(buttonMainSize.x, buttonMainSize.y);
    buttonBack.SetPosition(screenSize.x / 2.0f, screenSize.y - buttonMainSize.y);

    switch (state)
    {
    case MAIN_MENU:
    {
        // Title
        font->SetColor(LIB_COLOR_WHITE);
        font->SetAlign(LIB_CENTER);
        font->SetSize(libCast<int>(screenSize.y * FONT_MENU_TITLE_SIZE));
        font->Print2D(screenSize.x / 2.0f, screenSize.y / 4.0f, "Pusher");

        // Sound button
        buttonSound.SetSize(screenSize.y * BUTTON_SOUND_SIZE, screenSize.y * BUTTON_SOUND_SIZE);
        buttonSound.SetPosition(screenSize.x - buttonSound.size.x * 0.75f, buttonSound.size.y * 0.75f);
        buttonSound.Draw();

        // Help button
        buttonHelp.SetSize(screenSize.y * BUTTON_HELP_SIZE, screenSize.y * BUTTON_HELP_SIZE);
        buttonHelp.SetPosition(screenSize.x - buttonHelp.size.x, screenSize.y - buttonHelp.size.y);
        buttonHelp.Draw();

        // Menus
        buttonPlay.SetSize(buttonMainSize.x, buttonMainSize.y);
        buttonPlay.SetPosition(screenSize.x / 2.0f, screenSize.y - buttonPlay.size.y * 5.0f);
        buttonPlay.Draw();

        buttonCustom.SetSize(buttonMainSize.x, buttonMainSize.y);
        buttonCustom.SetPosition(screenSize.x / 2.0f, screenSize.y - buttonCustom.size.y * 4.0f);
        buttonCustom.Draw();

        buttonEditor.SetSize(buttonMainSize.x, buttonMainSize.y);
        buttonEditor.SetPosition(screenSize.x / 2.0f, screenSize.y - buttonEditor.size.y * 3.0f);
        buttonEditor.Draw();

        buttonCredits.SetSize(buttonMainSize.x, buttonMainSize.y);
        buttonCredits.SetPosition(screenSize.x / 2.0f, screenSize.y - buttonCredits.size.y * 2.0f);
        buttonCredits.Draw();

        buttonExit.SetSize(buttonMainSize.x, buttonMainSize.y);
        buttonExit.SetPosition(screenSize.x / 2.0f, screenSize.y - buttonExit.size.y);
        buttonExit.Draw();

        break;
    }
    case PLAY_EPISODES:
    case CUSTOM_EPISODES:
    case EDITOR_EPISODES:
    {
        float offsetY = 0.0f;
        int columns = EPISODE_COLUMNS;
        int rows = EPISODE_ROWS;
        size_t numOfEpisodes = episodePaths.Size();
        int numOfEpisodesOnPage = (columns * rows);
        int currentBottomEpisode = numOfEpisodesOnPage * currentPage;

        libVec2 indent(screenSize.y * EPISODE_INDENT, screenSize.y * EPISODE_INDENT);
        libVec2 size((screenSize.x - (indent.x * (columns + 1))) / columns, ((screenSize.y * EPISODES_HEIGHT) - (indent.y * (rows + 1))) / rows);

        // Reduces the page number when a user enlarges the window, since more episodes can fit on one page.
        /*while (currentBottomEpisode >= numOfEpisodes)
        {
            currentPage--;
            currentBottomEpisode = numOfEpisodesOnPage * currentPage;
        }*/

        // Title
        font->SetColor(LIB_COLOR_WHITE);
        font->SetAlign(LIB_TOP | LIB_HCENTER);
        font->SetSize(libCast<int>(screenSize.y * FONT_MENU_H1_SIZE));
        offsetY += font->LineSpacing() / 2;
        font->Print2D(screenSize.x / 2.0f, offsetY, (state == PLAY_EPISODES) ? "Episodes" : "Custom Episodes");
        offsetY += font->LineSpacing();

        // Episodes
        for (int i = currentBottomEpisode; i < (currentBottomEpisode + numOfEpisodesOnPage) && i < numOfEpisodes; i++)
        {
            float x = indent.x * (i % columns + 1) + (size.x * (i % columns)) + size.x / 2;
            float y = indent.y * (i / columns % rows + 1) + (size.y * (i / columns % rows)) + size.y / 2 + offsetY;

            episodeButtons[i].Set(size.x, size.y, x, y);
            episodeButtons[i].Draw();

            // Draws a check mark if an episode was fully completed
            if (state != EDITOR_EPISODES && finishedEpisodeList[i])
            {
                libVec2 checkMarkSize(episodeButtons[i].size.y * CHECKMARK_SIZE, episodeButtons[i].size.y * CHECKMARK_SIZE);
                libQuad q_checkMark(libVertex(), libVertex(checkMarkSize, libVec2(1.0f, 1.0f)));
                mesh_checkMark->Add(q_checkMark, libVec3(x + episodeButtons[i].size.x / 2.0f - checkMarkSize.x * 2.0f, y - checkMarkSize.y / 2.0f, 0.0f));
            }
        }

        // Draws Previous/Next buttons only when there are more episodes than can fit on a single page
        if (numOfEpisodesOnPage < numOfEpisodes)
        {
            buttonPrev.Draw();
            buttonNext.Draw();
        }

        buttonBack.Draw();

        break;
    }
    case PLAY_LEVELS:
    case CUSTOM_LEVELS:
    case EDITOR_LEVELS:
    {
        levelButtonIndent.Set(32.0f, 32.0f);
        levelButtonSize.Set(64.0f, 64.0f);

        float offsetY = 0.0f;
        int columns = libCast<int>((screenSize.x - levelButtonIndent.x) / (levelButtonSize.x + levelButtonIndent.x));
        int rows = libCast<int>((screenSize.y * BUTTON_LEVEL_SIZE) / (levelButtonSize.y + levelButtonIndent.y));
        int numOfLevelsOnPage = columns * rows;
        int currentBottomLevel = numOfLevelsOnPage * currentPage;

        // Title
        font->SetColor(LIB_COLOR_WHITE);
        font->SetAlign(LIB_TOP | LIB_HCENTER);
        font->SetSize(libCast<int>(screenSize.y * FONT_MENU_H1_SIZE));
        offsetY += font->LineSpacing() / 2.0f;
        font->Print2D(screenSize.x / 2.0f, offsetY, "Levels");
        offsetY += font->LineSpacing();

        // Reduces the page number when a user enlarges the window, since more episodes can fit on one page.
        while (currentBottomLevel >= episode.levels.Size())
        {
            currentPage--;
            currentBottomLevel = numOfLevelsOnPage * currentPage;
        }

        // Levels
        for (int i = currentBottomLevel; i < (currentBottomLevel + numOfLevelsOnPage) && i < episode.levels.Size(); i++)
        {
            float indent = screenSize.x - (columns * (levelButtonSize.x + levelButtonIndent.x) - levelButtonIndent.x);
            float x = indent / 2.0f + levelButtonSize.x / 2.0f + ((levelButtonSize.x + levelButtonIndent.x) * ((i - currentBottomLevel) % columns));
            float y = levelButtonIndent.y + levelButtonSize.y / 2.0f + ((levelButtonSize.y + levelButtonIndent.y) * ((i - currentBottomLevel) / columns % rows)) + offsetY;

            // Makes font darken if a level is locked
            if (state != EDITOR_LEVELS && episode.unlocked.Get() < (i + 1))
                levelButtons[i].fontColor.base.Set(0.70f, 0.70f, 0.70f);

            levelButtons[i].Set(levelButtonSize.x, levelButtonSize.y, x, y);
            levelButtons[i].Draw();

            // Locks/check marks
            if (state != EDITOR_LEVELS)
            {
                libVec2 lockedSize(levelButtons[i].size.x * LOCKED_SIZE, levelButtons[i].size.y * LOCKED_SIZE);
                libQuad q_locked(libVertex(), libVertex(lockedSize, libVec2(1.0f, 1.0f)));

                libVec2 checkMarkSize(levelButtons[i].size.x * CHECKMARK_SIZE, levelButtons[i].size.y * CHECKMARK_SIZE);
                libQuad q_checkMark(libVertex(), libVertex(checkMarkSize, libVec2(1.0f, 1.0f)));

                if (episode.unlocked.Get() < (i + 1))
                {
                    mesh_locked->Add(q_locked, libVec3(x - lockedSize.x / 2, y - lockedSize.y / 2, 0.0f));
                }
                else if (episode.unlocked.Get() > (i + 1))
                {
                    mesh_checkMark->Add(q_checkMark, libVec3(x, y, 0.0f));
                }
            }
        }

        // Draws Previous/Next buttons only when there are more levels than can fit on a single page
        if (numOfLevelsOnPage < episode.levels.Size())
        {
            buttonPrev.Draw();
            buttonNext.Draw();
        }

        buttonBack.Draw();

        break;
    }
    case EDITOR:
    {
        float y = 0.0f;

        font->SetAlign(LIB_TOP | LIB_HCENTER);
        font->SetColor(LIB_COLOR_WHITE);

        // Title
        font->SetSize(libCast<int>(screenSize.y * FONT_MENU_H1_SIZE));
        y += font->LineSpacing() / 4.0f;
        font->Print2D(screenSize.x / 2.0f, y, "Editor Controls");
        y += font->LineSpacing();

        // Editor keybindings
        font->SetSize(libCast<int>(((screenSize.y - y) * FONT_MENU_TEXT_SIZE)));
        font->Print2D(screenSize.x / 2.0f, y, editorHelp);

        buttonNew.SetSize(buttonMainSize.x, buttonMainSize.y);
        buttonNew.SetPosition(screenSize.x / 2.0f, buttonBack.pos.y - buttonBack.size.y * 2.5f);
        buttonLoad.SetSize(buttonMainSize.x, buttonMainSize.y);
        buttonLoad.SetPosition(screenSize.x / 2.0f, buttonBack.pos.y - buttonBack.size.y * 1.5f);

        buttonNew.Draw();
        buttonLoad.Draw();
        buttonBack.Draw();

        break;
    }
    case CREDITS:
    {
        creditsSize = 0.0f;
        creditsStart.Set(screenSize.x / 2.0f, screenSize.y / 4.0f);
        float y = creditsStart.y + creditsPos;

        font->SetAlign(LIB_CENTER);
        font->SetColor(LIB_COLOR_WHITE);

        // Title
        font->SetSize(libCast<int>(screenSize.y * FONT_MENU_TITLE_SIZE));
        font->Print2D(creditsStart.x, y + creditsSize, "Pusher");
        creditsSize += font->LineSpacing() / 2.0f;

        // Version
        font->SetSize(libCast<int>(screenSize.y * FONT_MENU_TEXT_SIZE));
        font->Print2D(creditsStart.x, y + creditsSize, libFormat("Version: %s", PUSHER_VERSION));
        creditsSize += font->LineSpacing() * 2.0f;

        font->SetAlign(LIB_TOP | LIB_HCENTER);

        // Credits
        font->SetSize(libCast<int>(screenSize.y * FONT_MENU_H2_SIZE));
        font->Print2D(creditsStart.x, y + creditsSize, credits);
        creditsSize += font->LineSpacing() * creditsLines;

        // Copyright
        font->SetSize(libCast<int>(screenSize.y * FONT_MENU_TEXT_SIZE));
        font->Print2D(creditsStart.x, y + creditsSize, "Copyright (C) 2015-2023 Ilya Lyakhovets");

        break;
    }
    case HELP:
    {
        float y = 0.0f;

        font->SetAlign(LIB_TOP | LIB_HCENTER);
        font->SetColor(LIB_COLOR_WHITE);

        // Title
        font->SetSize(libCast<int>(screenSize.y * FONT_MENU_H1_SIZE));
        y += font->LineSpacing() / 4.0f;
        font->Print2D(screenSize.x / 2.0f, y, "Game Controls");
        y += font->LineSpacing();

        // Game keybindings
        font->SetSize(libCast<int>((screenSize.y * FONT_MENU_TEXT_SIZE)));
        font->Print2D(screenSize.x / 2.0f, y, gameHelp);

        buttonBack.Draw();
        break;
    }
    }

    engine->Draw(mesh_locked, tex_locked, true);
    engine->Draw(mesh_checkMark, tex_checkMark, true);

    return true;
}

/*
===================
Menu::Update
===================
*/
bool Menu::Update(Game *game)
{
    switch (state)
    {
    case MAIN_MENU:
    {
        buttonSound.Update();
        buttonHelp.Update();
        buttonPlay.Update();
        buttonCustom.Update();
        buttonEditor.Update();
        buttonCredits.Update();
        buttonExit.Update();

        // Sound
        if (buttonSound.IsReleased())
        {
            if (engine->State(LIB_AUDIO_VOLUME))
            {
                buttonSound.SetTexture(tex_buttonSoundOff);
                engine->SetState(LIB_AUDIO_VOLUME, 0.0f);
                cfg.SetBool("SoundEnabled", false);
            }
            else
            {
                buttonSound.SetTexture(tex_buttonSoundOn);
                engine->SetState(LIB_AUDIO_VOLUME, 1.0f);
                cfg.SetBool("SoundEnabled", true);
            }
        }

        // Help
        if (buttonHelp.IsReleased())
        {
            state = HELP;
            break;
        }

        // Play/Custom
        if (buttonPlay.IsReleased() || buttonCustom.IsReleased())
        {
            currentPage = 0;
            finishedEpisodeList.Clear();
            episodePaths.Clear();
            episodeNames.Clear();
            episodeButtons.Clear();

            if (buttonPlay.IsReleased())
            {
                state = PLAY_EPISODES;

                for (int i = 0; i < NUM_OF_EPISODES; i++)
                    episodePaths.Append(libStr(builtInEpisodePaths[i]));
            }
            else
            {
                state = CUSTOM_EPISODES;
                libDir::FindFiles(CUSTOM_LEVELS_PATH "*.lvl*", episodePaths);
            }

            for (auto &path : episodePaths)
            {
                libHolder<libInt32> unlocked;
                Episode::LoadNumberOfUnlockedLevels(unlocked, path.Get(), Episode::LoadNumberOfLevels(path.Get()));
                finishedEpisodeList.Append(unlocked.Get() > Episode::LoadNumberOfLevels(path.Get()));

                episodeNames.Append(libStr());
                path.ExtractBaseName(episodeNames.Last());
                episodeNames.Last().Replace("_", " ");

                libColorState_t buttonColor = { BUTTON_COLOR, BUTTON_HOVER_COLOR, BUTTON_PRESSED_COLOR };

                episodeButtons.Append(libButton());
                episodeButtons.Last().SetColor(buttonColor);
                episodeButtons.Last().SetTexture(tex_buttonWide);
                episodeButtons.Last().SetClickSound(snd_buttonClick);
                episodeButtons.Last().SetFont(font);
                episodeButtons.Last().SetText(char2wchar(episodeNames.Last().Get()));
                episodeButtons.Last().SetTextScale(BUTTON_TEXT_SCALE);
            }

            break;
        }

        // Editor
        if (buttonEditor.IsReleased())
        {
            state = EDITOR;
            break;
        }

        // Credits
        if (buttonCredits.IsReleased())
        {
            creditsPos = 0.0f;
            state = CREDITS;
            break;
        }

        // Exit
        if (buttonExit.IsReleased())
        {
            engine->Stop();
            break;
        }

        break;
    }
    case PLAY_EPISODES:
    case CUSTOM_EPISODES:
    case EDITOR_EPISODES:
    {
        int columns = EPISODE_COLUMNS;
        int rows = EPISODE_ROWS;
        int numOfEpisodesOnPage = columns * rows;
        int currentBottomEpisode = numOfEpisodesOnPage * currentPage;
        size_t numOfEpisodes = episodePaths.Size();

        // Episodes
        for (int i = currentBottomEpisode; i < (currentBottomEpisode + numOfEpisodesOnPage) && i < numOfEpisodes; i++)
        {
            episodeButtons[i].Update();

            if (episodeButtons[i].IsReleased())
            {
                if (Episode::Load(episode, episodePaths[i].Get()))
                {
                    currentPage = 0;
                    levelButtons.Clear();
                    engine->SetState(LIB_WINDOW_TITLE, libFormat("Pusher - %s", episode.name.Get()));

                    for (int j = 0; j < episode.levels.Size(); j++)
                    {
                        libColorState_t buttonColor = { BUTTON_COLOR, BUTTON_HOVER_COLOR, BUTTON_PRESSED_COLOR };

                        levelButtons.Append(libButton());
                        levelButtons.Last().SetColor(buttonColor);
                        levelButtons.Last().SetTexture(tex_buttonSmall);
                        levelButtons.Last().SetClickSound(snd_buttonClick);
                        levelButtons.Last().SetFont(font);
                        levelButtons.Last().SetText(libWFormat(L"%d", j + 1));
                        levelButtons.Last().SetTextScale(BUTTON_TEXT_SCALE);
                    }

                    if (state == PLAY_EPISODES)
                        state = PLAY_LEVELS;
                    else if (state == CUSTOM_EPISODES)
                        state = CUSTOM_LEVELS;
                    else if (state == EDITOR_EPISODES)
                        state = EDITOR_LEVELS;
                }

                break;
            }
        }

        // Previous/next buttons only appear when there are more episodes than we can fit on one page.
        if (numOfEpisodesOnPage < numOfEpisodes)
        {
            buttonPrev.Update();
            buttonNext.Update();

            if (buttonPrev.IsReleased() && currentPage)
                currentPage--;

            if (buttonNext.IsReleased() && (currentBottomEpisode + numOfEpisodesOnPage) < numOfEpisodes)
                currentPage++;
        }

        buttonBack.Update();

        // Returns to the previous menu
        if (buttonBack.IsReleased() || engine->IsKeyPressed(LIBK_ESCAPE))
        {
            state = (state != EDITOR_EPISODES) ? MAIN_MENU : EDITOR;
        }

        break;
    }
    case PLAY_LEVELS:
    case CUSTOM_LEVELS:
    case EDITOR_LEVELS:
    {
        int columns = libCast<int>((screenSize.x - levelButtonIndent.x) / (levelButtonSize.x + levelButtonIndent.x));
        int rows = libCast<int>((screenSize.y * BUTTON_LEVEL_SIZE) / (levelButtonSize.y + levelButtonIndent.y));
        int numOfLevelsOnPage = columns * rows;
        int currentBottomLevel = numOfLevelsOnPage * currentPage;

        // Levels
        for (int i = currentBottomLevel; i < (currentBottomLevel + numOfLevelsOnPage) && i < episode.levels.Size(); i++)
        {
            // Only enables buttons for levels that are unlocked or when in editor mode
            levelButtons[i].SetEnabled((i + 1) <= episode.unlocked.Get() || state == EDITOR_LEVELS);
            levelButtons[i].Update();

            if (levelButtons[i].IsReleased())
            {
                if (game)
                {
                    game->SetEpisode(episode);
                    game->SetState(state != EDITOR_LEVELS ? Game::IN_GAME : Game::EDITOR);
                    game->SetLevel(i + 1);
                }

                state = IN_GAME;
                break;
            }
        }

        // Previous/next buttons only appear when there are more levels than we can fit on one page.
        if (numOfLevelsOnPage < episode.levels.Size())
        {
            buttonPrev.Update();
            buttonNext.Update();

            if (buttonPrev.IsReleased() && currentPage)
                currentPage--;

            if (buttonNext.IsReleased() && (currentBottomLevel + numOfLevelsOnPage) < episode.levels.Size())
                currentPage++;
        }

        buttonBack.Update();

        // Returns to the previous menu
        if (buttonBack.IsReleased() || engine->IsKeyPressed(LIBK_ESCAPE))
        {
            currentPage = 0;
            engine->SetState(LIB_WINDOW_TITLE, "Pusher");

            if (state == PLAY_LEVELS)
                state = PLAY_EPISODES;
            else if (state == CUSTOM_LEVELS)
                state = CUSTOM_EPISODES;
            else if (state == EDITOR_LEVELS)
                state = EDITOR_EPISODES;
        }

        break;
    }
    case EDITOR:
    {
        buttonNew.Update();
        buttonLoad.Update();
        buttonBack.Update();

        // A new episode
        if (buttonNew.IsReleased())
        {
            state = IN_GAME;

            if (game)
            {
                game->Reset();
                game->SetState(Game::EDITOR);
            }
        }

        // Loads custom episodes
        if (buttonLoad.IsReleased())
        {
            state = EDITOR_EPISODES;
            currentPage = 0;
            episodePaths.Clear();
            episodeNames.Clear();
            episodeButtons.Clear();

            libDir::FindFiles(CUSTOM_LEVELS_PATH "*.lvl*", episodePaths);

            for (auto &path : episodePaths)
            {
                episodeNames.Append(libStr());
                path.ExtractBaseName(episodeNames.Last());
                episodeNames.Last().Replace("_", " ");

                libColorState_t buttonColor = { BUTTON_COLOR, BUTTON_HOVER_COLOR, BUTTON_PRESSED_COLOR };

                episodeButtons.Append(libButton());
                episodeButtons.Last().SetColor(buttonColor);
                episodeButtons.Last().SetTexture(tex_button);
                episodeButtons.Last().SetClickSound(snd_buttonClick);
                episodeButtons.Last().SetFont(font);
                episodeButtons.Last().SetText(char2wchar(episodeNames.Last().Get()));
            }
        }

        // Returns to the previous menu
        if (buttonBack.IsReleased() || engine->IsKeyPressed(LIBK_ESCAPE))
            state = MAIN_MENU;

        break;
    }
    case CREDITS:
    {
        creditsPos -= CREDITS_SPEED * engine->State(LIB_DELTA_TIME);
        creditsStart.Set(screenSize.x / 2.0f, screenSize.y / 4.0f);

        // Returns to the main menu if any key is pressed or credits are over
        if ((engine->CurrentKey() && !engine->MouseWheel()) || (creditsStart.y + creditsSize + creditsPos < 0.0f))
            state = MAIN_MENU;

        break;
    }
    case HELP:
    {
        buttonBack.Update();

        // Returns to the main menu
        if (buttonBack.IsReleased() || engine->IsKeyPressed(LIBK_ESCAPE))
            state = MAIN_MENU;

        break;
    }
    }

    return true;
}
