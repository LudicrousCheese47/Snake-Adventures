/*******************************************************************************************
*
*   raylib [core] example - basic window
*
*   Example complexity rating: [★☆☆☆] 1/4
*
*   Welcome to raylib!
*
*   To test examples, just press F6 and execute 'raylib_compile_execute' script
*   Note that compiled executable is placed in the same folder as .c file
*
*   To test the examples on Web, press F6 and execute 'raylib_compile_execute_web' script
*   Web version of the program is generated in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   Example originally created with raylib 1.0, last time updated with raylib 1.0
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2013-2026 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "time.h"
#include "float.h"
#include "math.h"
#include "Structures.h"
#include "UI_Functions.h"
#include "Definitions.h"
#include "Fruit_Functions.h"
#include "Player_Functions.h"
#include "Particle_Functions.h"


struct TextButton buttons[MAX_UI];
struct Box boxes[MAX_UI];
struct TextLabel labels[MAX_UI];
struct Food powerUps[POWER_UP_COUNT];
struct Food foods[FRUIT_COUNT];
struct Player bodies[MAX_P_LENGTH];
struct Particle particles[MAX_PARTICLES];
struct Splat splats[MAX_SPLATS];
struct TextLabel tempLabels[MAX_UI];

const float JUMPSCARE_DURATION = 0.25f;

Texture2D jumpscareTexture;
const float SCARED_DURATION = 4.0f;
bool directDebounce = false;

struct Player player1 = {
    0, 0, PLAYER_SIZE,
    false,
    PLAYER_H_DEFAULT_R, PLAYER_H_DEFAULT_G, PLAYER_H_DEFAULT_B, PLAYER_H_DEFAULT_A, 
    220, 220,
    220.0f, 220.0f,
    0
};

struct Food powerUps[] = {
    {"apple",       0, 0, (8+PLAYER_SIZE)/2,      255, 0, 0, 255,     3, 5.00, true, "Fruit",
    "A delicious apple, grants 3 energy"},
    {"purple grape",       0, 0, (2+PLAYER_SIZE)/2,      150, 0, 255, 255,  0.8, 100.00, true, "Fruit",
    "A juicy grape, grants 0.8 energy"},
    {"green grape",         0, 0, (2+PLAYER_SIZE)/2,      0, 135, 20, 255,   1.5, 75.00, true, "Fruit",
    "A sour grape, grants 1.5 energy, because sour grapes are better than purple grapes."},
    {"blueberry",           0, 0, (1+PLAYER_SIZE)/2,      0, 35, 255, 255,   0.4, 90.00, true, "Fruit",
    "A blueberry, grants 0.4 energy"},
    {"water melon", 0, 0, (12+PLAYER_SIZE)/2,     0, 185, 0, 255,   5, 2.00, true, "Fruit",
    "A water melon, grants 5 energy"},
    {"cheese",      0, 0, (4+PLAYER_SIZE)/2,      200, 200, 0, 255,  2, 20.00, true, "Fruit",
    "Cheesy cheese, grants 2 energy"},
    {"orange",      0, 0, (8+PLAYER_SIZE)/2,      255, 150, 0, 255,    3, 5.00, true, "Fruit",
    "A zesty sour orange, grants 3 energy"},
    {"posion apple",0, 0, (8+PLAYER_SIZE)/2,      105, 0, 0, 255,     4, 10.00, true, "Poison",
    "A poison apple will cause 4 snake segments to die slowly, do NOT eat it"},
    {"ghost apple", 0, 0, (0.5+PLAYER_SIZE)/2,      100, 100, 100, 20,  -1, 1.00, true, "Ghost", 
    "This ghost apple is haunted by a entity that will scare the ever living segment off of the snake, and then the snake will go crazy. Do the snake a favor by NOT eat this"}
};

struct Food foods[FRUIT_COUNT] = { 0 };

struct Player bodies[MAX_P_LENGTH] = { 0 };

struct Particle particles[MAX_PARTICLES] = { 0 };

struct Splat splats[MAX_SPLATS] = { 0 };

struct TextLabel tempLabels[MAX_UI] = { 0 };

// UI Init

struct TextButton buttons[MAX_UI] = {
    {
        "resume",
        (SCREEN_WIDTH - 200) / 2,
        ((SCREEN_HEIGHT - 100) / 2)-20,
        200,
        100,
        1.0f,
        1.0f,
        0, 255, 255, 255,
        0, 20, 255, 255,
        2,
        2.0f,
        24,
        0, 0, 0, 255,
        0, 0, 0, 255,
        false,
        true,
        "Resume Game",
        "main"
    },
    
    {
        "quit",
        (SCREEN_WIDTH - 200) / 2,
        ((SCREEN_HEIGHT - 100) / 2)+150,
        200,
        100,
        1.0f,
        1.0f,
        255, 200, 0, 255,
        255, 0, 0, 255,
        2,
        2.0f,
        24,
        0, 0, 0, 255,
        0, 0, 0, 255,
        false,
        true,
        "QUIT",
        "main"
    }
};

struct Box boxes[MAX_UI] = {
    {
        "main_menu_bg",
        0,
        0,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        0, 0, 255, 150,
        1,
        false,
        "main"
    }
};

struct TextLabel labels[MAX_UI] = { 
    {
        "score",
        40, 0,
        50, UI_BORDER_OFFSET-2,
        1.0f,
        1.0f,
        0.0,
        0.0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0,
        0,
        UI_BORDER_OFFSET-2,
        0, 0, 0, 0,
        0, 255, 0, 255,
        0, 0,
        true,
        "SCORE: ",
        "stats"
    },
    
    {
        "level",
        UI_BORDER_OFFSET*2+90, 0,
        50, UI_BORDER_OFFSET-2,
        1.0f,
        1.0f,
        0.0f,
        0.0f,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0,
        0,
        UI_BORDER_OFFSET-2,
        0, 0, 0, 0,
        255, 255, 255, 255,
        0, 0,
        true,
        "LEVEL: ",
        "stats"
    },
};

//UI Init

void resetGame(void) {
    player1.length = 0;
    player1.stomach = 0;
    player1.x = 240;
    player1.y = 240;
    player1.visualX = 240.0f;
    player1.visualY = 240.0f;
    direction = "right";
    score = 0;
    level = 1;
    scared = false;
    jumpscareActive = false;
    jumpscareTimer = 0.0f;
    scaredTimer = 0.0f;
    poisonTick = 4;
    poisoned = false;
    player1.r = PLAYER_H_DEFAULT_R;
    player1.g = PLAYER_H_DEFAULT_G;
    player1.b = PLAYER_H_DEFAULT_B;
    player1.a = PLAYER_H_DEFAULT_A;
    
    
    memset(bodies, 0, sizeof(struct Player) * MAX_P_LENGTH);
    memset(foods, 0, sizeof(struct Food) * FRUIT_COUNT);
    
    for (int i = 0; i < FRUIT_COUNT; i++) {
        foods[i].active = false;
    }
    Background();
    loadFoods();
    game = true;
}

Color getRandomBoardColor(void)
{
    return (Color){
        GetRandomValue(0, 15),
        GetRandomValue(0, 60),
        GetRandomValue(220, 255),
        255
    };
}

Color darken(Color c)
{
    c.r = (unsigned char)(c.r * 0.7f);
    c.g = (unsigned char)(c.g * 0.7f);
    c.b = (unsigned char)(c.b * 0.7f);
    return c;
}

void renderBackground(void) {
    int sqaures = ((SCREEN_WIDTH - UI_BORDER_OFFSET * 2) / PLAYER_SIZE) *
                  ((SCREEN_HEIGHT - UI_BORDER_OFFSET * 2) / PLAYER_SIZE);
    int x = UI_BORDER_OFFSET;
    int y = UI_BORDER_OFFSET;
    bool swapColors = false;
    Color color1 = getRandomBoardColor();
    Color color2 = darken(color1);
    for (int i = 0; i < sqaures; i++) {
        if (x >= SCREEN_WIDTH-UI_BORDER_OFFSET) {
            x = UI_BORDER_OFFSET; 
            y+=PLAYER_SIZE; 
        }
        if (swapColors == false) {
            Rectangle rec = (struct Rectangle){(float)x, (float)y, (float)PLAYER_SIZE, (float)PLAYER_SIZE};
            DrawRectangle(x, y, PLAYER_SIZE, PLAYER_SIZE, color1);
            DrawRectangleLinesEx(rec, 2.0, GetColor(0x000000CC));
            x+=PLAYER_SIZE;
            swapColors = true;
        } else {
            Rectangle rec = (struct Rectangle){(float)x, (float)y, (float)PLAYER_SIZE, (float)PLAYER_SIZE};
            DrawRectangle(x, y, PLAYER_SIZE, PLAYER_SIZE, color2);
            DrawRectangleLinesEx(rec, 2.0, GetColor(0x00000055));
            x+=PLAYER_SIZE;
            swapColors = false;
        }
    }
}

void Background(void) {
    BeginTextureMode(gridTexture);
        ClearBackground(BLANK);

        renderBackground();
    EndTextureMode();
}


//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake Adventures");
    SetExitKey(0);
    InitAudioDevice();
    gridTexture = LoadRenderTexture(SCREEN_WIDTH-UI_BORDER_OFFSET, SCREEN_HEIGHT-UI_BORDER_OFFSET);
    snakeCanvas = LoadRenderTexture(SCREEN_WIDTH-UI_BORDER_OFFSET, SCREEN_HEIGHT-UI_BORDER_OFFSET);
    
    SetTargetFPS(60); 
    
    Background();
    
    // --- Load Textures and Audios ---
    headTexture = LoadTexture("Head2.png");
    bodyTexture = LoadTexture("Body2.png");
    jumpscareTexture = LoadTexture("jumpscare (2).png");
    headAngryTexture = LoadTexture("Head_Angry.png");
    headEatTexture = LoadTexture("Head_Eat.png");
    bodyHurtTexture = LoadTexture("Body_Hurt.png");
    energyTexture = LoadTexture("Energy.png");
    hurtSound = LoadSound("Hurt2.wav");
    eatSound = LoadSound("Eat.wav");
    leftSound = LoadSound("Left2.wav");
    rightSound = LoadSound("Right2.wav");
    downSound = LoadSound("Down2.wav");
    upSound = LoadSound("Up2.wav");
    growSound = LoadSound("Grow2.wav");
   // energyLevel = LoadSound("energy_level.wav");
    levelUpSound = LoadSound("Level_Up2.wav");
    jumpScareSound = LoadSound("jump.wav");
    UI_Hover_Sfx = LoadSound("UI_Hover.wav");
    //backgroundSound = LoadMusicStream("Ahh Music.wav");

    
    loadFoods();
    
    //UI Assign
    
    
    //UI Assign

    double moveTimer = 0.0;
    double moveDelay = 0.10; // Lower number = faster speed
    
    PlayMusicStream(backgroundSound);
    
    char *prevDir = "";

    while (!WindowShouldClose() && !quitRequested) {
        if (!scared) {
            if (IsKeyPressed(KEY_S) && strcmp(direction, "up") != 0 && directDebounce == false) {
                directDebounce = true;
                direction = "down";
                if (strcmp(direction, prevDir) != 0) PlaySound(downSound);
                prevDir = direction;
            }
            if (IsKeyPressed(KEY_W) && strcmp(direction, "down") != 0 && directDebounce == false) {
                directDebounce = true;
                direction = "up";
                if (strcmp(direction, prevDir) != 0) PlaySound(upSound);
                prevDir = direction;
            }
            if (IsKeyPressed(KEY_D) && strcmp(direction, "left") != 0 && directDebounce == false) { 
                directDebounce = true;
                direction = "right";
                if (strcmp(direction, prevDir) != 0) PlaySound(rightSound);
                prevDir = direction;
            }
            if (IsKeyPressed(KEY_A) && strcmp(direction, "right") != 0 && directDebounce == false) { 
                directDebounce = true;
                direction = "left";
                if (strcmp(direction, prevDir) != 0) PlaySound(leftSound);
                prevDir = direction;
            }
            if(IsKeyPressed(KEY_ESCAPE)) {
                paused = !paused;
                
                if (mainUIHiden == true) {
                    mainUIHiden = false;
                    showUI("main");
                } else {
                    mainUIHiden = true;
                    hideUI("main");
                }
            }

        }

        if (game == true && !paused) {
            UpdateMusicStream(backgroundSound); 
            moveTimer += GetFrameTime();
            if (moveTimer >= moveDelay) {
                moveTimer -= moveDelay;
                if (scared) {
                    int randomDirection = GetRandomValue(0, 3);

                    // 0 = down
                    // 1 = up
                    // 2 = right
                    // 3 = left

                    if (randomDirection == 0 &&
                        strcmp(direction, "up") != 0)
                    {
                        direction = "down";
                    }
                    else if (randomDirection == 1 &&
                        strcmp(direction, "down") != 0)
                    {
                        direction = "up";
                    }
                    else if (randomDirection == 2 &&
                        strcmp(direction, "left") != 0)
                    {
                        direction = "right";
                    }
                    else if (randomDirection == 3 &&
                        strcmp(direction, "right") != 0)
                    {
                        direction = "left";
                    }
                }
                
                int prevX = player1.x;
                int prevY = player1.y;
                for (int i = 0; i < player1.length; i++) {
                    bodies[i].visualX = (float)bodies[i].x;
                    bodies[i].visualY = (float)bodies[i].y;
                }

                for (int i = player1.length - 1; i > 0; i--) {
                    bodies[i].x = bodies[i - 1].x;
                    bodies[i].y = bodies[i - 1].y;
                }
                if (player1.length > 0) {
                    bodies[0].x = player1.x;
                    bodies[0].y = player1.y;
                }
                player1.visualX = prevX;
                player1.visualY = prevY;
                
                if (strcmp(direction, "down") == 0) player1.y += player1.size;
                else if (strcmp(direction, "up") == 0) player1.y -= player1.size;
                else if (strcmp(direction, "right") == 0) player1.x += player1.size;
                else if (strcmp(direction, "left") == 0) player1.x -= player1.size;
                directDebounce = false;
                isEatingFood();
                isTouchingEdge();
                checkSelfCollision();
            }
        } else {
            if (IsKeyPressed(KEY_R)) {
                resetGame();
                moveTimer = 0.0;
            }
        }

        float t = (float)(moveTimer / moveDelay);
        if (t > 1.0f) t = 1.0f;
        if (t < 0.0f) t = 0.0f;

        float headDrawX = player1.visualX +
                          (player1.x - player1.visualX) * t;

        float headDrawY = player1.visualY +
                          (player1.y - player1.visualY) * t;
                          
        float screenShakeX = 0.0f;
        float screenShakeY = 0.0f;
        
        if (scared)
        {
            headDrawX += (float)GetRandomValue(-2, 2);
            headDrawY += (float)GetRandomValue(-2, 2);
        }
        whilePoisoned();
        updateScaredMode();
        updateParticles();
        updateSplats();
        updateJumpscare();
        updateEnergyPopups();
        updateBodyRipple();
        //updateTempLabels();
        BeginDrawing();
            ClearBackground(BLANK);
            
            Camera2D shakeCamera = {
                .offset = {
                    SCREEN_WIDTH / 2.0f + screenShakeX,
                    SCREEN_HEIGHT / 2.0f + screenShakeY
                },
                .target = {
                    SCREEN_WIDTH / 2.0f,
                    SCREEN_HEIGHT / 2.0f
                },
                .rotation = 0.0f,
                .zoom = 1.0f
            };

            BeginMode2D(shakeCamera);

            DrawTextureRec(
                gridTexture.texture,
                (Rectangle){
                    0,
                    0,
                    gridTexture.texture.width,
                    -gridTexture.texture.height
                },
                (Vector2){0,0},
                WHITE
            );
            
            if (game == true) {
                renderSplats();
                renderFoods();
                renderPlayerBodies(t);
                
                float headRotation = 0.0f;
                if (strcmp(direction, "right") == 0) headRotation = 0.0f;
                else if (strcmp(direction, "down") == 0) headRotation = 90.0f;
                else if (strcmp(direction, "left") == 0) headRotation = 180.0f;
                else if (strcmp(direction, "up") == 0) headRotation = 270.0f;

               // Rectangle playerRectangle = { headDrawX, headDrawY, player1.size, player1.size };
                
              //  DrawRectangleRec(
               //     playerRectangle,
               //     (Color){player1.r, player1.g, player1.b, player1.a}
               // );
                
               // DrawRectangleLinesEx(
                //    playerRectangle, 
                //    2.0f,
                //    (Color){0, 0, 0, 255}
               // );

                Color headColor = {
                    (unsigned char)player1.r,
                    (unsigned char)player1.g,
                    (unsigned char)player1.b,
                    (unsigned char)player1.a
                };

                DrawTexturePro(
                    headTexture,
                    (Rectangle){
                        0,
                        0,
                        (float)headTexture.width,
                        (float)headTexture.height
                    },
                    (Rectangle){
                        headDrawX + player1.size / 2.0f,
                        headDrawY + player1.size / 2.0f,
                        player1.size,
                        player1.size
                    },
                    (Vector2){
                        player1.size / 2.0f,
                        player1.size / 2.0f
                    },
                    headRotation,
                    headColor
                );
                
                renderEye(headDrawX, headDrawY);
                renderParticles();
                updateUI();
                renderTempLabels();
                int mX = GetMouseX();
                int mY = GetMouseY();

                cur_btn = isHoveringOverBtn(mX, mY);

                if (strcmp(cur_btn, prev_btn) != 0) {
                    if (strcmp(cur_btn, "") != 0) {
                        PlaySound(UI_Hover_Sfx);
                    }

                    prev_btn = cur_btn;
                }
                
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    if (strcmp(cur_btn, "quit") == 0) {
                        quitRequested = true;
                    }
                    if (strcmp(cur_btn, "resume") == 0) {
                        mainUIHiden = true;
                        hideUI("main");
                        paused = false;
                    }
                }
                starve();
                
                //DrawText(TextFormat("SCORE: %d", score), 0, 0, UI_BORDER_OFFSET-2, GREEN);
               // DrawText(TextFormat("LEVEL: %d", level), UI_BORDER_OFFSET*2+50, 0, UI_BORDER_OFFSET-2, WHITE);
                DrawTextureEx(
                    energyTexture,
                    //(Rectangle){ 0, SCREEN_HEIGHT/2, energyTexture.width, energyTexture.height },
                    (Vector2){ 0, SCREEN_HEIGHT/2-400 }, 
                    0.0f,
                    0.2f,
                    WHITE
                );
                float barX = 5.0f;
                float barY = SCREEN_HEIGHT / 2.0f - 400.0f;
                float barWidth = 10.0f;
                float maxHeight = 180.0f;
                DrawRectangle(
                    (int)barX - 2,
                    (int)barY - 2,
                    (int)barWidth + 4,
                    (int)maxHeight + 4,
                    (Color){0, 0, 255, 30}
                );

                DrawRectangle(
                    (int)barX,
                    (int)barY,
                    (int)barWidth,
                    (int)maxHeight,
                    (Color){0, 0, 0, 80}
                );

                float barHeight = player1.stomach * 20.0f;

                if (barHeight > maxHeight) {
                    barHeight = maxHeight;
                }
                
                if (barHeight > 0.0f)
                {
                    DrawRectangleGradientV(
                        (int)barX,
                        (int)(barY + maxHeight - barHeight),
                        (int)barWidth,
                        (int)barHeight,
                        (Color){0, 255, 255, 255},
                        (Color){0, 0, 255, 255}
                    );
                }
            } else {
                DrawText("GAME OVER", SCREEN_WIDTH/2 - MeasureText("GAME OVER", 30)/2, SCREEN_HEIGHT/2 - 40, 30, RED);
                DrawText(TextFormat("FINAL SCORE: %d", score), SCREEN_WIDTH/2 - MeasureText(TextFormat("FINAL SCORE: %d", score), 20)/2, SCREEN_HEIGHT/2, 20, WHITE);
                DrawText("Press [R] to Replay", SCREEN_WIDTH/2 - MeasureText("Press [R] to Replay", 16)/2, SCREEN_HEIGHT/2 + 40, 16, WHITE);
            }
            EndMode2D();

            if (jumpscareActive)
            {
                // Draw jumpscare image first
                DrawTexturePro(
                    jumpscareTexture,
                    (Rectangle){
                        0,
                        0,
                        (float)jumpscareTexture.width,
                        (float)jumpscareTexture.height
                    },
                    (Rectangle){
                        0,
                        0,
                        (float)SCREEN_WIDTH,
                        (float)SCREEN_HEIGHT
                    },
                    (Vector2){0, 0},
                    0.0f,
                    WHITE
                );

                // Then draw flashing overlay ON TOP
                int flashFrame = (int)(jumpscareTimer * 30.0f);

                if (flashFrame % 2 == 0)
                {
                    DrawRectangle(
                    0,
                    0,
                    SCREEN_WIDTH,
                    SCREEN_HEIGHT,
                    (Color){255, 0, 0, 100}
                );
                }
                else
                {
                    DrawRectangle(
                        0,
                        0,
                        SCREEN_WIDTH,
                        SCREEN_HEIGHT,
                    (Color){255, 255, 255, 60}
                );
                }
            }
        EndDrawing();
        
    }

    // --- VRAM CLEANUP UNLOADERS ---
    UnloadTexture(headTexture);
    UnloadTexture(bodyTexture);
    UnloadTexture(jumpscareTexture);
    UnloadRenderTexture(gridTexture);
    UnloadSound(eatSound);
    UnloadSound(hurtSound);
    UnloadSound(growSound);
    UnloadSound(leftSound);
    UnloadSound(rightSound);
    UnloadSound(upSound);
    UnloadSound(downSound);
    UnloadSound(energyLevel);
    UnloadSound(levelUpSound);
    UnloadSound(UI_Hover_Sfx);
    UnloadMusicStream(backgroundSound);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
