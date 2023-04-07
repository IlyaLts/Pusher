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
#include "Game.h"
#include "Menu.h"

/*
===================
Game::Game
===================
*/
Game::Game()
{
    state = NOT_STARTED;
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
    LIB_CHECK(engine->Get(tex_moveBlocked.Get(), DATA_PACK "Textures/MoveBlocked.tga"));
    LIB_CHECK(engine->Get(tex_movePath.Get(), DATA_PACK "Textures/MovePath.tga"));
    LIB_CHECK(engine->Get(tex_move.Get(), DATA_PACK "Textures/Move.tga"));
    LIB_CHECK(engine->Get(tex_moveBox.Get(), DATA_PACK "Textures/MoveBox.tga"));
    LIB_CHECK(engine->Get(tex_box.Get(), DATA_PACK "Textures/Box.tga"));
    LIB_CHECK(engine->Get(tex_endpoint.Get(), DATA_PACK "Textures/Endpoint.tga"));
    LIB_CHECK(engine->Get(tex_wall.Get(), DATA_PACK "Textures/Wall.tga"));
    LIB_CHECK(engine->Get(tex_floor.Get(), DATA_PACK "Textures/Floor.tga"));
    LIB_CHECK(engine->Get(spr_robot.Get(), DATA_PACK "Textures/Robot/Robot.tga"));
    LIB_CHECK(engine->Get(snd_move.Get(), DATA_PACK "Sounds/Move.wav"));
    LIB_CHECK(engine->Get(snd_levelComplete.Get(), DATA_PACK "Sounds/LevelComplete.wav"));

    engine->Get(mesh_moveBlocked.Get());
    engine->Get(mesh_movePath.Get());
    engine->Get(mesh_move.Get());
    engine->Get(mesh_moveBox.Get());
    engine->Get(mesh_box.Get());
    engine->Get(mesh_endpoint.Get());
    engine->Get(mesh_wall.Get());
    engine->Get(mesh_floor.Get());

    spr_robot->SetDuration(ROBOT_ANIMATION_DURATION);
    spr_robot->Play();

    moveMouseDelay = cfg.GetFloat("MoveMouseDelay", DEFAULT_MOVE_DELAY);
    moveKeyDelay = cfg.GetFloat("MoveKeyDelay", DEFAULT_KEY_DELAY);

    return true;
}

/*
===================
Game::Draw
===================
*/
void Game::Draw()
{
    mesh_moveBlocked->Clear();
    mesh_movePath->Clear();
    mesh_move->Clear();
    mesh_moveBox->Clear();
    mesh_box->Clear();
    mesh_endpoint->Clear();
    mesh_wall->Clear();
    mesh_floor->Clear();

    if (state == EDITOR_NEW_NAME)
    {
        font->SetAlign(LIB_CENTER);
        font->SetColor(LIB_COLOR_WHITE);
        font->SetSize(libCast<int>(screenSize.y * FONT_H2_SCALE_RATIO));
        font->Print2D(screenSize.x / 2.0f, font->LineSpacing(), "Please enter your episode name.");
        font->Print2D(screenSize.x / 2.0f, screenSize.y / 2.0f, episode.name.Get());

        if (episodeAlreadyExists)
        {
            font->SetColor(LIB_COLOR_RED);
            font->SetSize(libCast<int>(screenSize.y * FONT_TEXT_SCALE_RATIO));

            float x = screenSize.x / 2.0f;
            float y = screenSize.y - font->LineSpacing();
            font->Print2D(x, y, "Episode with given name already exists!");
        }

        return;
    }

    libColor defaultColor(1.0f, 1.0f, 1.0f, 1.0f);
    libColor transparentColor(1.0f, 1.0f, 1.0f, 0.75f);
    libColor darkenedColor(0.75f, 0.75f, 0.75f, 1.0f);
    libQuad block(libVertex(libVec2(0.0f, 0.0f), libVec2(0.0f, 0.0f)), libVertex(tileSize, libVec2(1.0f, 1.0f)));
    libVec2 hoverOver(gameScreenPos + mouse * libVec2i(tileSize));

    mesh_endpoint->Clear();
    mesh_wall->Clear();
    mesh_floor->Clear();

    // Level
    for (int i = 0; i < LEVEL_WIDTH; i++)
    {
        for (int j = 0; j < LEVEL_HEIGHT; j++)
        {
            float x = libCast<float>(gameScreenPos.x + i * tileSize.x);
            float y = libCast<float>(gameScreenPos.y + j * tileSize.y);
            
            // Floor
            if (GetLevel().GetBlock(i, j).floor)
                mesh_floor->Add(block, libVec3(x, y, 0.0f));

            // Blocks
            switch (GetLevel().GetBlock(i, j).type)
            {
                case Block::BOX:
                {
                    mesh_box->Add(block, libVec3(x, y, 0.0f));
                    break;
                }
                case Block::WALL:
                {
                    mesh_wall->Add(block, libVec3(x, y, 0.0f));
                    break;
                }
                case Block::ENDPOINT:
                {
                    mesh_endpoint->Add(block, libVec3(x, y, 0.0f));
                    break;
                }
                case Block::BOX_ON_ENDPOINT:
                {
                    mesh_endpoint->Add(block, libVec3(x, y, 0.0f));
                    block.SetColor(darkenedColor);
                    mesh_box->Add(block, libVec3(x, y, 0.0f));
                    block.SetColor(defaultColor);
                    break;
                }
                case Block::ROBOT_ON_ENDPOINT:
                {
                    mesh_endpoint->Add(block, libVec3(x, y, 0.0f));
                    break;
                }
            }

            // Path dots
            if ((state == IN_GAME || state == EDITOR_PLAYTEST) && path[i][j] > 0)
                mesh_movePath->Add(block, libVec3(x, y, 0.0f));
        }
    }

    if (IsCursorInField())
    {
        // Game cursor
        if (state == IN_GAME || state == EDITOR_PLAYTEST)
        {
            int temp[LEVEL_WIDTH][LEVEL_HEIGHT];
            moveType_t result = FindPath(temp, mouse.x, mouse.y);

            switch (result)
            {
            case MOVE:
            {
                mesh_move->Add(block, libVec3(hoverOver.x, hoverOver.y, 0.0f));
                break;
            }
            case MOVE_BOX:
            {
                mesh_moveBox->Add(block, libVec3(hoverOver.x, hoverOver.y, 0.0f));
                break;
            }
            case MOVE_BLOCKED:
            {
                mesh_moveBlocked->Add(block, libVec3(hoverOver.x, hoverOver.y, 0.0f));
                break;
            }
            }
        }
        // Brush tool
        else if (state == EDITOR)
        {
            block.SetColor(transparentColor);

            switch (currentBlock)
            {
            case Block::BOX:
            {
                mesh_box->Add(block, libVec3(hoverOver.x, hoverOver.y, 0.0f));
                break;
            }
            case Block::WALL:
            {
                mesh_wall->Add(block, libVec3(hoverOver.x, hoverOver.y, 0.0f));
                break;
            }
            case Block::ENDPOINT:
            {
                mesh_endpoint->Add(block, libVec3(hoverOver.x, hoverOver.y, 0.0f));
                break;
            }
            case Block::BOX_ON_ENDPOINT:
            {
                mesh_endpoint->Add(block, libVec3(hoverOver.x, hoverOver.y, 0.0f));
                mesh_box->Add(block, libVec3(hoverOver.x, hoverOver.y, 0.0f));
                break;
            }
            case Block::ROBOT_ON_ENDPOINT:
            {
                mesh_endpoint->Add(block, libVec3(hoverOver.x, hoverOver.y, 0.0f));
                break;
            }
            }

            block.SetColor(defaultColor);
        }
    }

    engine->Draw(mesh_floor.Get(), tex_floor.Get(), true);
    engine->Draw(mesh_wall.Get(), tex_wall.Get(), true);
    engine->Draw(mesh_endpoint.Get(), tex_endpoint.Get(), true);
    engine->Draw(mesh_moveBox.Get(), tex_moveBox.Get(), true);
    engine->Draw(mesh_movePath.Get(), tex_movePath.Get(), true);
    engine->Draw(mesh_moveBlocked.Get(), tex_moveBlocked.Get(), true);
    engine->Draw(mesh_move.Get(), tex_move.Get(), true);
    engine->Draw(mesh_box.Get(), tex_box.Get(), true);

    // Level
    for (int i = 0; i < LEVEL_WIDTH; i++)
    {
        for (int j = 0; j < LEVEL_HEIGHT; j++)
        {
            float x = libCast<float>(gameScreenPos.x + i * tileSize.x);
            float y = libCast<float>(gameScreenPos.y + j * tileSize.y);

            // Floor
            if (GetLevel().GetBlock(i, j).floor)
                mesh_floor->Add(block, libVec3(x, y, 0.0f));

            // Blocks
            switch (GetLevel().GetBlock(i, j).type)
            {
            case Block::ROBOT:
            case Block::ROBOT_ON_ENDPOINT:
            {
                spr_robot->Draw2DQuad(block, x, y, 0.1f);
                break;
            }
            }
        }
    }

    if (IsCursorInField())
    {
        // Brush tool
        if (state == EDITOR)
        {
            block.SetColor(transparentColor);

            switch (currentBlock)
            {
            case Block::ROBOT:
            case Block::ROBOT_ON_ENDPOINT:
            {
                spr_robot->Draw2DQuad(block, hoverOver.x, hoverOver.y, 0.1f);
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
    if (state == EDITOR_NEW_NAME)
    {
        EditorNewNameUpdate();
        return;
    }

    engine->GetMousePos(mouse.x, mouse.y);
    mouse -= gameScreenPos;

    if (mouse.x >= 0 && mouse.y >= 0 && mouse.x < gameScreenSize.x && mouse.y < gameScreenSize.y)
    {
        mouse.x = libCast<int>(mouse.x / tileSize.x);
        mouse.y = libCast<int>(mouse.y / tileSize.y);
    }
    else
    {
        mouse.Set(-1, -1);
    }

    // Switches to Game/Editor modes
#ifdef LIB_DEBUG
    if (engine->IsKey(LIBK_CTRL))
#else
    if (engine->IsKey(LIBK_CTRL) && (State() == EDITOR || State() == EDITOR_PLAYTEST))
#endif
    {
        if (engine->IsKeyPressed(LIBK_G))
        {
            SetState((State() == IN_GAME || state == EDITOR_PLAYTEST) ? EDITOR : EDITOR_PLAYTEST);
        }
    }

    if ((state == IN_GAME) || (state == EDITOR_PLAYTEST))
        GameUpdate();
    else if (state == EDITOR)
        EditorUpdate();
}

/*
===================
Game::GameUpdate
===================
*/
void Game::GameUpdate()
{
    engine->SetState(LIB_WINDOW_TITLE, libFormat("Pusher - %s - Level: %d", episode.name.Get(), currentLevel));

#ifdef LIB_DEBUG
    if (GetLevel().IsCompleted() || (engine->IsKey(LIBK_CTRL) && engine->IsKeyPressed(LIBK_ENTER)))
#else
    if (GetLevel().IsCompleted())
#endif
    {
        if (currentLevel == episode.levels.Size())
        {
            if (state == EDITOR_PLAYTEST)
                state = EDITOR;
            else
                gameCompleted = true;
        }

        if (state != EDITOR_PLAYTEST)
        {
            if (episode.unlocked.Get() == currentLevel)
                episode.unlocked = currentLevel + 1;

            Episode::SaveNumberOfUnlockedLevels(episode);
        }

        snd_levelComplete->Play();
        NextLevel();
    }

    InputUpdate();

    // Moves the robot up
    if (robot.y > 0)
    {
        if ((path[robot.x][robot.y - 1] > path[robot.x][robot.y] && canMoveByMouse) || moveKeyDir == UP)
        {
            MoveRobot(libVec2i(0, -1));
            return;
        }
    }

    // Moves the robot down
    if (robot.y < LEVEL_HEIGHT - 1)
    {
        if ((path[robot.x][robot.y + 1] > path[robot.x][robot.y] && canMoveByMouse) || moveKeyDir == DOWN)
        {
            MoveRobot(libVec2i(0, 1));
            return;
        }
    }

    // Moves the robot left
    if (robot.x > 0)
    {
        if ((path[robot.x - 1][robot.y] > path[robot.x][robot.y] && canMoveByMouse) || moveKeyDir == LEFT)
        {
            MoveRobot(libVec2i(-1, 0));
            return;
        }
    }
    
    // Moves the robot right
    if (robot.x < LEVEL_WIDTH - 1)
    {
        if ((path[robot.x + 1][robot.y] > path[robot.x][robot.y] && canMoveByMouse) || moveKeyDir == RIGHT)
        {
            MoveRobot(libVec2i(1, 0));
            return;
        }
    }
}

/*
===================
Game::InputUpdate
===================
*/
void Game::InputUpdate()
{
    moveKeyDir = NEUTRAL;
    static moveDirection_t movePressedKeyDir = NEUTRAL;
    static libVec2i mouseTarget(-1, -1);

    canMoveByMouse = (moveTimer.Milliseconds() >= moveMouseDelay);
    bool canMoveByKey = (moveTimer.Milliseconds() >= moveKeyDelay);

    if (engine->IsKey(LIBK_CTRL))
    {
        if (engine->IsKeyPressed(LIBK_R))
            RestartLevel();

        if (engine->IsKeyPressed(LIBK_LBRACKET))
            PreviousLevel();

        if (engine->IsKeyPressed(LIBK_RBRACKET))
            NextLevel();

        if (engine->IsKeyPressed(LIBK_Z))
            UndoLatestChange();
    }

    // Sets a path from the robot to the mouse cursor position
    if (engine->IsKeyPressed(LIBK_MOUSE_LEFT))
    {
        mouseTarget.Set(mouse.x, mouse.y);
        FindPath(path, mouse.x, mouse.y);
    }

    // Determines the direction to move the robot based on a newly pressed key
    if (engine->IsKeyPressed(LIBK_UP) || engine->IsKeyPressed(LIBK_W))
        movePressedKeyDir = UP;

    if (engine->IsKeyPressed(LIBK_DOWN) || engine->IsKeyPressed(LIBK_S))
        movePressedKeyDir = DOWN;

    if (engine->IsKeyPressed(LIBK_LEFT) || engine->IsKeyPressed(LIBK_A))
        movePressedKeyDir = LEFT;

    if (engine->IsKeyPressed(LIBK_RIGHT) || engine->IsKeyPressed(LIBK_D))
        movePressedKeyDir = RIGHT;

    // If the robot moves to the target after pressing the mouse
    if (mouseTarget.x >= 0 && mouseTarget.y >= 0)
    {
        // If the robot reached the target
        if (path[mouseTarget.x][mouseTarget.y] == MOVE_BLOCKED)
            mouseTarget.Set(-1, -1);

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

    // Determines the final movement direction for the robot based on keyboard input
    if (canMoveByKey)
    {
        // Did a new key get pressed?
        if (movePressedKeyDir)
        {
            moveKeyDir = movePressedKeyDir;
            movePressedKeyDir = NEUTRAL;
        }
        else
        {
            if (engine->IsKey(LIBK_UP) || engine->IsKey(LIBK_W))
                moveKeyDir = UP;

            if (engine->IsKey(LIBK_DOWN) || engine->IsKey(LIBK_S))
                moveKeyDir = DOWN;

            if (engine->IsKey(LIBK_LEFT) || engine->IsKey(LIBK_A))
                moveKeyDir = LEFT;

            if (engine->IsKey(LIBK_RIGHT) || engine->IsKey(LIBK_D))
                moveKeyDir = RIGHT;
        }
    }
}

/*
===================
Game::MoveRobot
===================
*/
void Game::MoveRobot(const libVec2i &direction)
{
    moveTimer.Restart();
    movedSaved = false;

    Level &level = GetLevel();
    libVec2i targetCoord(robot + direction);
    libVec2i afterCoord(robot + direction * 2);
    const Block &current = level.GetBlock(robot.x, robot.y);
    const Block &target = level.GetBlock(targetCoord.x, targetCoord.y);
    const Block &after = level.GetBlock(afterCoord.x, afterCoord.y);

    // Moves a box
    if (target.IsBox() && after.CanMoveBoxes())
    {
        SaveHistory();

        if (target.type == Block::BOX)
            level.SetBlockType(targetCoord.x, targetCoord.y, Block::EMPTY);

        if (target.type == Block::BOX_ON_ENDPOINT)
            level.SetBlockType(targetCoord.x, targetCoord.y, Block::ENDPOINT);

        if (after.type == Block::EMPTY)
            level.SetBlockType(afterCoord.x, afterCoord.y, Block::BOX);

        if (after.type == Block::ENDPOINT)
            level.SetBlockType(afterCoord.x, afterCoord.y, Block::BOX_ON_ENDPOINT);
    }

    // Moves the robot
    if (target.CanMoveBoxes())
    {
        SaveHistory();

        if (current.type == Block::ROBOT)
            level.SetBlockType(robot.x, robot.y, Block::EMPTY);

        if (current.type == Block::ROBOT_ON_ENDPOINT)
            level.SetBlockType(robot.x, robot.y, Block::ENDPOINT);

        if (target.type == Block::EMPTY)
            level.SetBlockType(targetCoord.x, targetCoord.y, Block::ROBOT);

        if (target.type == Block::ENDPOINT)
            level.SetBlockType(targetCoord.x, targetCoord.y, Block::ROBOT_ON_ENDPOINT);

        robot += direction;
        path[robot.x][robot.y] = MOVE_BLOCKED;
        snd_move->Play();
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
        engine->SetState(LIB_WINDOW_TITLE,
                         libFormat("Pusher Editor: Level: %d (%d:%d)", currentLevel, mouse.x + 1, mouse.y + 1));
    }
    else
    {
        engine->SetState(LIB_WINDOW_TITLE, libFormat("Pusher Editor: Level: %d", currentLevel));
    }

    if (engine->IsKey(LIBK_CTRL))
    {
        if (engine->IsKeyPressed(LIBK_A))
            AddLevel();

        if (engine->IsKeyPressed(LIBK_S))
            SaveEpisode();

        if (engine->IsKeyPressed(LIBK_D))
            DeleteLevel();

        if (engine->IsKeyPressed(LIBK_LBRACKET))
            PreviousLevel();

        if (engine->IsKeyPressed(LIBK_RBRACKET))
            NextLevel();

        if (engine->IsKeyPressed(LIBK_Q))
            MoveLevelBackward();

        if (engine->IsKeyPressed(LIBK_E))
            MoveLevelForward();

        if (engine->IsKeyPressed(LIBK_C))
            DuplicateLevel();

        if (engine->IsKeyPressed(LIBK_Z))
            UndoLatestChange();
    }

    if (engine->IsKeyPressed(LIBK_UP))
        GetLevel().ShiftUp();

    if (engine->IsKeyPressed(LIBK_DOWN))
        GetLevel().ShiftDown();

    if (engine->IsKeyPressed(LIBK_LEFT))
        GetLevel().ShiftLeft();

    if (engine->IsKeyPressed(LIBK_RIGHT))
        GetLevel().ShiftRight();

    // The easiest fix for the touchpad as it has scrolling inertia and switches brush types way too fast
    static int wheelZeros = 0;
    static bool wheelZero = true;

    wheelZeros = (engine->MouseWheel() ? 0 : (wheelZeros <= 2 ? wheelZeros + 1 : wheelZeros));
    
    if (wheelZeros > 2)
        wheelZero = true;

    // Switches brush types - Mouse wheel up
    if (engine->MouseWheel() > 0 && wheelZero)
    {
        wheelZero = false;
        currentBlock++;

        if (currentBlock == Block::TYPE_COUNT)
        {
            currentBlock = Block::EMPTY;
            currentBlock++;
        }
    }
    // Mouse wheel down
    else if (engine->MouseWheel() < 0 && wheelZero)
    {
        wheelZero = false;
        currentBlock--;

        if (currentBlock == Block::EMPTY)
        {
            currentBlock = Block::TYPE_COUNT;
            currentBlock--;
        }
    }

    if (IsCursorInField())
    {
        Block::type_t curType = level.GetBlock(mouse.x, mouse.y).type;

        // Adds the current block to the current level
        if (engine->IsKey(LIBK_MOUSE_LEFT) && curType != libCast<Block::type_t>(currentBlock))
        {
            history.Push(episode.levels[currentLevel - 1]);

            // Doesn't allow to have more than one robot per a level
            if (currentBlock == Block::ROBOT || currentBlock == Block::ROBOT_ON_ENDPOINT)
            {
                for (int i = 0; i < LEVEL_WIDTH; i++)
                {
                    for (int j = 0; j < LEVEL_HEIGHT; j++)
                    {
                        Block::type_t type = level.GetBlock(i, j).type;

                        if (type == Block::ROBOT || type == Block::ROBOT_ON_ENDPOINT)
                            level.SetBlockType(i, j, Block::EMPTY);
                    }
                }
            }

            level.SetBlockType(mouse.x, mouse.y, libCast<Block::type_t>(currentBlock));
            level.CalculateFloor();
        }

        // Clears a block from the current level
        if (engine->IsKey(LIBK_MOUSE_RIGHT) && curType != Block::EMPTY)
        {
            if (curType == Block::ROBOT)
                robot.Set(-1, -1);

            history.Push(episode.levels[currentLevel - 1]);
            level.SetBlockType(mouse.x, mouse.y, Block::EMPTY);
            level.CalculateFloor();
        }
    }
}

/*
===================
Game::EditorNewNameUpdate
===================
*/
void Game::EditorNewNameUpdate()
{
    char c = engine->KeyValue(engine->CurrentKey());

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
            state = EDITOR;
            SaveEpisode();
        }
        else
        {
            episodeAlreadyExists = true;
        }
    }
}

/*
===================
Game::SetState
===================
*/
void Game::SetState(Game::state_t state)
{
    this->state = state;
    history.PopAll();
    gameCompleted = false;

    if (state == IN_GAME || state == EDITOR_PLAYTEST)
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
    if (state != EDITOR)
        return;

    // If filepath is unknown, then asks for a new episode name
    if (!path && episode.filepath.IsEmpty())
    {
        state = EDITOR_NEW_NAME;
        return;
    }

    if (!Episode::Save(episode, path ? path : episode.filepath.Get()))
        libDialog::Error("Error", "Couldn't save the episode.");

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
                Block::type_t type = level.GetBlock(i, j).type;

                if (type == Block::ROBOT || type == Block::ROBOT_ON_ENDPOINT)
                    hasRobot = true;

                if (type == Block::BOX)
                    boxes++;
                else if (type == Block::ENDPOINT || type == Block::ROBOT_ON_ENDPOINT)
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

    snd_move->Play();
}

/*
===================
Game::Reset
===================
*/
void Game::Reset()
{
    state = NOT_STARTED;
    episode.Reset();
    gameLevel.Reset();
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
    if (state != IN_GAME && state != EDITOR_PLAYTEST)
        return;

    gameLevel = episode.levels[currentLevel - 1];
    history.PopAll();
    robot.Set(-1, -1);

    for (int i = 0; i < LEVEL_WIDTH; i++)
    {
        for (int j = 0; j < LEVEL_HEIGHT; j++)
        {
            path[i][j] = MOVE_BLOCKED;
            Block::type_t type = gameLevel.GetBlock(i, j).type;

            if (type == Block::ROBOT || type == Block::ROBOT_ON_ENDPOINT)
                robot.Set(i, j);
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
    if (level <= 0)
        level = 1;

    if (level > episode.levels.Size())
        return;

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
    // If the next level is locked
    if (state != EDITOR && currentLevel + 1 > episode.unlocked.Get())
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
    if (state != EDITOR)
        return;

    episode.levels.Insert(Level(), currentLevel);
    NextLevel();
}

/*
===================
Game::DuplicateLevel
===================
*/
void Game::DuplicateLevel()
{
    if (state != EDITOR)
        return;

    int temp = currentLevel;
    AddLevel();
    episode.levels[currentLevel - 1] = episode.levels[temp - 1];
}

/*
===================
Game::MoveLevelForward
===================
*/
void Game::MoveLevelForward()
{
    if (currentLevel >= episode.levels.Size())
        return;

    Level temp(episode.levels[currentLevel - 1]);
    episode.levels[currentLevel - 1] = episode.levels[currentLevel];
    episode.levels[currentLevel] = temp;
    currentLevel++;
}

/*
===================
Game::MoveLevelBackward
===================
*/
void Game::MoveLevelBackward()
{
    if (currentLevel <= 1)
        return;

    Level temp(episode.levels[currentLevel - 1]);
    episode.levels[currentLevel - 1] = episode.levels[currentLevel - 2];
    episode.levels[currentLevel - 2] = temp;
    currentLevel--;
}

/*
===================
Game::DeleteLevel
===================
*/
void Game::DeleteLevel()
{
    if (state != EDITOR)
        return;
        
    episode.levels.RemoveIndex(currentLevel - 1);

    if (episode.levels.IsEmpty())
        AddLevel();

    PreviousLevel();
}

/*
===================
Game::SaveHistory
===================
*/
void Game::SaveHistory()
{
    if (movedSaved)
        return;

    history.Push(GetLevel());
    movedSaved = true;
}

/*
===================
Game::UndoLatestChange
===================
*/
void Game::UndoLatestChange()
{
    if (history.IsEmpty())
        return;

    if (state == IN_GAME || state == EDITOR_PLAYTEST)
    {
        GetLevel() = history.Get();
        history.Pop();
        snd_move->Play();

        for (int i = 0; i < LEVEL_WIDTH; i++)
        {
            for (int j = 0; j < LEVEL_HEIGHT; j++)
            {
                Block::type_t type = GetLevel().GetBlock(i, j).type;
                path[i][j] = MOVE_BLOCKED;

                if (type == Block::ROBOT || type == Block::ROBOT_ON_ENDPOINT)
                    robot.Set(i, j);
            }
        }
    }
    else if (state == EDITOR || state == EDITOR_PLAYTEST)
    {
        GetLevel() = history.Get();
        history.Pop();
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
Game::State
===================
*/
Game::state_t Game::State()
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
    if (state == EDITOR)
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
    Block::type_t type = level.GetBlock(x, y).type;

    // No robot on a level
    if (robot.x < 0 || robot.y < 0 || robot.x >= LEVEL_WIDTH || robot.y >= LEVEL_HEIGHT)
        return MOVE_NONE;

    // Hovering over the robot
    if (robot.x == x && robot.y == y)
        return MOVE_NONE;

    // Hovering over walls or boxes
    if (type == Block::WALL || type == Block::BOX || type == Block::BOX_ON_ENDPOINT)
        return MOVE_NONE;

    // Initiates a wave map
    for (int i = 0; i < LEVEL_WIDTH; i++)
    {
        for (int j = 0; j < LEVEL_HEIGHT; j++)
        {
            Block::type_t type = level.GetBlock(i, j).type;

            if (type == Block::WALL || type == Block::BOX || type == Block::BOX_ON_ENDPOINT)
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
                Block::type_t type = level.GetBlock(robot.x, i).type;

                if (type == Block::BOX || type == Block::BOX_ON_ENDPOINT)
                    boxes++;

                if (type == Block::WALL || boxes >= 2)
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
                    Block::type_t type = level.GetBlock(robot.x, i).type;

                    if (type == Block::BOX || type == Block::BOX_ON_ENDPOINT)
                    {
                        map[robot.x][i] = -1;
                        moveType = MOVE_BOX;

                        if (i > y)
                            moveBox = true;
                    }
                }
            }

            if (moveBox)
                y++;
        }
        // If a target is located under the robot
        else
        {
            // Only move if we have only one box between the target
            for (int i = y, boxes = 0; i > robot.y; i--)
            {
                Block::type_t type = level.GetBlock(robot.x, i).type;

                if (type == Block::BOX || type == Block::BOX_ON_ENDPOINT)
                    boxes++;

                if (type == Block::WALL || boxes >= 2)
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
                    Block::type_t type = level.GetBlock(robot.x, i).type;

                    if (type == Block::BOX || type == Block::BOX_ON_ENDPOINT)
                    {
                        map[robot.x][i] = -1;
                        moveType = MOVE_BOX;

                        if (y > i)
                            moveBox = true;
                    }
                }
            }

            if (moveBox)
                y--;
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
                Block::type_t type = level.GetBlock(i, robot.y).type;

                if (type == Block::BOX || type == Block::BOX_ON_ENDPOINT)
                    boxes++;

                if (type == Block::WALL || boxes >= 2)
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
                    Block::type_t type = level.GetBlock(i, robot.y).type;

                    if (type == Block::BOX || type == Block::BOX_ON_ENDPOINT)
                    {
                        map[i][robot.y] = -1;
                        moveType = MOVE_BOX;

                        if (x < i)
                            moveBox = true;
                    }
                }
            }

            if (moveBox)
                x++;
        }
        // If a target is located on the right of the robot
        else
        {
            // Only move if we have only one box between the target
            for (int i = x, boxes = 0; i > robot.x; i--)
            {
                Block::type_t type = level.GetBlock(i, robot.y).type;

                if (type == Block::BOX || type == Block::BOX_ON_ENDPOINT)
                    boxes++;

                if (type == Block::WALL || boxes >= 2)
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
                    Block::type_t type = level.GetBlock(i, robot.y).type;

                    if (type == Block::BOX || type == Block::BOX_ON_ENDPOINT)
                    {
                        map[i][robot.y] = -1;
                        moveType = MOVE_BOX;

                        if (x > i)
                            moveBox = true;
                    }
                }
            }

            if (moveBox)
                x--;
        }
    }

    // Sets a start to the target
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
                if (map[i][j] != step - 1)
                    continue;

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
        if (map[pos.x][pos.y] != step + 1)
            continue;

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
