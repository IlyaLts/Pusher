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

#include "Main.h"
#include "Menu.h"
#include "Game.h"

libVec2i screenSize(WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT);
libVec2i gameScreen;
libVec2i gameScreenSize;
float aspectRatio;

libTexture *t_background = nullptr;
libFont *font = nullptr;

Menu menu;
Game game;
libCfg cfg;

/*
===================
Init
===================
*/
bool Init()
{
    LIB_CHECK(engine->GetTexture(t_background, DATA_PACK "Textures/Background.png"));
    LIB_CHECK(engine->GetFont(font, DATA_PACK "Font.ttf"));

    LIB_CHECK(menu.Init());
    LIB_CHECK(game.Init());

    font->SetColor(LIB_COLOR_WHITE);
    font->SetShadowShift(libVec2(1.0f, 1.0f));
    font->SetShadowType(libFont::SHADOW_ADDAPTIVE);

    engine->UnloadEverything();

    return true;
}

/*
===================
Draw
===================
*/
bool Draw()
{
    screenSize.Set(engine->GetState(LIB_SCREEN_WIDTH), engine->GetState(LIB_SCREEN_HEIGHT));

    // Adjusts the game screen according to our aspect ratio
    if (screenSize.x < screenSize.y)
    {
        gameScreenSize.Set(screenSize.x, libCast<int>(screenSize.x / aspectRatio));
        gameScreen.Set(0, (screenSize.y - gameScreenSize.y) / 2);
    }
    else
    {
        gameScreenSize.Set(libCast<int>(screenSize.y * aspectRatio), screenSize.y);
        gameScreen.Set((screenSize.x - gameScreenSize.x) / 2, 0);
    }

    libVec2 cell(libCast<float>(gameScreenSize.x / LEVEL_WIDTH), libCast<float>(gameScreenSize.y / LEVEL_HEIGHT));
    libVec2i offset(libCast<int>((gameScreenSize.x - cell.x * LEVEL_WIDTH) / 2.0f), libCast<int>((gameScreenSize.y - cell.y * LEVEL_HEIGHT) / 2.0f));
    gameScreen += offset;
    gameScreenSize -= offset;

    engine->ClearScreen(LIB_COLOR_BLACK);
    t_background->Draw2DQuad(libQuad(libVertex(), libVertex(libVec2(screenSize), libCast<float>(screenSize.x / t_background->GetWidth()), libCast<float>(screenSize.y / t_background->GetHeight()))));

    if (menu.GetState() != Menu::State::IN_GAME)
        menu.Draw();
    else
        game.Draw();

    return true;
}

/*
===================
Update
===================
*/
bool Update()
{
    if (engine->IsKeyPressed(LIBK_F12)) engine->TakeScreenshot();

    if (menu.GetState() != Menu::State::IN_GAME)
    {
        // If currently playing, closes the menu. Otherwise, quits the game.
        if (menu.GetState() == Menu::State::MAIN_MENU && engine->IsKeyPressed(LIBK_ESCAPE))
        {
            if (game.GetState() != Game::State::NONE)
                menu.SetState(Menu::State::IN_GAME);
            else
                engine->Stop();
        }

        menu.Update(&game);
    }
    else
    {
        game.Update();

        // Opens the main menu or exits editor play mode
        if (engine->IsKeyPressed(LIBK_ESCAPE))
        {
            if (game.GetState() == Game::State::EDITOR_PLAY)
            {
                game.SetState(Game::State::EDITOR);
            }
            else
            {
                menu.SetState(Menu::State::MAIN_MENU);
                engine->SetState(LIB_WINDOW_TITLE, "Pusher");
            }
        }

        if (game.IsCompleted())
        {
            game.Reset();
            menu.SetState(Menu::State::MAIN_MENU);
            engine->SetState(LIB_WINDOW_TITLE, "Pusher");
        }
    }

    return true;
}

/*
===================
Free
===================
*/
bool Free()
{
    cfg.SetBool("Maximalized", engine->GetState(LIB_WINDOW) == LIB_MAXIMALIZED);
    cfg.SetBool("Fullscreen", engine->GetState(LIB_VIDEO_MODE) == LIB_FULLSCREEN_WINDOW);
    cfg.SetInt("Width", engine->GetState(LIB_WINDOW_WIDTH));
    cfg.SetInt("Height", engine->GetState(LIB_WINDOW_HEIGHT));
    cfg.Save();

    return true;
}

/*
===================
libMain
===================
*/
libMain()
{
    if (!libGetEngine(ENGINE_PATH))
    {
        libDialog::Error("Error!", "Couldn't load lib Engine.");
        return -1;
    }

    libStr path;
    libDir::GetLocalDataLocation(path);
    path.Append("/Pusher");
    libDir::Create(path.Get());
    path.Append("/Pusher.cfg");

    cfg.Load(path.Get(), DATA_PACK "Pusher.cfg");
    bool maximalized = cfg.GetBool("Maximalized", false);
    bool fullscreen = cfg.GetBool("Fullscreen", false);
    bool soundEnabled = cfg.GetBool("SoundEnabled", true);
    int width = cfg.GetInt("Width", WINDOW_DEFAULT_WIDTH);
    int height = cfg.GetInt("Height", WINDOW_DEFAULT_HEIGHT);
    aspectRatio = libCast<float>(WINDOW_DEFAULT_WIDTH) / libCast<float>(WINDOW_DEFAULT_HEIGHT);

    // Makes sure that we have the right aspect ratio of the window
    if (width > height)
    {
        height = libCast<int>(width / aspectRatio);
    }
    else
    {
        width = libCast<int>(height * aspectRatio);
    }

    engine->SetState(LIB_APP_NAME, "Pusher");
    engine->SetState(LIB_WINDOW_TITLE, "Pusher");
    engine->SetState(LIB_WINDOW, maximalized ? LIB_MAXIMALIZED : LIB_NORMAL);
    engine->SetState(LIB_VIDEO_MODE, fullscreen ? LIB_FULLSCREEN_WINDOW : LIB_WINDOWED);
    engine->SetState(LIB_WINDOW_SIZE, width, height);
    engine->SetState(LIB_WINDOW_SIZE_MIN, WINDOW_MINIMAL_WIDTH, WINDOW_MINIMAL_HEIGHT);
    engine->SetState(LIB_WINDOW_KEEP_RATIO, true);
    engine->SetState(LIB_WINDOW_ALLOW_RESIZING, true);
    engine->SetState(LIB_ALLOW_FULLSCREEN_SWITCHING, true);
    engine->SetState(LIB_PREVENT_SECOND_LAUNCH, true);
    engine->SetState(LIB_AUDIO_VOLUME, soundEnabled ? 1.0f : 0.0f);
    engine->SetState(LIB_FPS_LIMIT, 60);
    engine->SetState(LIB_LOG_FILE, true);
    engine->SetState(LIB_LOG_FILENAME, "Pusher.log");
    engine->SetState(LIB_INIT, Init);
    engine->SetState(LIB_RENDER, Draw);
    engine->SetState(LIB_FRAME, Update);
    engine->SetState(LIB_FREE, Free);

    engine->Start();
    libFreeEngine();

    return 0;
}
