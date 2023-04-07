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
    LIB_CHECK(engine->GetTexture(t_background, DATA_PACK "Textures/Background.tga"));
    LIB_CHECK(engine->GetFont(font, DATA_PACK "Font.ttf"));

    LIB_CHECK(menu.Init());
    LIB_CHECK(game.Init());

    font->SetColor(LIB_COLOR_WHITE);
    font->SetShadowShift(libVec2(1.0f, 1.0f));
    font->SetShadowType(libFont::SHADOW_ADDAPTIVE);

    engine->UnloadUnused();

    return true;
}

/*
===================
Draw
===================
*/
bool Draw()
{
    engine->ClearScreen(LIB_COLOR_BLACK);

    screenSize.Set(engine->GetState(LIB_SCREEN_WIDTH), engine->GetState(LIB_SCREEN_HEIGHT));
    gameScreenSize.Set(engine->GetState(LIB_SCREEN_WIDTH), engine->GetState(LIB_SCREEN_HEIGHT));
    gameScreen.Set(0, 0);

    libVec2 tileSize;

    if (screenSize.x > screenSize.y)
    {
        tileSize.y = libCast<float>(screenSize.y / LEVEL_HEIGHT);
        tileSize.x = tileSize.y;
    }
    else
    {
        tileSize.x = libCast<float>(screenSize.x / LEVEL_WIDTH);
        tileSize.y = tileSize.x;
    }

    libVec2i offset;
    offset.x = libCast<int>((gameScreenSize.x - tileSize.x * LEVEL_WIDTH) / 2.0f);
    offset.y = libCast<int>((gameScreenSize.y - tileSize.y * LEVEL_HEIGHT) / 2.0f);
    gameScreen += offset;
    gameScreenSize -= offset;

    libVec2 vec(libCast<float>(screenSize.x / t_background->GetWidth()),
                libCast<float>(screenSize.y / t_background->GetHeight()));

    t_background->Draw2DQuad(libQuad(libVertex(), libVertex(libVec2(screenSize), vec)));

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
    if (engine->IsKeyPressed(LIBK_F12))
        engine->TakeScreenshot();

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
    float aspectRatio = libCast<float>(WINDOW_DEFAULT_WIDTH) / libCast<float>(WINDOW_DEFAULT_HEIGHT);

    // Makes sure that we have the right aspect ratio of the window
    if (width > height)
        height = libCast<int>(width / aspectRatio);
    else
        width = libCast<int>(height * aspectRatio);

    engine->SetState(LIB_APP_NAME, "Pusher");
    engine->SetState(LIB_WINDOW_TITLE, "Pusher");
    engine->SetState(LIB_WINDOW, maximalized ? LIB_MAXIMALIZED : LIB_NORMAL);
    engine->SetState(LIB_VIDEO_MODE, fullscreen ? LIB_FULLSCREEN_WINDOW : LIB_WINDOWED);
    engine->SetState(LIB_WINDOW_SIZE, width, height);
    engine->SetState(LIB_WINDOW_SIZE_MIN, WINDOW_MINIMAL_WIDTH, WINDOW_MINIMAL_HEIGHT);
    engine->SetState(LIB_WINDOW_KEEP_RATIO, true);
    engine->SetState(LIB_WINDOW_RESIZABLE, true);
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
