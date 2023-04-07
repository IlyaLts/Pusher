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

libVec2i screenSize(WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT);
libVec2i gameScreenPos;
libVec2i gameScreenSize;
libVec2 tileSize;

libTexture *tex_background = nullptr;
libFont *font = nullptr;

libMesh *mesh_background = nullptr;

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
    LIB_CHECK(engine->Get(tex_background, DATA_PACK "Textures/Background.tga"));
    LIB_CHECK(engine->Get(font, DATA_PACK "Font.ttf"));
    LIB_CHECK(engine->Get(mesh_background));

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
    engine->ClearScreen();

    screenSize.Set(engine->State(LIB_SCREEN_WIDTH), engine->State(LIB_SCREEN_HEIGHT));
    gameScreenSize = screenSize;
    gameScreenPos.Set(0, 0);

    if (screenSize.x > screenSize.y)
    {
        tileSize.x = tileSize.y;
        tileSize.y = libCast<float>(screenSize.y / LEVEL_HEIGHT);
    }
    else
    {
        tileSize.x = libCast<float>(screenSize.x / LEVEL_WIDTH);
        tileSize.y = tileSize.x;
    }

    libVec2i offset;
    offset.x = libCast<int>((gameScreenSize.x - tileSize.x * LEVEL_WIDTH) / 2.0f);
    offset.y = libCast<int>((gameScreenSize.y - tileSize.y * LEVEL_HEIGHT) / 2.0f);
    gameScreenSize -= offset;
    gameScreenPos += offset;

    libVec2 texCoord(libCast<float>(screenSize.x / tex_background->Width()),
                     libCast<float>(screenSize.y / tex_background->Height()));

    mesh_background->Clear();
    mesh_background->Add(libQuad(libVertex(), libVertex(libVec2(screenSize), texCoord)));
    engine->Draw(mesh_background, tex_background, true);

    if (menu.State() == Menu::HIDDEN)
        game.Draw();

    menu.Draw();

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

    if (menu.State() != Menu::HIDDEN)
    {
        if (menu.State() == Menu::MAIN_MENU && engine->IsKeyPressed(LIBK_ESCAPE))
        {
            if (game.State() == Game::NOT_STARTED)
                engine->Stop();
            else
                menu.SetState(Menu::HIDDEN);
        }

        menu.Update(&game);
    }
    else
    {
        game.Update();

        if (engine->IsKeyPressed(LIBK_ESCAPE))
        {
            if (game.State() == Game::EDITOR_PLAYTEST)
                game.SetState(Game::EDITOR);
            else
                menu.SetState(Menu::MAIN_MENU);
        }

        if (game.IsCompleted())
        {
            game.Reset();
            menu.SetState(Menu::MAIN_MENU);
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
    cfg.SetBool("Maximalized", engine->State(LIB_WINDOW) == LIB_MAXIMALIZED);
    cfg.SetBool("Fullscreen", engine->State(LIB_VIDEO_MODE) == LIB_FULLSCREEN_WINDOW);
    cfg.SetInt("Width", engine->State(LIB_WINDOW_WIDTH));
    cfg.SetInt("Height", engine->State(LIB_WINDOW_HEIGHT));
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
    engine->SetState(LIB_FPS_LIMIT, 30);
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
