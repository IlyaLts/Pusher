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
    "Levels/Thinking_Rabbit.lvl",
    "Levels/Boxxle_I.lvl",
    "Levels/Boxxle_II.lvl",
    "Levels/Cosmos.lvl",
    "Levels/Sharpen.lvl",
    "Levels/Sokomania.lvl",
    "Levels/Monde.lvl",
    "Levels/Atlas.lvl"
};

const char *gameHelp = "LMB\n"
                       "W / Up\n"
                       "A / Left\n"
                       "S / Down\n"
                       "D / Right\n"
                       "CTRL + Z\n"
                       "CTRL + R\n"
                       "CTRL + [\n"
                       "CTRL + ]\n"
                       "F12";

const char *gameHelp2 = "Move to mouse cursor position.\n"
                        "Move the robot up.\n"
                        "Move the robot left.\n"
                        "Move the robot down.\n"
                        "Move the robot right.\n"
                        "Undo the latest move.\n"
                        "Restart the current level.\n"
                        "Previous level.\n"
                        "Next level.\n"
                        "Take a screenshot.";

const char *editorHelp = "LMB\n"
                         "RMB\n"
                         "SCROLL\n"
                         "UP\n"
                         "DOWN\n"
                         "LEFT\n"
                         "RIGHT\n"
                         "CTRL + A\n"
                         "CTRL + S\n"
                         "CTRL + D\n"
                         "CTRL + Q\n"
                         "CTRL + E\n"
                         "CTRL + C\n"
                         "CTRL + Z\n"
                         "CTRL + G\n"
                         "CTRL + [\n"
                         "CTRL + ]";

const char *editorHelp2 = "Add a block.\n"
                          "Remove a block.\n"
                          "Change a block type of the brush.\n"
                          "Shift blocks to the up.\n"
                          "Shift blocks to the down.\n"
                          "Shift blocks to the left.\n"
                          "Shift blocks to the right.\n"
                          "Add a new level.\n"
                          "Save the episode.\n"
                          "Delete the level.\n"
                          "Shift the level in the episode to the start.\n"
                          "Shift the level in the episode to the end.\n"
                          "Make a copy of the level.\n"
                          "Undo the latest change.\n"
                          "Switch to game/editor.\n"
                          "Previous level.\n"
                          "Next level.";

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
    LIB_CHECK(engine->Get(tex_buttonSoundOn.Get(), DATA_PACK "Textures/SoundOn.tga"));
    LIB_CHECK(engine->Get(tex_buttonSoundOff.Get(), DATA_PACK "Textures/SoundOff.tga"));
    LIB_CHECK(engine->Get(tex_button.Get(), DATA_PACK "Textures/Button.tga"));
    LIB_CHECK(engine->Get(tex_buttonWide.Get(), DATA_PACK "Textures/ButtonWide.tga"));
    LIB_CHECK(engine->Get(tex_buttonSmall.Get(), DATA_PACK "Textures/ButtonSmall.tga"));
    LIB_CHECK(engine->Get(tex_checkMark.Get(), DATA_PACK "Textures/CheckMark.tga"));
    LIB_CHECK(engine->Get(tex_locked.Get(), DATA_PACK "Textures/Locked.tga"));
    LIB_CHECK(engine->Get(snd_buttonClick.Get(), DATA_PACK "Sounds/ButtonClick.wav"));

    engine->Get(mesh_locked.Get());
    engine->Get(mesh_checkMark.Get());

    libColorState_t buttonColor = { BUTTON_COLOR, BUTTON_HOVER_COLOR, BUTTON_PRESSED_COLOR };

    buttonSound.SetColor(buttonColor);
    buttonSound.SetTexture(engine->State(LIB_AUDIO_VOLUME) ? tex_buttonSoundOn.Get() : tex_buttonSoundOff.Get());
    buttonSound.SetClickSound(snd_buttonClick.Get());

    buttonHelp.SetColor(buttonColor);
    buttonHelp.SetTexture(tex_buttonSmall.Get());
    buttonHelp.SetFont(font);
    buttonHelp.SetText(L"?");
    buttonHelp.SetTextScale(BUTTON_TEXT_SCALE_RATIO);
    buttonHelp.SetClickSound(snd_buttonClick.Get());

    buttonPrev.SetColor(buttonColor);
    buttonPrev.SetTexture(tex_button.Get());
    buttonPrev.SetFont(font);
    buttonPrev.SetText(L"<");
    buttonPrev.SetTextScale(BUTTON_TEXT_SCALE_RATIO);
    buttonPrev.SetClickSound(snd_buttonClick.Get());
    buttonPrev.SetBordersInside(true);

    buttonNext.SetColor(buttonColor);
    buttonNext.SetTexture(tex_button.Get());
    buttonNext.SetFont(font);
    buttonNext.SetText(L">");
    buttonNext.SetTextScale(BUTTON_TEXT_SCALE_RATIO);
    buttonNext.SetClickSound(snd_buttonClick.Get());
    buttonNext.SetBordersInside(true);

    buttonBack.SetColor(buttonColor);
    buttonBack.SetTexture(tex_button.Get());
    buttonBack.SetFont(font);
    buttonBack.SetText(L"Back");
    buttonBack.SetTextScale(BUTTON_TEXT_SCALE_RATIO);
    buttonBack.SetClickSound(snd_buttonClick.Get());
    buttonBack.SetBordersInside(true);

    buttonPlay.SetColor(buttonColor);
    buttonPlay.SetTexture(tex_button.Get());
    buttonPlay.SetFont(font);
    buttonPlay.SetText(L"Play");
    buttonPlay.SetClickSound(snd_buttonClick.Get());
    buttonPlay.SetTextScale(BUTTON_TEXT_SCALE_RATIO);
    buttonPlay.SetBordersInside(true);

    buttonCustom.SetColor(buttonColor);
    buttonCustom.SetTexture(tex_button.Get());
    buttonCustom.SetFont(font);
    buttonCustom.SetText(L"Custom");
    buttonCustom.SetClickSound(snd_buttonClick.Get());
    buttonCustom.SetTextScale(BUTTON_TEXT_SCALE_RATIO);
    buttonCustom.SetBordersInside(true);

    buttonEditor.SetColor(buttonColor);
    buttonEditor.SetTexture(tex_button.Get());
    buttonEditor.SetFont(font);
    buttonEditor.SetText(L"Editor");
    buttonEditor.SetTextScale(BUTTON_TEXT_SCALE_RATIO);
    buttonEditor.SetClickSound(snd_buttonClick.Get());
    buttonEditor.SetBordersInside(true);

    buttonNew.SetColor(buttonColor);
    buttonNew.SetTexture(tex_button.Get());
    buttonNew.SetFont(font);
    buttonNew.SetText(L"New");
    buttonNew.SetTextScale(BUTTON_TEXT_SCALE_RATIO);
    buttonNew.SetClickSound(snd_buttonClick.Get());
    buttonNew.SetBordersInside(true);

    buttonLoad.SetColor(buttonColor);
    buttonLoad.SetTexture(tex_button.Get());
    buttonLoad.SetFont(font);
    buttonLoad.SetText(L"Load");
    buttonLoad.SetTextScale(BUTTON_TEXT_SCALE_RATIO);
    buttonLoad.SetClickSound(snd_buttonClick.Get());
    buttonLoad.SetBordersInside(true);

    buttonCredits.SetColor(buttonColor);
    buttonCredits.SetTexture(tex_button.Get());
    buttonCredits.SetFont(font);
    buttonCredits.SetText(L"Credits");
    buttonCredits.SetTextScale(BUTTON_TEXT_SCALE_RATIO);
    buttonCredits.SetClickSound(snd_buttonClick.Get());
    buttonCredits.SetBordersInside(true);

    buttonExit.SetColor(buttonColor);
    buttonExit.SetTexture(tex_button.Get());
    buttonExit.SetFont(font);
    buttonExit.SetText(L"Exit");
    buttonExit.SetTextScale(BUTTON_TEXT_SCALE_RATIO);
    buttonExit.SetClickSound(snd_buttonClick.Get());
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
    if (state == HIDDEN)
        return false;

    buttonMainSize.y = libMath::Floor(screenSize.y * BUTTON_MAIN_SCALE_RATIO);
    buttonMainSize.x = libMath::Floor(buttonMainSize.y * BUTTON_MAIN_WIDTH_SCALE_RATIO);

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
        DrawMainMenu();
        break;
    }
    case PLAY_EPISODES:
    case CUSTOM_EPISODES:
    case EDITOR_EPISODES:
    {
        DrawEpisodes();
        break;
    }
    case PLAY_LEVELS:
    case CUSTOM_LEVELS:
    case EDITOR_LEVELS:
    {
        DrawLevels();
        break;
    }
    case EDITOR:
    {
        DrawEditor();
        break;
    }
    case CREDITS:
    {
        DrawCredits();
        break;
    }
    case HELP:
    {
        DrawHelp();
        break;
    }
    }

    return true;
}

/*
===================
Menu::Update
===================
*/
bool Menu::Update(Game *game)
{
    this->game = game;

    switch (state)
    {
    case MAIN_MENU:
    {
        UpdateMainMenu();
        break;
    }
    case PLAY_EPISODES:
    case CUSTOM_EPISODES:
    case EDITOR_EPISODES:
    {
        UpdateEpisodes();
        break;
    }
    case PLAY_LEVELS:
    case CUSTOM_LEVELS:
    case EDITOR_LEVELS:
    {
        UpdateLevels();
        break;
    }
    case EDITOR:
    {
        UpdateEditor();
        break;
    }
    case CREDITS:
    {
        UpdateCredits();
        break;
    }
    case HELP:
    {
        UpdateHelp();
        break;
    }
    }

    return true;
}

/*
===================
Menu::SetState
===================
*/
void Menu::SetState(state_t state)
{
    this->state = state;

    if (state == PLAY_LEVELS || state == CUSTOM_LEVELS || state == EDITOR_LEVELS)
        engine->SetState(LIB_WINDOW_TITLE, libFormat("Pusher - %s", episode.name.Get()));
    else
        engine->SetState(LIB_WINDOW_TITLE, "Pusher");
}

/*
===================
Menu::DrawMainMenu
===================
*/
void Menu::DrawMainMenu()
{
    // Title
    font->SetColor(LIB_COLOR_WHITE);
    font->SetAlign(LIB_CENTER);
    font->SetSize(libCast<int>(screenSize.y *FONT_TITLE_SCALE_RATIO));
    font->Print2D(screenSize.x / 2.0f, screenSize.y / 4.0f, "Pusher");

    // Sound button
    buttonSound.SetSize(screenSize.y *BUTTON_SOUND_SCALE_RATIO, screenSize.y *BUTTON_SOUND_SCALE_RATIO);
    buttonSound.SetPosition(screenSize.x - buttonSound.size.x * 0.75f, buttonSound.size.y * 0.75f);
    buttonSound.Draw();

    // Help button
    buttonHelp.SetSize(screenSize.y *BUTTON_HELP_SCALE_RATIO, screenSize.y *BUTTON_HELP_SCALE_RATIO);
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
}

/*
===================
Menu::DrawEpisodes
===================
*/
void Menu::DrawEpisodes()
{
    float offsetY = 0.0f;
    int columns = EPISODE_COLUMNS;
    int rows = EPISODE_ROWS;
    size_t numOfEpisodes = episodePaths.Size();
    int numOfEpisodesOnPage = (columns * rows);
    int currentBottomEpisode = numOfEpisodesOnPage * currentPage;

    libVec2 indent(screenSize.y *EPISODE_INDENT, screenSize.y *EPISODE_INDENT);
    libVec2 size((screenSize.x - (indent.x * (columns + 1))) / columns, ((screenSize.y *EPISODES_HEIGHT) - (indent.y * (rows + 1))) / rows);

    mesh_checkMark->Clear();

    // Reduces the page number when a user enlarges the window, since more episodes can fit on one page.
    /*while (currentBottomEpisode >= numOfEpisodes)
    {
        currentPage--;
        currentBottomEpisode = numOfEpisodesOnPage * currentPage;
    }*/

    // Title
    font->SetColor(LIB_COLOR_WHITE);
    font->SetAlign(LIB_TOP | LIB_HCENTER);
    font->SetSize(libCast<int>(screenSize.y *FONT_H1_SCALE_RATIO));
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
            libVec2 checkMarkSize(episodeButtons[i].size.y * CHECKMARK_SCALE_RATIO, episodeButtons[i].size.y * CHECKMARK_SCALE_RATIO);
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
    engine->Draw(mesh_checkMark.Get(), tex_checkMark.Get(), true);
}

/*
===================
Menu::DrawLevels
===================
*/
void Menu::DrawLevels()
{
    levelButtonIndent.Set(32.0f, 32.0f);
    levelButtonSize.Set(72.0f, 72.0f);

    float offsetY = 0.0f;
    int columns = libCast<int>((screenSize.x - levelButtonIndent.x) / (levelButtonSize.x + levelButtonIndent.x));
    int rows = libCast<int>((screenSize.y * BUTTON_LEVEL_SCALE_RATIO) / (levelButtonSize.y + levelButtonIndent.y));
    int numOfLevelsOnPage = columns * rows;
    int currentBottomLevel = numOfLevelsOnPage * currentPage;

    mesh_locked->Clear();
    mesh_checkMark->Clear();

    // Title
    font->SetColor(LIB_COLOR_WHITE);
    font->SetAlign(LIB_TOP | LIB_HCENTER);
    font->SetSize(libCast<int>(screenSize.y *FONT_H1_SCALE_RATIO));
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
            libVec2 lockedSize(levelButtons[i].size.x * LOCKED_SCALE_RATIO, levelButtons[i].size.y * LOCKED_SCALE_RATIO);
            libQuad q_locked(libVertex(), libVertex(lockedSize, libVec2(1.0f, 1.0f)));

            libVec2 checkMarkSize(levelButtons[i].size.x * CHECKMARK_SCALE_RATIO, levelButtons[i].size.y * CHECKMARK_SCALE_RATIO);
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
    engine->Draw(mesh_locked.Get(), tex_locked.Get(), true);
    engine->Draw(mesh_checkMark.Get(), tex_checkMark.Get(), true);
}

/*
===================
Menu::DrawEditor
===================
*/
void Menu::DrawEditor()
{
    float y = 0.0f;

    font->SetAlign(LIB_TOP | LIB_HCENTER);
    font->SetColor(LIB_COLOR_WHITE);

    // Title
    font->SetSize(libCast<int>(screenSize.y *FONT_H1_SCALE_RATIO));
    y += font->LineSpacing() / 4.0f;
    font->Print2D(screenSize.x / 2.0f, y, "Editor Controls");
    y += font->LineSpacing();

    // Editor keybindings
    font->SetAlign(LIB_HCENTER | LIB_TOP);
    font->SetSize(libCast<int>(((screenSize.y - y) *FONT_TEXT_SCALE_RATIO)));
    font->Print2D(screenSize.x * 0.25f, y, editorHelp);
    font->Print2D(screenSize.x * 0.75f, y, editorHelp2);

    buttonNew.SetSize(buttonMainSize.x, buttonMainSize.y);
    buttonNew.SetPosition(screenSize.x / 2.0f, buttonBack.pos.y - buttonBack.size.y * 2.5f);
    buttonLoad.SetSize(buttonMainSize.x, buttonMainSize.y);
    buttonLoad.SetPosition(screenSize.x / 2.0f, buttonBack.pos.y - buttonBack.size.y * 1.5f);

    buttonNew.Draw();
    buttonLoad.Draw();
    buttonBack.Draw();
}

/*
===================
Menu::DrawCredits
===================
*/
void Menu::DrawCredits()
{
    creditsSize = 0.0f;
    creditsStart.Set(screenSize.x / 2.0f, screenSize.y / 4.0f);
    float y = creditsStart.y + creditsPos;

    font->SetAlign(LIB_CENTER);
    font->SetColor(LIB_COLOR_WHITE);

    // Title
    font->SetSize(libCast<int>(screenSize.y * FONT_TITLE_SCALE_RATIO));
    font->Print2D(creditsStart.x, y + creditsSize, "Pusher");
    creditsSize += font->LineSpacing() / 2.0f;

    // Version
    font->SetSize(libCast<int>(screenSize.y * FONT_TEXT_SCALE_RATIO));
    font->Print2D(creditsStart.x, y + creditsSize, libFormat("Version: %s", PUSHER_VERSION));
    creditsSize += font->LineSpacing() * 2.0f;

    font->SetAlign(LIB_TOP | LIB_HCENTER);

    // Credits
    font->SetSize(libCast<int>(screenSize.y * FONT_H2_SCALE_RATIO));
    font->Print2D(creditsStart.x, y + creditsSize, credits);
    creditsSize += font->LineSpacing() * creditsLines;

    // Copyright
    font->SetSize(libCast<int>(screenSize.y * FONT_TEXT_SCALE_RATIO));
    font->Print2D(creditsStart.x, y + creditsSize, "Copyright (C) 2015 Ilya Lyakhovets");
}

/*
===================
Menu::DrawHelp
===================
*/
void Menu::DrawHelp()
{
    float y = 0.0f;

    font->SetAlign(LIB_TOP | LIB_HCENTER);
    font->SetColor(LIB_COLOR_WHITE);

    // Title
    font->SetSize(libCast<int>(screenSize.y *FONT_H1_SCALE_RATIO));
    y += font->LineSpacing() / 4.0f;
    font->Print2D(screenSize.x / 2.0f, y, "Game Controls");
    y += font->LineSpacing() * 1.25f;

    // Game keybindings
    font->SetAlign(LIB_HCENTER | LIB_TOP);
    font->SetSize(libCast<int>((screenSize.y *FONT_TEXT_SCALE_RATIO)));
    font->Print2D(screenSize.x * 0.25f, y, gameHelp);
    font->Print2D(screenSize.x * 0.75f, y, gameHelp2);

    buttonBack.Draw();
}

/*
===================
Menu::UpdateMainMenu
===================
*/
void Menu::UpdateMainMenu()
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
            buttonSound.SetTexture(tex_buttonSoundOff.Get());
            engine->SetState(LIB_AUDIO_VOLUME, 0.0f);
            cfg.SetBool("SoundEnabled", false);
        }
        else
        {
            buttonSound.SetTexture(tex_buttonSoundOn.Get());
            engine->SetState(LIB_AUDIO_VOLUME, 1.0f);
            cfg.SetBool("SoundEnabled", true);
        }
    }

    // Help
    if (buttonHelp.IsReleased())
        SetState(HELP);

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
            SetState(PLAY_EPISODES);

            for (int i = 0; i < NUM_OF_EPISODES; i++)
                episodePaths.Append(libStr(DATA_PACK) + libStr(builtInEpisodePaths[i]));
        }
        else if (buttonCustom.IsReleased())
        {
            SetState(CUSTOM_EPISODES);
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
            episodeButtons.Last().SetTexture(tex_buttonWide.Get());
            episodeButtons.Last().SetClickSound(snd_buttonClick.Get());
            episodeButtons.Last().SetFont(font);
            episodeButtons.Last().SetText(char2wchar(episodeNames.Last().Get()));
            episodeButtons.Last().SetTextScale(BUTTON_TEXT_SCALE_RATIO);
        }
    }

    // Editor
    if (buttonEditor.IsReleased())
        SetState(EDITOR);

    // Credits
    if (buttonCredits.IsReleased())
    {
        creditsPos = 0.0f;
        SetState(CREDITS);
    }

    // Exit
    if (buttonExit.IsReleased())
        engine->Stop();
}

/*
===================
Menu::UpdateEpisodes
===================
*/
void Menu::UpdateEpisodes()
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

                for (int j = 0; j < episode.levels.Size(); j++)
                {
                    libColorState_t buttonColor = { BUTTON_COLOR, BUTTON_HOVER_COLOR, BUTTON_PRESSED_COLOR };

                    levelButtons.Append(libButton());
                    levelButtons.Last().SetColor(buttonColor);
                    levelButtons.Last().SetTexture(tex_buttonSmall.Get());
                    levelButtons.Last().SetClickSound(snd_buttonClick.Get());
                    levelButtons.Last().SetFont(font);
                    levelButtons.Last().SetText(libWFormat(L"%d", j + 1));
                    levelButtons.Last().SetTextScale(BUTTON_LEVEL_TEXT_SCALE_RATIO);
                }

                if (state == PLAY_EPISODES)
                    SetState(PLAY_LEVELS);
                else if (state == CUSTOM_EPISODES)
                    SetState(CUSTOM_LEVELS);
                else if (state == EDITOR_EPISODES)
                    SetState(EDITOR_LEVELS);
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

    if (buttonBack.IsReleased() || engine->IsKeyPressed(LIBK_ESCAPE))
        SetState((state != EDITOR_EPISODES) ? MAIN_MENU : EDITOR);
}

/*
===================
Menu::UpdateLevels
===================
*/
void Menu::UpdateLevels()
{
    int columns = libCast<int>((screenSize.x - levelButtonIndent.x) / (levelButtonSize.x + levelButtonIndent.x));
    int rows = libCast<int>((screenSize.y * BUTTON_LEVEL_SCALE_RATIO) / (levelButtonSize.y + levelButtonIndent.y));
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

            SetState(HIDDEN);
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

        if (state == PLAY_LEVELS)
            SetState(PLAY_EPISODES);
        else if (state == CUSTOM_LEVELS)
            SetState(CUSTOM_EPISODES);
        else if (state == EDITOR_LEVELS)
            SetState(EDITOR_EPISODES);
    }
}

/*
===================
Menu::UpdateEditor
===================
*/
void Menu::UpdateEditor()
{
    buttonNew.Update();
    buttonLoad.Update();
    buttonBack.Update();

    // A new episode
    if (buttonNew.IsReleased())
    {
        SetState(HIDDEN);

        if (game)
        {
            game->Reset();
            game->SetState(Game::EDITOR);
        }
    }

    // Loads custom episodes
    if (buttonLoad.IsReleased())
    {
        SetState(EDITOR_EPISODES);
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
            episodeButtons.Last().SetTexture(tex_button.Get());
            episodeButtons.Last().SetTexture(tex_buttonWide.Get());
            episodeButtons.Last().SetClickSound(snd_buttonClick.Get());
            episodeButtons.Last().SetFont(font);
            episodeButtons.Last().SetText(char2wchar(episodeNames.Last().Get()));
            episodeButtons.Last().SetTextScale(BUTTON_TEXT_SCALE_RATIO);
        }
    }

    if (buttonBack.IsReleased() || engine->IsKeyPressed(LIBK_ESCAPE))
        SetState(MAIN_MENU);
}

/*
===================
Menu::UpdateCredits
===================
*/
void Menu::UpdateCredits()
{
    creditsPos -= CREDITS_SPEED * engine->State(LIB_DELTA_TIME);
    creditsStart.Set(screenSize.x / 2.0f, screenSize.y / 4.0f);

    // Returns to the main menu if any key is pressed or credits are over
    if ((engine->CurrentKey() && !engine->MouseWheel()) || (creditsStart.y + creditsSize + creditsPos < 0.0f))
        SetState(MAIN_MENU);
}

/*
===================
Menu::UpdateHelp
===================
*/
void Menu::UpdateHelp()
{
    buttonBack.Update();

    if (buttonBack.IsReleased() || engine->IsKeyPressed(LIBK_ESCAPE))
        SetState(MAIN_MENU);
}
