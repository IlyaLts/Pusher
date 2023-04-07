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
#include "Game.h"

/*
===================
Game::Game
===================
*/
Game::Game()
{
    state = State::NONE;
    currentLevel = 1;
    currentBlock = 1;
    episodeAlreadyExists = false;
    gameCompleted = false;

    for (int i = 0; i < LEVEL_WIDTH; i++)
        for (int j = 0; j < LEVEL_HEIGHT; j++)
            path[i][j] = MOVE_BLOCKED;
}

/*
===================
Game::Init
===================
*/
bool Game::Init()
{
    LIB_CHECK(engine->GetTexture(t_moveBlocked, DATA_PACK "Textures/MoveBlocked.tga"));
    LIB_CHECK(engine->GetTexture(t_movePath, DATA_PACK "Textures/MovePath.tga"));
    LIB_CHECK(engine->GetTexture(t_move, DATA_PACK "Textures/Move.tga"));
    LIB_CHECK(engine->GetTexture(t_moveBox, DATA_PACK "Textures/MoveBox.tga"));
    LIB_CHECK(engine->GetTexture(t_box, DATA_PACK "Textures/Box.tga"));
    LIB_CHECK(engine->GetTexture(t_endpoint, DATA_PACK "Textures/Endpoint.tga"));
    LIB_CHECK(engine->GetTexture(t_wall, DATA_PACK "Textures/Wall.tga"));
    LIB_CHECK(engine->GetTexture(t_floor, DATA_PACK "Textures/Floor.tga"));
    LIB_CHECK(engine->GetTexture(t_backgrounds, DATA_PACK "Textures/Background.tga"));
    LIB_CHECK(engine->GetSprite(t_robot, DATA_PACK "Textures/Robot/Robot.tga"));
    LIB_CHECK(engine->GetSound(s_move, DATA_PACK "Sounds/Move.wav"));
    LIB_CHECK(engine->GetSound(s_levelComplete, DATA_PACK "Sounds/LevelComplete.wav"));

    t_robot->SetDuration(ROBOT_DURATION);
    t_robot->Play();

    moveMouseDelay = cfg.GetFloat("MoveMouseDelay", MOVE_DELAY);
    moveKeyDelay = cfg.GetFloat("MoveKeyDelay", KEY_DELAY);

    return true;
}

/*
===================
Game::Draw
===================
*/
void Game::Draw()
{
    if (state == State::EDITOR_NEW_NAME)
    {
        font->SetAlign(LIB_CENTER);
        font->SetColor(LIB_COLOR_WHITE);
        font->SetSize(libCast<int>(screenSize.y * FONT_MENU_H2_SIZE));
        font->Print2D(screenSize.x / 2.0f, font->GetLineSpacing(), "Please enter your episode name.");
        font->Print2D(screenSize.x / 2.0f, screenSize.y / 2.0f, episode.name.Get());

        if (episodeAlreadyExists)
        {
            font->SetColor(LIB_COLOR_RED);
            font->SetSize(libCast<int>(screenSize.y * FONT_MENU_TEXT_SIZE));
            font->Print2D(screenSize.x / 2.0f, screenSize.y - font->GetLineSpacing(), "Episode with given name already exists!");
        }

        return;
    }

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

    libVec2 hoverOver(gameScreen.x + mouse.x * tileSize.x, gameScreen.y + mouse.y * tileSize.y);

    libColor transparentColor(1.0f, 1.0f, 1.0f, 0.75f);
    libColor darkenedColor(0.75f, 0.75f, 0.75f, 1.0f);

    libQuad block(libVertex(libVec2(0.0f, 0.0f), libVec2(0.0f, 0.0f)), libVertex(tileSize, libVec2(1.0f, 1.0f)));
    libQuad blockInEndpoint(libVertex(libVec2(0.0f, 0.0f), libVec2(0.0f, 0.0f)), libVertex(tileSize, libVec2(1.0f, 1.0f)));
    libQuad blockTransparent(libVertex(libVec2(0.0f, 0.0f), libVec2(0.0f, 0.0f)), libVertex(tileSize, libVec2(1.0f, 1.0f)));

    blockInEndpoint.Set(darkenedColor);
    blockTransparent.Set(transparentColor);

    // Level
    for (int i = 0; i < LEVEL_WIDTH; i++)
    {
        for (int j = 0; j < LEVEL_HEIGHT; j++)
        {
            float x = libCast<float>(gameScreen.x + i * tileSize.x);
            float y = libCast<float>(gameScreen.y + j * tileSize.y);
            
            // Floor
            if (GetLevel().Get(i, j).floor) t_floor->Draw2DQuad(block, x, y);

            // Blocks
            switch (GetLevel().Get(i, j).type)
            {
                case Block::ROBOT:
                {
                    t_robot->Draw2DQuad(block, x, y);
                    break;
                }
                case Block::BOX:
                {
                    t_box->Draw2DQuad(block, x, y);
                    break;
                }
                case Block::WALL:
                {
                    t_wall->Draw2DQuad(block, x, y);
                    break;
                }
                case Block::ENDPOINT:
                {
                    t_endpoint->Draw2DQuad(block, x, y);
                    break;
                }
                case Block::BOX_ON_ENDPOINT:
                {
                    t_endpoint->Draw2DQuad(block, x, y);
                    t_box->Draw2DQuad(blockInEndpoint, x, y);
                    break;
                }
                case Block::ROBOT_ON_ENDPOINT:
                {
                    t_endpoint->Draw2DQuad(block, x, y);
                    t_robot->Draw2DQuad(block, x, y);
                    break;
                }
            }

            // Path points
            if ((state == State::GAME || state == State::EDITOR_PLAY) && path[i][j] > 0)
            {
                t_movePath->Draw2DQuad(block, x, y);
            }
        }
    }

    if (IsCursorInField())
    {
        if (state == State::GAME || state == State::EDITOR_PLAY)
        {
            // Game cursor
            int temp[LEVEL_WIDTH][LEVEL_HEIGHT];
            moveType_t result = FindPath(temp, mouse.x, mouse.y);

            switch (result)
            {
            case MOVE:
            {
                t_move->Draw2DQuad(block, hoverOver.x, hoverOver.y);
                break;
            }
            case MOVE_BOX:
            {
                t_moveBox->Draw2DQuad(block, hoverOver.x, hoverOver.y);
                break;
            }
            case MOVE_BLOCKED:
            {
                t_moveBlocked->Draw2DQuad(block, hoverOver.x, hoverOver.y);
                break;
            }
            }
        }
        else if (state == State::EDITOR)
        {
            // Brush tool
            switch (currentBlock)
            {
            case Block::ROBOT:
            {               
                t_robot->Draw2DQuad(blockTransparent, hoverOver.x, hoverOver.y);
                break;
            }
            case Block::BOX:
            {
                t_box->Draw2DQuad(blockTransparent, hoverOver.x, hoverOver.y);
                break;
            }
            case Block::WALL:
            {
                t_wall->Draw2DQuad(blockTransparent, hoverOver.x, hoverOver.y);
                break;
            }
            case Block::ENDPOINT:
            {
                t_endpoint->Draw2DQuad(blockTransparent, hoverOver.x, hoverOver.y);
                break;
            }
            case Block::BOX_ON_ENDPOINT:
            {
                libQuad blockInEndpointTransparent(blockInEndpoint);
                blockInEndpointTransparent.v[0].color.alpha = blockTransparent.v[0].color.alpha;
                blockInEndpointTransparent.v[1].color.alpha = blockTransparent.v[0].color.alpha;
                blockInEndpointTransparent.v[2].color.alpha = blockTransparent.v[0].color.alpha;
                blockInEndpointTransparent.v[3].color.alpha = blockTransparent.v[0].color.alpha;

                t_endpoint->Draw2DQuad(blockTransparent, hoverOver.x, hoverOver.y);
                t_box->Draw2DQuad(blockInEndpointTransparent, hoverOver.x, hoverOver.y);
                break;
            }
            case Block::ROBOT_ON_ENDPOINT:
            {
                t_endpoint->Draw2DQuad(blockTransparent, hoverOver.x, hoverOver.y);
                t_robot->Draw2DQuad(blockTransparent, hoverOver.x, hoverOver.y);
                break;
            }
            }
        }
    }
}

/*
===================
Game::Update
===================
*/
void Game::Update()
{
    if (state == State::EDITOR_NEW_NAME)
    {
        char c = engine->GetKeyValue(engine->GetKey());

        if (c)
        {
            episodeAlreadyExists = false;
            episode.name.Append(c);
        }

        if (engine->IsKeyPressed(LIBK_BACKSPACE))
        {
            episodeAlreadyExists = false;
            episode.name.Erase(episode.name.Length() - 1);
        }

        if (engine->IsKeyPressed(LIBK_ENTER))
        {
            episode.filepath = episode.name;
            episode.filepath.Replace(" ", "_");
            episode.filepath.Append(".lvl");
            episode.filepath.Insert(CUSTOM_LEVELS_PATH, 0);

            libDir::Create(CUSTOM_LEVELS_PATH);

            if (!libFile::Exists(episode.filepath.Get()))
            {
                state = State::EDITOR;
                SaveEpisode();
            }
            else
            {
                episodeAlreadyExists = true;
            }
        }

        return;
    }

    engine->GetMousePos(mouse.x, mouse.y);
    mouse -= gameScreen;

    if (mouse.x >= 0 && mouse.y >= 0 && mouse.x < gameScreenSize.x && mouse.y < gameScreenSize.y)
    {
        mouse.x = libCast<int>(mouse.x / tileSize.x);
        mouse.y = libCast<int>(mouse.y / tileSize.y);
    }
    else
    {
        mouse.Set(-1, -1);
    }

    // Switches to Game/Editor modes (Only for editor)
#ifdef LIB_DEBUG
    if (engine->IsKey(LIBK_CTRL))
#else
    if ((GetState() == State::EDITOR || GetState() == State::EDITOR_PLAY) && engine->IsKey(LIBK_CTRL))
#endif
    {
        if (engine->IsKeyPressed(LIBK_G))
        {
            SetState((GetState() == State::GAME || state == State::EDITOR_PLAY) ? State::EDITOR : State::EDITOR_PLAY);
        }
    }

    if ((state == State::GAME) || (state == State::EDITOR_PLAY))
        GameUpdate();
    else if (state == State::EDITOR)
        EditorUpdate();
}

/*
===================
Game::GameUpdate
===================
*/
void Game::GameUpdate()
{
    auto &level = GetLevel();
    libVec2i currentCoord(robot.x, robot.y);
    auto &current = level.Get(currentCoord.x, currentCoord.y).type;

    engine->SetState(LIB_WINDOW_TITLE, libFormat("Pusher - %s - Level: %d", episode.name.Get(), currentLevel));

    bool levelCompleted = true;

    for (int i = 0; i < LEVEL_WIDTH; i++)
    {
        for (int j = 0; j < LEVEL_HEIGHT; j++)
        {
            if (level.Get(i, j).type == Block::ENDPOINT || level.Get(i, j).type == Block::ROBOT_ON_ENDPOINT)
            {
                levelCompleted = false;
            }
        }
    }

#ifdef LIB_DEBUG
    if (engine->IsKey(LIBK_CTRL) && engine->IsKeyPressed(LIBK_ENTER))
        levelCompleted = true;
#endif

    if (levelCompleted)
    {
        if (currentLevel == episode.levels.Size())
        {
            if (state == State::EDITOR_PLAY)
                state = State::EDITOR;
            else
                gameCompleted = true;
        }

        if (state != State::EDITOR_PLAY)
        {
            if (episode.unlocked.Get() == currentLevel)
                episode.unlocked = currentLevel + 1;

            // Saves the current number of unlocked levels
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
            libHolder<int> levelsSize(episode.levels.Size());
            libHolder<int> temp(episode.unlocked.Get());
            save.Write(&levelsSize, sizeof(libHolder<int>), 1);
            save.Write(&temp, sizeof(libHolder<int>), 1);
            save.Close();
        }

        s_levelComplete->Play();
        NextLevel();
    }

    if (engine->IsKey(LIBK_CTRL))
    {
        if (engine->IsKeyPressed(LIBK_R)) RestartLevel();
        if (engine->IsKeyPressed(LIBK_LBRACKET)) PreviousLevel();
        if (engine->IsKeyPressed(LIBK_RBRACKET)) NextLevel();

        // Undo the latest move
        if (engine->IsKeyPressed(LIBK_Z))
        {
            if (!history.IsEmpty())
            {
                GetLevel() = history.Get();
                history.Pop();
                s_move->Play();

                for (int i = 0; i < LEVEL_WIDTH; i++)
                {
                    for (int j = 0; j < LEVEL_HEIGHT; j++)
                    {
                        path[i][j] = MOVE_BLOCKED;

                        if (GetLevel().Get(i, j).type == Block::ROBOT || GetLevel().Get(i, j).type == Block::ROBOT_ON_ENDPOINT)
                        {
                            robot.x = i;
                            robot.y = j;
                        }
                    }
                }
            }
        }
    }

    enum moveDirection
    {
        None,
        Up,
        Down,
        Left,
        Right
    };

    moveDirection moveKeyDir = None;
    static moveDirection movePressedKeyDir = None;
    static libVec2i mouseTarget(-1, -1);

    bool canMoveByMouse = (moveTimer.GetMilliseconds() >= moveMouseDelay);
    bool canMoveByKey = (moveTimer.GetMilliseconds() >= moveKeyDelay);

    // Sets a path from the robot to the mouse cursor position
    if (engine->IsKeyPressed(LIBK_MOUSE_LEFT))
    {
        mouseTarget.Set(mouse.x, mouse.y);
        FindPath(path, mouse.x, mouse.y);
    }

    // Determines which way to move the robot based on a new pressed key
    if (engine->IsKeyPressed(LIBK_UP) || engine->IsKeyPressed(LIBK_W))      movePressedKeyDir = Up;
    if (engine->IsKeyPressed(LIBK_DOWN) || engine->IsKeyPressed(LIBK_S))    movePressedKeyDir = Down;
    if (engine->IsKeyPressed(LIBK_LEFT) || engine->IsKeyPressed(LIBK_A))    movePressedKeyDir = Left;
    if (engine->IsKeyPressed(LIBK_RIGHT) || engine->IsKeyPressed(LIBK_D))   movePressedKeyDir = Right;

    // If the robot moves to the target after pressing the mouse
    if (mouseTarget.x >= 0 && mouseTarget.y >= 0)
    {
        // If the robot reached the target
        if (path[mouseTarget.x][mouseTarget.y] == MOVE_BLOCKED) mouseTarget.Set(-1, -1);

        // Stops moving the robot along a path if moving the robot manually
        if ((moveKeyDir || movePressedKeyDir) && path[mouseTarget.x][mouseTarget.y] >= 0)
        {
            for (int i = 0; i < LEVEL_WIDTH; i++)
                for (int j = 0; j < LEVEL_HEIGHT; j++)
                    path[i][j] = MOVE_BLOCKED;

            // Allows to move the robot manually without a delay
            canMoveByKey = true;
        }
    }

    // Determines which final way to move the robot based on the keyboard
    if (canMoveByKey)
    {
        // Did a new key get pressed?
        if (movePressedKeyDir)
        {
            moveKeyDir = movePressedKeyDir;
            movePressedKeyDir = None;
        }
        else
        {
            if (engine->IsKey(LIBK_UP) || engine->IsKey(LIBK_W))        moveKeyDir = Up;
            if (engine->IsKey(LIBK_DOWN) || engine->IsKey(LIBK_S))      moveKeyDir = Down;
            if (engine->IsKey(LIBK_LEFT) || engine->IsKey(LIBK_A))      moveKeyDir = Left;
            if (engine->IsKey(LIBK_RIGHT) || engine->IsKey(LIBK_D))     moveKeyDir = Right;
        }
    }

    // Moves the robot up
    if (((path[robot.x][robot.y - 1] > path[robot.x][robot.y] && canMoveByMouse) || moveKeyDir == Up) && robot.y > 0)
    {
        moveTimer.Restart();

        libVec2i targetCoord(robot.x, robot.y - 1);
        libVec2i afterCoord(robot.x, robot.y - 2);
        auto &target = level.Get(targetCoord.x, targetCoord.y).type;
        auto &after = level.Get(afterCoord.x, afterCoord.y).type;

        bool historySaved = false;

        // Moves a box
        if ((target == Block::BOX || target == Block::BOX_ON_ENDPOINT) && (after == Block::EMPTY || after == Block::ENDPOINT))
        {
            if (!historySaved)
            {
                history.Push(GetLevel());
                historySaved = true;
            }

            if (target == Block::BOX)                   level.Set(targetCoord.x, targetCoord.y, Block::EMPTY);
            if (target == Block::BOX_ON_ENDPOINT)       level.Set(targetCoord.x, targetCoord.y, Block::ENDPOINT);
            if (after == Block::EMPTY)                  level.Set(afterCoord.x, afterCoord.y, Block::BOX);
            if (after == Block::ENDPOINT)               level.Set(afterCoord.x, afterCoord.y, Block::BOX_ON_ENDPOINT);
        }

        // Moves the robot
        if (target == Block::EMPTY || target == Block::ENDPOINT)
        {
            if (!historySaved)
            {
                history.Push(GetLevel());
                historySaved = true;
            }

            if (current == Block::ROBOT)                level.Set(currentCoord.x, currentCoord.y, Block::EMPTY);
            if (current == Block::ROBOT_ON_ENDPOINT)    level.Set(currentCoord.x, currentCoord.y, Block::ENDPOINT);
            if (target == Block::EMPTY)                 level.Set(targetCoord.x, targetCoord.y, Block::ROBOT);
            if (target == Block::ENDPOINT)              level.Set(targetCoord.x, targetCoord.y, Block::ROBOT_ON_ENDPOINT);

            robot.y--;
            path[robot.x][robot.y] = MOVE_BLOCKED;
            s_move->Play();
        }
    }
    // Moves the robot down
    else if (((path[robot.x][robot.y + 1] > path[robot.x][robot.y] && canMoveByMouse) || moveKeyDir == Down) && robot.y < LEVEL_HEIGHT - 1)
    {
        moveTimer.Restart();

        libVec2i targetCoord(robot.x, robot.y + 1);
        libVec2i afterCoord(robot.x, robot.y + 2);
        auto &target = level.Get(targetCoord.x, targetCoord.y).type;
        auto &after = level.Get(afterCoord.x, afterCoord.y).type;

        bool historySaved = false;

        // Moves a box
        if ((target == Block::BOX || target == Block::BOX_ON_ENDPOINT) && (after == Block::EMPTY || after == Block::ENDPOINT))
        {
            if (!historySaved)
            {
                history.Push(GetLevel());
                historySaved = true;
            }

            if (target == Block::BOX)                   level.Set(targetCoord.x, targetCoord.y, Block::EMPTY);
            if (target == Block::BOX_ON_ENDPOINT)       level.Set(targetCoord.x, targetCoord.y, Block::ENDPOINT);
            if (after == Block::EMPTY)                  level.Set(afterCoord.x, afterCoord.y, Block::BOX);
            if (after == Block::ENDPOINT)               level.Set(afterCoord.x, afterCoord.y, Block::BOX_ON_ENDPOINT);
        }

        // Moves the robot
        if (target == Block::EMPTY || target == Block::ENDPOINT)
        {
            if (!historySaved)
            {
                history.Push(GetLevel());
                historySaved = true;
            }

            if (current == Block::ROBOT)                level.Set(currentCoord.x, currentCoord.y, Block::EMPTY);
            if (current == Block::ROBOT_ON_ENDPOINT)    level.Set(currentCoord.x, currentCoord.y, Block::ENDPOINT);
            if (target == Block::EMPTY)                 level.Set(targetCoord.x, targetCoord.y, Block::ROBOT);
            if (target == Block::ENDPOINT)              level.Set(targetCoord.x, targetCoord.y, Block::ROBOT_ON_ENDPOINT);

            robot.y++;
            path[robot.x][robot.y] = MOVE_BLOCKED;
            s_move->Play();
        }
    }
    // Moves the robot left
    else if (((path[robot.x - 1][robot.y] > path[robot.x][robot.y] && canMoveByMouse) || moveKeyDir == Left) && robot.x > 0)
    {
        moveTimer.Restart();

        libVec2i targetCoord(robot.x - 1, robot.y);
        libVec2i afterCoord(robot.x - 2, robot.y);
        auto &target = level.Get(targetCoord.x, targetCoord.y).type;
        auto &after = level.Get(afterCoord.x, afterCoord.y).type;

        bool historySaved = false;

        // Moves a box
        if ((target == Block::BOX || target == Block::BOX_ON_ENDPOINT) && (after == Block::EMPTY || after == Block::ENDPOINT))
        {
            if (!historySaved)
            {
                history.Push(GetLevel());
                historySaved = true;
            }

            if (target == Block::BOX)                   level.Set(targetCoord.x, targetCoord.y, Block::EMPTY);
            if (target == Block::BOX_ON_ENDPOINT)       level.Set(targetCoord.x, targetCoord.y, Block::ENDPOINT);
            if (after == Block::EMPTY)                  level.Set(afterCoord.x, afterCoord.y, Block::BOX);
            if (after == Block::ENDPOINT)               level.Set(afterCoord.x, afterCoord.y, Block::BOX_ON_ENDPOINT);
        }

        // Moves the robot
        if (target == Block::EMPTY || target == Block::ENDPOINT)
        {
            if (!historySaved)
            {
                history.Push(GetLevel());
                historySaved = true;
            }

            if (current == Block::ROBOT)                level.Set(currentCoord.x, currentCoord.y, Block::EMPTY);
            if (current == Block::ROBOT_ON_ENDPOINT)    level.Set(currentCoord.x, currentCoord.y, Block::ENDPOINT);
            if (target == Block::EMPTY)                 level.Set(targetCoord.x, targetCoord.y, Block::ROBOT);
            if (target == Block::ENDPOINT)              level.Set(targetCoord.x, targetCoord.y, Block::ROBOT_ON_ENDPOINT);

            robot.x--;
            path[robot.x][robot.y] = MOVE_BLOCKED;
            s_move->Play();
        }
    }
    // Moves the robot right
    else if (((path[robot.x + 1][robot.y] > path[robot.x][robot.y] && canMoveByMouse) || moveKeyDir == Right) && robot.x < LEVEL_WIDTH - 1)
    {
        moveTimer.Restart();

        libVec2i targetCoord(robot.x + 1, robot.y);
        libVec2i afterCoord(robot.x + 2, robot.y);
        auto &target = level.Get(targetCoord.x, targetCoord.y).type;
        auto &after = level.Get(afterCoord.x, afterCoord.y).type;

        bool historySaved = false;

        // Moves a box
        if ((target == Block::BOX || target == Block::BOX_ON_ENDPOINT) && (after == Block::EMPTY || after == Block::ENDPOINT))
        {
            if (!historySaved)
            {
                history.Push(GetLevel());
                historySaved = true;
            }

            if (target == Block::BOX)                   level.Set(targetCoord.x, targetCoord.y, Block::EMPTY);
            if (target == Block::BOX_ON_ENDPOINT)       level.Set(targetCoord.x, targetCoord.y, Block::ENDPOINT);
            if (after == Block::EMPTY)                  level.Set(afterCoord.x, afterCoord.y, Block::BOX);
            if (after == Block::ENDPOINT)               level.Set(afterCoord.x, afterCoord.y, Block::BOX_ON_ENDPOINT);
        }

        // Moves the robot
        if (target == Block::EMPTY || target == Block::ENDPOINT)
        {
            if (!historySaved)
            {
                history.Push(GetLevel());
                historySaved = true;
            }

            if (current == Block::ROBOT)                level.Set(currentCoord.x, currentCoord.y, Block::EMPTY);
            if (current == Block::ROBOT_ON_ENDPOINT)    level.Set(currentCoord.x, currentCoord.y, Block::ENDPOINT);
            if (target == Block::EMPTY)                 level.Set(targetCoord.x, targetCoord.y, Block::ROBOT);
            if (target == Block::ENDPOINT)              level.Set(targetCoord.x, targetCoord.y, Block::ROBOT_ON_ENDPOINT);

            robot.x++;
            path[robot.x][robot.y] = MOVE_BLOCKED;
            s_move->Play();
        }
    }
}

/*
===================
Game::EditorUpdate
===================
*/
void Game::EditorUpdate()
{
    auto &level = GetLevel();

    if (IsCursorInField())
    {
        engine->SetState(LIB_WINDOW_TITLE, libFormat("Pusher Editor: Level: %d (%d:%d)", currentLevel, mouse.x + 1, mouse.y + 1));
    }
    else
    {
        engine->SetState(LIB_WINDOW_TITLE, libFormat("Pusher Editor: Level: %d", currentLevel));
    }

    if (engine->IsKey(LIBK_CTRL))
    {
        if (engine->IsKeyPressed(LIBK_A))           AddLevel();
        if (engine->IsKeyPressed(LIBK_S))           SaveEpisode();
        if (engine->IsKeyPressed(LIBK_D))           DeleteLevel();
        if (engine->IsKeyPressed(LIBK_LBRACKET))    PreviousLevel();
        if (engine->IsKeyPressed(LIBK_RBRACKET))    NextLevel();

        // Shifts the current level in the current episode to the start
        if (engine->IsKeyPressed(LIBK_Q))
        {
            if (currentLevel > 1)
            {
                Level temp(episode.levels[currentLevel - 1]);
                episode.levels[currentLevel - 1] = episode.levels[currentLevel - 2];
                episode.levels[currentLevel - 2] = temp;
                currentLevel--;
            }
        }

        // Shifts the current level in the current episode to the end
        if (engine->IsKeyPressed(LIBK_E))
        {
            if (currentLevel < episode.levels.Size())
            {
                Level temp(episode.levels[currentLevel - 1]);
                episode.levels[currentLevel - 1] = episode.levels[currentLevel];
                episode.levels[currentLevel] = temp;
                currentLevel++;
            }
        }

        // Makes a copy of a level
        if (engine->IsKeyPressed(LIBK_C))
        {
            int temp = currentLevel;
            AddLevel();
            episode.levels[currentLevel - 1] = episode.levels[temp - 1];
        }

        // Undo the latest change
        if (engine->IsKeyPressed(LIBK_Z))
        {
            if (!history.IsEmpty())
            {
                GetLevel() = history.Get();
                history.Pop();
            }
        }
    }

    if (engine->IsKeyPressed(LIBK_UP))      GetLevel().ShiftUp();
    if (engine->IsKeyPressed(LIBK_DOWN))    GetLevel().ShiftDown();
    if (engine->IsKeyPressed(LIBK_LEFT))    GetLevel().ShiftLeft();
    if (engine->IsKeyPressed(LIBK_RIGHT))   GetLevel().ShiftRight();

    // The easiest fix for the touchpad as it has scrolling inertia and switches brush types way too fast
    static int wheelZeros = 0;
    static bool wheelZero = true;

    wheelZeros = (engine->GetMouseWheel() ? 0 : (wheelZeros <= 2 ? wheelZeros + 1 : wheelZeros));
    if (wheelZeros > 2) wheelZero = true;

    // Switches brush types - Mouse wheel up
    if (engine->GetMouseWheel() > 0 && wheelZero)
    {
        wheelZero = false;
        currentBlock++;

        if (currentBlock == Block::TYPE_SIZE)
        {
            currentBlock = Block::EMPTY;
            currentBlock++;
        }
    }
    // Mouse wheel down
    else if (engine->GetMouseWheel() < 0 && wheelZero)
    {
        wheelZero = false;
        currentBlock--;

        if (currentBlock == Block::EMPTY)
        {
            currentBlock = Block::TYPE_SIZE;
            currentBlock--;
        }
    }

    if (IsCursorInField())
    {
        // Adds the current block to the current level
        if (engine->IsKey(LIBK_MOUSE_LEFT) && level.Get(mouse.x, mouse.y).type != (Block::Type)currentBlock)
        {
            history.Push(episode.levels[currentLevel - 1]);

            // Doesn't allow to have more than one robot per a level
            if (currentBlock == Block::ROBOT || currentBlock == Block::ROBOT_ON_ENDPOINT)
            {
                for (int i = 0; i < LEVEL_WIDTH; i++)
                {
                    for (int j = 0; j < LEVEL_HEIGHT; j++)
                    {
                        if (level.Get(i, j).type == Block::ROBOT || level.Get(i, j).type == Block::ROBOT_ON_ENDPOINT)
                        {
                            level.Set(i, j, Block::EMPTY);
                        }
                    }
                }
            }

            level.Set(mouse.x, mouse.y, (Block::Type)currentBlock);
        }

        // Clears a block from the current level
        if (engine->IsKey(LIBK_MOUSE_RIGHT) && level.Get(mouse.x, mouse.y).type != Block::EMPTY)
        {
            if (level.Get(mouse.x, mouse.y).type == Block::ROBOT) robot.Set(-1, -1);

            history.Push(episode.levels[currentLevel - 1]);
            level.Set(mouse.x, mouse.y, Block::EMPTY);
        }
    }
}

/*
===================
Game::SetState
===================
*/
void Game::SetState(Game::State state)
{
    this->state = state;
    history.PopAll();
    gameCompleted = false;

    if (state == State::GAME || state == State::EDITOR_PLAY)
        RestartLevel();
}

/*
===================
Game::SetEpisode
===================
*/
void Game::SetEpisode(const Episode &episode)
{
    Reset();
    this->episode = episode;
    SetLevel(1);
}

/*
===================
Game::SaveEpisode
===================
*/
void Game::SaveEpisode(const char *path)
{
    if (state != State::EDITOR) return;

    // If filepath is unknown, then asks for a new episode name
    if (!path && episode.filepath.IsEmpty())
    {
        state = State::EDITOR_NEW_NAME;
        return;
    }

    libFile lvl(path ? path : episode.filepath.Get(), libFile::Mode::WriteBin);
    if (!lvl.IsOpened()) return;

    int numOfLevels = episode.levels.Size();
    lvl.Write(&numOfLevels, sizeof(int), 1);

    for (auto &level : episode.levels)
    {
        for (int i = 0; i < LEVEL_WIDTH; i++)
        {
            for (int j = 0; j < LEVEL_HEIGHT; j++)
            {
                lvl.Write(&level.Get(i, j), sizeof(Block::Type), 1);
            }
        }
    }

    lvl.Close();
    
    // Checks whether levels were completed or not
    for (int n = 1; auto & level : episode.levels)
    {
        bool hasRobot = false;
        int boxes = 0;
        int endpoints = 0;

        const char *title = "Episode is saved, but not fully completed!";

        for (int i = 0; i < LEVEL_WIDTH; i++)
        {
            for (int j = 0; j < LEVEL_HEIGHT; j++)
            {
                if (level.Get(i, j).type == Block::ROBOT || level.Get(i, j).type == Block::ROBOT_ON_ENDPOINT)
                    hasRobot = true;

                if (level.Get(i, j).type == Block::BOX)
                    boxes++;
                else if (level.Get(i, j).type == Block::ENDPOINT || level.Get(i, j).type == Block::ROBOT_ON_ENDPOINT)
                    endpoints++;
            }
        }

        if (!hasRobot)
        {
            libDialog::Warning(title, libFormat("The robot is missing on level %d.", n));
            break;
        }

        if (boxes != endpoints)
        {
            libDialog::Warning(title, libFormat("The amount of boxes and endpoints on level %d are not equal.", n));
            break;
        }

        n++;
    }

    s_move->Play();
}

/*
===================
Game::Reset
===================
*/
void Game::Reset()
{
    state = State::NONE;
    episode.Clear();
    gameLevel.Clear();
    history.PopAll();
    currentLevel = 1;
    gameCompleted = false;
}

/*
===================
Game::RestartLevel
===================
*/
void Game::RestartLevel()
{
    if (state != State::GAME && state != State::EDITOR_PLAY) return;

    gameLevel = episode.levels[currentLevel - 1];
    history.PopAll();
    robot.Set(-1, -1);

    for (int i = 0; i < LEVEL_WIDTH; i++)
    {
        for (int j = 0; j < LEVEL_HEIGHT; j++)
        {
            path[i][j] = MOVE_BLOCKED;

            if (gameLevel.Get(i, j).type == Block::ROBOT || gameLevel.Get(i, j).type == Block::ROBOT_ON_ENDPOINT)
            {
                robot.x = i;
                robot.y = j;
            }
        }
    }

    moveTimer.Restart();
}

/*
===================
Game::SetLevel
===================
*/
void Game::SetLevel(int level)
{
    if (level <= 0) level = 1;
    if (level > episode.levels.Size()) return;

    currentLevel = level;
    RestartLevel();
}

/*
===================
Game::PreviousLevel
===================
*/
void Game::PreviousLevel()
{
    SetLevel(currentLevel - 1);
}

/*
===================
Game::NextLevel
===================
*/
void Game::NextLevel()
{
    // Returns if the next level is locked
    if (state != State::EDITOR && currentLevel + 1 > episode.unlocked.Get())
        return;

    SetLevel(currentLevel + 1);
}

/*
===================
Game::AddLevel
===================
*/
void Game::AddLevel()
{
    if (state == State::EDITOR)
    {
        episode.levels.Insert(Level(), currentLevel);
        NextLevel();
    }
}

/*
===================
Game::DeleteLevel
===================
*/
void Game::DeleteLevel()
{
    if (state == State::EDITOR)
    {
        episode.levels.RemoveIndex(currentLevel - 1);
        if (episode.levels.IsEmpty()) AddLevel();
        PreviousLevel();
    }
}

/*
===================
Game::IsCompleted
===================
*/
bool Game::IsCompleted() const
{
    return gameCompleted;
}

/*
===================
Game::IsCursorInField
===================
*/
bool Game::IsCursorInField() const
{
    return mouse.x >= 0 && mouse.y >= 0 && mouse.x < LEVEL_WIDTH && mouse.y < LEVEL_HEIGHT;
}

/*
===================
Game::GetState
===================
*/
Game::State Game::GetState()
{
    return state;
}

/*
===================
Game::GetLevel
===================
*/
Level &Game::GetLevel()
{
    if (state == State::EDITOR)
        return episode.levels[currentLevel - 1];
    else
        return gameLevel;
}

/*
===================
Game::FindPath
===================
*/
moveType_t Game::FindPath(int (&path)[LEVEL_WIDTH][LEVEL_HEIGHT], int x, int y)
{
    if (x < 0 || y < 0 || x >= LEVEL_WIDTH || y >= LEVEL_HEIGHT)
        return MOVE_NONE;

    int map[LEVEL_WIDTH][LEVEL_HEIGHT];
    int step = 0;
    bool found = true;
    moveType_t moveType = MOVE_NONE;
    libVec2i pos(robot.x, robot.y);
    const auto &level = GetLevel();

    // No robot on a level
    if (robot.x < 0 || robot.y < 0 || robot.x >= LEVEL_WIDTH || robot.y >= LEVEL_HEIGHT)
        return MOVE_NONE;

    // Hovering over the robot
    if (robot.x == x && robot.y == y)
        return MOVE_NONE;

    // Hovering over walls or boxes
    if (level.Get(x, y).type == Block::WALL || level.Get(x, y).type == Block::BOX || level.Get(x, y).type == Block::BOX_ON_ENDPOINT)
        return MOVE_NONE;

    // Inits a wave map
    for (int i = 0; i < LEVEL_WIDTH; i++)
    {
        for (int j = 0; j < LEVEL_HEIGHT; j++)
        {
            if (level.Get(i, j).type == Block::WALL || level.Get(i, j).type == Block::BOX || level.Get(i, j).type == Block::BOX_ON_ENDPOINT)
                map[i][j] = -2;
            else
                map[i][j] = -1;

            path[i][j] = MOVE_BLOCKED;
        }
    }

    bool canMove = true;
    bool moveBox = false;

    // Checks if we can move a box horizontally to the target
    if (robot.x == x)
    {
        // If the target is located above the robot
        if (y < robot.y)
        {
            // Only move if we have only one box between the target
            for (int i = y, boxes = 0; i < robot.y; i++)
            {
                if (level.Get(robot.x, i).type == Block::BOX || level.Get(robot.x, i).type == Block::BOX_ON_ENDPOINT)
                    boxes++;

                if (level.Get(robot.x, i).type == Block::WALL || boxes >= 2)
                {
                    canMove = false;
                    moveType = MOVE_BLOCKED;
                    break;
                }
            }

            if (canMove)
            {
                for (int i = y; i < robot.y; i++)
                {
                    if (level.Get(robot.x, i).type == Block::BOX || level.Get(robot.x, i).type == Block::BOX_ON_ENDPOINT)
                    {
                        map[robot.x][i] = -1;
                        moveType = MOVE_BOX;
                        if (i > y) moveBox = true;
                    }
                }
            }

            if (moveBox) y++;
        }
        // If a target is located under the robot
        else
        {
            // Only move if we have only one box between the target
            for (int i = y, boxes = 0; i > robot.y; i--)
            {
                if (level.Get(robot.x, i).type == Block::BOX || level.Get(robot.x, i).type == Block::BOX_ON_ENDPOINT)
                {
                    boxes++;
                }

                if (level.Get(robot.x, i).type == Block::WALL || boxes >= 2)
                {
                    canMove = false;
                    moveType = MOVE_BLOCKED;
                    break;
                }
            }

            if (canMove)
            {
                for (int i = y; i > robot.y; i--)
                {
                    if (level.Get(robot.x, i).type == Block::BOX || level.Get(robot.x, i).type == Block::BOX_ON_ENDPOINT)
                    {
                        map[robot.x][i] = -1;
                        moveType = MOVE_BOX;
                        if (y > i) moveBox = true;
                    }
                }
            }

            if (moveBox) y--;
        }
    }
    // Checks if we can move a box vertically to the target
    else if (robot.y == y)
    {
        // If a target is located on the left of the robot
        if (x < robot.x)
        {
            // Only move if we have only one box between the target
            for (int i = x, boxes = 0; i < robot.x; i++)
            {
                if (level.Get(i, robot.y).type == Block::BOX || level.Get(i, robot.y).type == Block::BOX_ON_ENDPOINT)
                {
                    boxes++;
                }

                if (level.Get(i, robot.y).type == Block::WALL || boxes >= 2)
                {
                    canMove = false;
                    moveType = MOVE_BLOCKED;
                    break;
                }
            }

            if (canMove)
            {
                for (int i = x; i < robot.x; i++)
                {
                    if (level.Get(i, robot.y).type == Block::BOX || level.Get(i, robot.y).type == Block::BOX_ON_ENDPOINT)
                    {
                        map[i][robot.y] = -1;
                        moveType = MOVE_BOX;
                        if (x < i) moveBox = true;
                    }
                }
            }

            if (moveBox) x++;
        }
        // If a target is located on the right of the robot
        else
        {
            // Only move if we have only one box between the target
            for (int i = x, boxes = 0; i > robot.x; i--)
            {
                if (level.Get(i, robot.y).type == Block::BOX || level.Get(i, robot.y).type == Block::BOX_ON_ENDPOINT)
                {
                    boxes++;
                }

                if (level.Get(i, robot.y).type == Block::WALL || boxes >= 2)
                {
                    canMove = false;
                    moveType = MOVE_BLOCKED;
                    break;
                }
            }

            if (canMove)
            {
                for (int i = x; i > robot.x; i--)
                {
                    if (level.Get(i, robot.y).type == Block::BOX || level.Get(i, robot.y).type == Block::BOX_ON_ENDPOINT)
                    {
                        map[i][robot.y] = -1;
                        moveType = MOVE_BOX;
                        if (x > i) moveBox = true;
                    }
                }
            }

            if (moveBox) x--;
        }
    }

    // Sets a start for finding the target
    map[x][y] = 0;

    // Makes a wave from the target to the robot
    while (map[pos.x][pos.y] == -1 && found)
    {
        found = false;
        step++;

        for (int i = 0; i < LEVEL_WIDTH; i++)
        {
            for (int j = 0; j < LEVEL_HEIGHT; j++)
            {
                if (map[i][j] != step - 1) continue;

                // Up
                if (j - 1 >= 0)
                {
                    if (map[i][j - 1] == -1)
                    {
                        map[i][j - 1] = step;
                        found = true;
                    }
                }
                // Down
                if (j + 1 < LEVEL_HEIGHT)
                {
                    if (map[i][j + 1] == -1)
                    {
                        map[i][j + 1] = step;
                        found = true;
                    }
                }
                // Left
                if (i - 1 >= 0)
                {
                    if (map[i - 1][j] == -1)
                    {
                        map[i - 1][j] = step;
                        found = true;
                    }
                }
                // Right
                if (i + 1 < LEVEL_WIDTH)
                {
                    if (map[i + 1][j] == -1)
                    {
                        map[i + 1][j] = step;
                        found = true;
                    }
                }
            }
        }
    }

    // Makes the closest path from the robot to the target
    for (int n = 1; step >= 0; step--)
    {
        if (map[pos.x][pos.y] != step + 1) continue;

        // Up
        if (pos.y - 1 >= 0)
        {
            if (map[pos.x][pos.y - 1] == step)
            {
                path[pos.x][pos.y - 1] = n;
                n++;
                pos.y--;
            }
        }
        // Down
        if (pos.y + 1 < LEVEL_HEIGHT)
        {
            if (map[pos.x][pos.y + 1] == step)
            {
                path[pos.x][pos.y + 1] = n;
                n++;
                pos.y++;
            }
        }
        // Left
        if (pos.x - 1 >= 0)
        {
            if (map[pos.x - 1][pos.y] == step)
            {
                path[pos.x - 1][pos.y] = n;
                n++;
                pos.x--;
            }
        }
        // Right
        if (pos.x + 1 < LEVEL_WIDTH)
        {
            if (map[pos.x + 1][pos.y] == step)
            {
                path[pos.x + 1][pos.y] = n;
                n++;
                pos.x++;
            }
        }
    }   

    if (!found)
        return MOVE_BLOCKED;
    if (found && (moveType == MOVE_BLOCKED || moveType == MOVE_NONE))
        return MOVE;
    else
        return moveType;
}
