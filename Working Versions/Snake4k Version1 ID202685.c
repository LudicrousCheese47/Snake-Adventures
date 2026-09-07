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
#include "time.h";

#define UI_BORDER_OFFSET 20
#define FRUIT_COUNT 100
#define SCREEN_WIDTH (1000+UI_BORDER_OFFSET)
#define SCREEN_HEIGHT (1000+UI_BORDER_OFFSET)
#define POWER_UP_COUNT 5
#define PLAYER_SIZE 20
#define MAX_P_LENGTH ((SCREEN_WIDTH/PLAYER_SIZE) * (SCREEN_HEIGHT/PLAYER_SIZE))
#define POISON_COLOR 0x005500FF
#define NORMAL_COLOR 0x000000FF

struct Player {
    int length; // Length of Player
    int prevlen; // Previous Length of Player
    float size; // Player Size
    bool growing;
    int r, g, b, a; //Player Color (If not using textures)
    
    //Actual Position
    int x, y;
    
    //Tween position for smooth movement
    float visualX, visualY;
    
    float stomach; //How full the stomach is
};

struct Food {
    char *name;
    int x, y;
    float size; 
    int r, g, b, a;

    int energy; //Determines how much energy the fruit gives to the players stomach
    double rarity; //How rare it is for the item to spawn
    bool active; //Helps indicate if the fruit was eaten
    char *ability; //Special quirk of the food
};

// Predefine Functions
void isTouchingEdge(void); //Ends game if player touches edge
void loadFoods(void); //Handles loading the food on a grid
void renderFoods(void); //Renders the loaded foods
bool isEatingObj(int x1, int x2, int y1, int y2, int l1, int l2, int w1, int w2); //Fires successfully when player is touching any object
void createBody(int directionMode); //Handles the creation of the players body segments
void updatePlayerLength(void); //Updates player length after stomach capacity has been reached therefore converting the energy in the stomach to +1 length
void onPlayerEatingFood(int energyConsumed); //Handles what happens after the player eats the fruit
void renderPlayerBodies(float lerpFactor); //Loads the player body segments
void isEatingFood(void); //Fires when isEatingObject is successeful and checks if the object is a fruit
void checkSelfCollision(void); //Ends game if player touches themselves
void resetGame(void); 
void renderBackground(void); //Renders a background grid pattern
void Background(void); //Uses renderBackground to create a unified background texture
bool isTileOccupied(int x, int y); //Checks if a object is already occupying a certain tile
char* getRandomFruit(); //Gets a random fruit based off of rarity
void bodySpawnAnimation(struct Player *body, float maxSize, float minSize); //Handles the spawn animation for when a new body segment is spawned
void starve(void); //Starves the player slowly



// Global
int fruitID = 0;
int gridColor1 = 0x0000FFFF;
int gridColor2 = 0x000088FF;
char *direction = "right";
float rotation = 0.0f;
bool game = true;
int score = 0;
float foodTick = 0;
float tickIncr = 0.005;
int minStomachCap = 10;
int level = 1;
int food_mult = 1;
bool directDebounce = false;

//Texture2D background;
Texture2D headTexture;
Texture2D bodyTexture;
Texture2D headEatTexture;
Texture2D headAngryTexture;
Texture2D bodyHurtTexture;
Texture2D appleTexture;
Texture2D grapeTexture;
Texture2D watermelonTexture;
Texture2D cheeseTexture;
Texture2D orangeTexture;
Texture2D energyTexture;
RenderTexture2D gridTexture;
Sound hurtSound;
Sound eatSound;
Sound leftSound;
Sound rightSound;
Sound downSound;
Sound upSound;
Sound growSound;
Sound energyLevel;
Sound levelUpSound;
Music backgroundSound;

struct Player player1 = {
    0, 0, PLAYER_SIZE,
    false,
    0,0,255,255, 
    220, 220,
    220.0f, 220.0f,
    0
};

struct Food foods[FRUIT_COUNT] = { 0 };

struct Food powerUps[] = {
    {"apple",       0, 0, (8+PLAYER_SIZE)/2,      255, 0, 0, 255,     3, 5.00, true, "Fruit"},
    {"grape",       0, 0, (2+PLAYER_SIZE)/2,      150, 0, 255, 255,  1, 100.00, true, "Fruit"},
    {"water melon", 0, 0, (12+PLAYER_SIZE)/2,     0, 185, 0, 255,   5, 2.00, true, "Fruit"},
    {"cheese",      0, 0, (4+PLAYER_SIZE)/2,      200, 200, 0, 255,  2, 20.00, true, "Fruit"},
    {"orange",      0, 0, (8+PLAYER_SIZE)/2,      255, 150, 0, 255,    3, 5.00, true, "Fruit"},
    {"posion apple",0, 0, (8+PLAYER_SIZE)/2,      105, 0, 0, 255,     -4, 10.00, true, "Poison"},
    {"ghost apple", 0, 0, (8+PLAYER_SIZE)/2,      100, 100, 100, 255,  1, 20.00, true, "Ghost"}
};

struct Player bodies[MAX_P_LENGTH] = { 0 };

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
    
    memset(bodies, 0, sizeof(struct Player) * MAX_P_LENGTH);
    memset(foods, 0, sizeof(struct Food) * FRUIT_COUNT);
    
    for (int i = 0; i < FRUIT_COUNT; i++) {
        foods[i].active = false;
    }
    Background();
    loadFoods();
    game = true;
}

void isTouchingEdge(void) {
    if (player1.x < 0+UI_BORDER_OFFSET || player1.x >= SCREEN_WIDTH-UI_BORDER_OFFSET || player1.y < 0+UI_BORDER_OFFSET || player1.y >= SCREEN_HEIGHT-UI_BORDER_OFFSET) {
        game = false;
        PlaySound(hurtSound);
    }
}

char* getRandomFruit() {
    int arralen = sizeof(powerUps) / sizeof(powerUps[0]);
    double weights[arralen];
    memset(weights, 0, sizeof(weights));
    char* lootNames[arralen];
    memset(lootNames, 0, sizeof(lootNames));
    double totalWeight = 0;
    for (int i = 0; i < arralen; i++) {
        weights[i] = powerUps[i].rarity;
        lootNames[i] = powerUps[i].name;
        totalWeight+= powerUps[i].rarity;
    }
    
    // Generate random number up to the total weight
    double randomRoll = rand() % (int)totalWeight;
    double currentWeightThreshold = 0;
    
    // Loop through and find which item the roll landed on
    for (int i = 0; i < arralen; i++) {
        currentWeightThreshold += weights[i];
        if (randomRoll < currentWeightThreshold) {
            return lootNames[i];
        }
    }
    memset(weights, 0, sizeof(weights)*arralen);
    memset(lootNames, 0, sizeof(lootNames)*arralen);
}

bool isTileOccupied(int x, int y)
{
    for (int i = 0; i < FRUIT_COUNT; i++)
    {
        if (foods[i].active &&
            foods[i].x == x &&
            foods[i].y == y)
        {
            return true;
        }
    }

    return false;
}

void loadFoods(void) {
    int arralen = sizeof(powerUps) / sizeof(powerUps[0]);
    bool alreadyLoaded = false;
    for (int i = 0; i < FRUIT_COUNT; i++) {
        if (foods[i].active == true && (strcmp(foods[i].ability, "Fruit") == 0)) {
            alreadyLoaded = true;
            break;
        }
    }

    if (!alreadyLoaded) {
        for (int i = 0; i < FRUIT_COUNT; i++) {
            char* selectedFruit = getRandomFruit();
            struct Food item;
            for (int j = 0; j < arralen; j++) {
                if (strcmp(selectedFruit, powerUps[j].name) == 0) {
                    item = powerUps[j];
                    break;
                }
            }
            //int type = GetRandomValue(0, POWER_UP_COUNT - 1);
            //struct Food item = powerUps[type];
            do {
                item.x = UI_BORDER_OFFSET+GetRandomValue(0, ((SCREEN_WIDTH - UI_BORDER_OFFSET*2) / PLAYER_SIZE) - 1) * PLAYER_SIZE;
                item.y = UI_BORDER_OFFSET+GetRandomValue(0, ((SCREEN_HEIGHT - UI_BORDER_OFFSET*2) / PLAYER_SIZE) - 1) * PLAYER_SIZE;
            } while (isTileOccupied(item.x, item.y));
            foods[i] = (struct Food)item;
         //   printf("%s -> %s\n", item.name, item.ability);
            fruitID++;
        }
        PlaySound(levelUpSound);
    }
}

void renderFoods(void) {
    for (int i = 0; i < FRUIT_COUNT; i++) {
        if (foods[i].active == true) {
            if (strcmp(foods[i].ability, "Fruit") == 0) {
                int offset = (PLAYER_SIZE - foods[i].size) / 2;
                DrawRectangle(foods[i].x + offset, foods[i].y + offset, foods[i].size, foods[i].size, (Color){foods[i].r, foods[i].g, foods[i].b, foods[i].a});
                Rectangle rec = (struct Rectangle){(float)foods[i].x + offset, foods[i].y + offset, (float)foods[i].size, (float)foods[i].size};
                DrawRectangleLinesEx(rec, 2.0, GetColor(NORMAL_COLOR));
            } else if (strcmp(foods[i].ability, "Poison") == 0) {
                int offset = (PLAYER_SIZE - foods[i].size) / 2;
                DrawRectangle(foods[i].x + offset, foods[i].y + offset, foods[i].size, foods[i].size, (Color){foods[i].r, foods[i].g, foods[i].b, foods[i].a});
                Rectangle rec = (struct Rectangle){(float)foods[i].x + offset, foods[i].y + offset, (float)foods[i].size, (float)foods[i].size};
                DrawRectangleLinesEx(rec, 2.0, GetColor(POISON_COLOR));
            }

        //    if (strcmp(foods[i].name, "apple") == 0) DrawTexturePro(appleTexture,(Rectangle){ 0, 0, appleTexture.width, appleTexture.height }, (Rectangle){ foods[i].x + offset, foods[i].y + offset, foods[i].size, foods[i].size}, (Vector2){ 0, 0 }, 0.0f, WHITE); 
        //    else if (strcmp(foods[i].name, "grape") == 0) DrawTexturePro(grapeTexture,(Rectangle){ 0, 0, grapeTexture.width, grapeTexture.height }, (Rectangle){ foods[i].x + offset, foods[i].y + offset, foods[i].size, foods[i].size}, (Vector2){ 0, 0 }, 0.0f, WHITE);
        //    else if (strcmp(foods[i].name, "water melon") == 0) DrawTexturePro(watermelonTexture,(Rectangle){ 0, 0, watermelonTexture.width, watermelonTexture.height }, (Rectangle){ foods[i].x + offset, foods[i].y + offset, foods[i].size, foods[i].size}, (Vector2){ 0, 0 }, 0.0f, WHITE);
        //    else if (strcmp(foods[i].name, "cheese") == 0) DrawTexturePro(cheeseTexture,(Rectangle){ 0, 0, cheeseTexture.width, cheeseTexture.height }, (Rectangle){ foods[i].x + offset, foods[i].y + offset, foods[i].size, foods[i].size}, (Vector2){ 0, 0 }, 0.0f, WHITE);
        //    else if (strcmp(foods[i].name, "orange") == 0) DrawTexturePro(orangeTexture,(Rectangle){ 0, 0, orangeTexture.width, orangeTexture.height }, (Rectangle){ foods[i].x + offset, foods[i].y + offset, foods[i].size, foods[i].size}, (Vector2){ 0, 0 }, 0.0f, WHITE);
        }
    }
    
}

bool isEatingObj(int x1, int x2, int y1, int y2, int l1, int l2, int w1, int w2) {
    if (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + l2 && y1 + l1 > y2) return true;
    return false;
}

void bodySpawnAnimation(struct Player *body, float maxSize, float minSize)
{
    const float speed = 0.5f;

    if (body->growing)
    {
        body->size += speed;

        if (body->size >= maxSize)
        {
            body->size = maxSize;
            body->growing = false;
        }
    }
    else
    {
        body->size -= speed;

        if (body->size <= minSize)
        {
            body->size = minSize;
        }
    }
}

// FIXED: Sets visualX and visualY instantly to the spawning grid node to stop fly-ins
void createBody(int directionMode) {
    int prevX = (player1.length == 0) ? player1.x : bodies[player1.length - 1].x;
    int prevY = (player1.length == 0) ? player1.y : bodies[player1.length - 1].y;

    if (directionMode == 0) prevX -= PLAYER_SIZE;
    else if (directionMode == 1) prevX += PLAYER_SIZE;
    else if (directionMode == 2) prevY += PLAYER_SIZE;
    else if (directionMode == 3) prevY -= PLAYER_SIZE;

    struct Player newSegment = {
        0, 0, 8.0f,
        true,
        0, 0, 255, 255, 
        prevX, prevY, 
        (float)prevX, (float)prevY, // Spawns directly in place visually
        0
    };
    
    bodies[player1.length] = newSegment;
}

void updatePlayerLength(void) {
    // 1. Handle shrinking (Poison)
    if (player1.length < player1.prevlen && player1.length > 0) {
        for (int i = player1.length; i < player1.prevlen; i++) {
            memset(&bodies[i], 0, sizeof(struct Player));
        }
        player1.prevlen = player1.length;
    } else if (player1.length < 0) {
        game = false;
    }
    
    // 2. Handle growing (Normal food)
    while (player1.stomach >= minStomachCap) {
        player1.stomach -= minStomachCap;
        player1.prevlen = player1.length;
        if (strcmp(direction, "right") == 0) createBody(0);
        else if (strcmp(direction, "left") == 0) createBody(1);
        else if (strcmp(direction, "up") == 0) createBody(2);
        else if (strcmp(direction, "down") == 0) createBody(3);
        PlaySound(growSound);
        player1.length++;
    }
}
void onPlayerEatingFood(int energyConsumed) {
    if (energyConsumed > 0) {
        player1.stomach += energyConsumed;
    } 
    else if (energyConsumed < 0) {
        player1.prevlen = player1.length;
        player1.length += energyConsumed; // Adds negative number (subtracts)
        
       // if (player1.length < 0) game = false; // Prevent negative length
    }
    
    // Process the changes immediately
    updatePlayerLength();
}

void renderPlayerBodies(float lerpFactor) {
    for (int i = 0; i < player1.length; i++)
    {   
        if (bodies[i].growing)
        {
            const float growSpeed = 0.8f;

            bodies[i].size += growSpeed;
    
            if (bodies[i].size >= 24.0f)
            {
                bodies[i].size = 24.0f;
                bodies[i].growing = false;
            }
        }
        else if (bodies[i].size > PLAYER_SIZE)
        {
            const float shrinkSpeed = 0.4f;

            bodies[i].size -= shrinkSpeed;

            if (bodies[i].size < PLAYER_SIZE)
                bodies[i].size = PLAYER_SIZE;
        }

        float drawX = bodies[i].visualX + (bodies[i].x - bodies[i].visualX) * lerpFactor;
        float drawY = bodies[i].visualY + (bodies[i].y - bodies[i].visualY) * lerpFactor;

        DrawTexturePro(
            bodyTexture,
            (Rectangle){0,0,bodyTexture.width,bodyTexture.height},
            (Rectangle){
                drawX + PLAYER_SIZE / 2.0f,
                drawY + PLAYER_SIZE / 2.0f,
                bodies[i].size,
                bodies[i].size
            },
            (Vector2){
                bodies[i].size / 2.0f,
                bodies[i].size / 2.0f
            },
            0.0f,
            WHITE
        );
    }
}

void isEatingFood(void) {
    int activeFruitsCount = 0;
    for (int i = 0; i < FRUIT_COUNT; i++) {
        if (foods[i].active == true) {
            if (strcmp(foods[i].ability, "Fruit") == 0) {
                activeFruitsCount++;
                //printf("%d", activeFruitsCount);
            }
            bool touchingFood = isEatingObj(player1.x, foods[i].x, player1.y, foods[i].y, player1.size, foods[i].size, player1.size, foods[i].size); 
            if (touchingFood == true) {
                foods[i].active = false;
                onPlayerEatingFood(foods[i].energy);
                score = player1.length;
                // player1.prevlen = player1.length;
                if (strcmp(foods[i].ability, "Fruit") == 0) {
                    activeFruitsCount--;
                    PlaySound(eatSound);
                    PlaySound(energyLevel);
                } else {
                    PlaySound(hurtSound);
                }
                // break;
            }
        }
    }
    if (activeFruitsCount == 0) {
        loadFoods();
        level++;
    }
}

void checkSelfCollision(void) {
    for (int i = 0; i < player1.length; i++) {
        if (player1.x == bodies[i].x && player1.y == bodies[i].y) {
            game = false;
            PlaySound(hurtSound);
            break;
        }
    }
}

void starve(void) {
    if (player1.stomach <= 0) return;
    if (player1.stomach > 0) foodTick+=1;
    if (foodTick >= 5 && player1.stomach > 0) player1.stomach -= tickIncr; 
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

        renderBackground();    // Your existing function
    EndTextureMode();
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake Adventures");
    InitAudioDevice();
    gridTexture = LoadRenderTexture(SCREEN_WIDTH-UI_BORDER_OFFSET, SCREEN_HEIGHT-UI_BORDER_OFFSET);
    SetTargetFPS(60); 
    
    Background();
    
    // --- LOAD ALL GAME TEXTURES ---
    //background = LoadTexture("Ground.png"); 
    headTexture = LoadTexture("Head.png");
    bodyTexture = LoadTexture("Body.png");
    headAngryTexture = LoadTexture("Head_Angry.png");
    headEatTexture = LoadTexture("Head_Eat.png");
    bodyHurtTexture = LoadTexture("Body_Hurt.png");
    energyTexture = LoadTexture("Energy.png");
   // appleTexture = LoadTexture("Apple.png");
   // grapeTexture = LoadTexture("Grape.png");
   // watermelonTexture = LoadTexture("Watermelon.png");
   // orangeTexture = LoadTexture("Orange.png");
   // cheeseTexture = LoadTexture("Cheese.png");
    hurtSound = LoadSound("Hurt.wav");
    eatSound = LoadSound("Eat.wav");
    leftSound = LoadSound("Left.wav");
    rightSound = LoadSound("Right.wav");
    downSound = LoadSound("Down.wav");
    upSound = LoadSound("Up.wav");
    growSound = LoadSound("Grow.wav");
    energyLevel = LoadSound("energy_level.wav");
    levelUpSound = LoadSound("Level_Up.wav");
    backgroundSound = LoadMusicStream("Ahh Music.wav");

    
    loadFoods();
    

    // --- NON-BLOCKING TICK SPEED TIMERS ---
    double moveTimer = 0.0;
    double moveDelay = 0.10; // Lower number = faster speed
    
    PlayMusicStream(backgroundSound);
    
    char *prevDir = "";

    while (!WindowShouldClose()) {
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


        if (game == true) {
            UpdateMusicStream(backgroundSound); 
            moveTimer += GetFrameTime();


            // FIXED: Process grid logic instantly when timer clicks past moveDelay thresholds
            if (moveTimer >= moveDelay) {
                moveTimer -= moveDelay; // Keep remainder time to eliminate precision-frame stuttering
                
                // Lock visual positions to current coordinates before calculations run
                int prevX = player1.x;
                int prevY = player1.y;
                for (int i = 0; i < player1.length; i++) {
                    bodies[i].visualX = (float)bodies[i].x;
                    bodies[i].visualY = (float)bodies[i].y;
                }

                // Move tail structural positions cascading forward
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

                // Shift grid head positions
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

        // FIXED: Slides both visual elements in exact lockstep synchronization
        float headDrawX = player1.visualX + (player1.x - player1.visualX) * t;
        float headDrawY = player1.visualY + (player1.y - player1.visualY) * t;

        BeginDrawing();
            ClearBackground(BLANK);
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
           // DrawTexturePro(
           //     background, 
           //     (Rectangle){ 0, 0, background.width, background.height }, 
            //    (Rectangle){ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT }, 
            //    (Vector2){ 0, 0 }, 0.0f, WHITE
            //);
            
            if (game == true) {
                renderFoods();
                renderPlayerBodies(t);
                
                // Rotates your texture on its center coordinate origin axis dynamically
                float headRotation = 0.0f;
                if (strcmp(direction, "right") == 0) headRotation = 0.0f;
                else if (strcmp(direction, "down") == 0) headRotation = 90.0f;
                else if (strcmp(direction, "left") == 0) headRotation = 180.0f;
                else if (strcmp(direction, "up") == 0) headRotation = 270.0f;

                DrawTexturePro(
                    headTexture,
                    (Rectangle){ 0, 0, headTexture.width, headTexture.height },
                    //(Rectangle){ player1.x + player1.size/2.0f, player1.y + player1.size/2.0f, player1.size, player1.size },
                    (Rectangle){ headDrawX+player1.size / 2.0f, headDrawY+player1.size / 2.0f, player1.size, player1.size },
                    (Vector2){ player1.size/2.0f, player1.size/2.0f }, 
                    headRotation, WHITE
                );
                starve();
                
                DrawText(TextFormat("SCORE: %d", score), 0, 0, UI_BORDER_OFFSET-2, GREEN);
                DrawText(TextFormat("LEVEL: %d", level), UI_BORDER_OFFSET*2+50, 0, UI_BORDER_OFFSET-2, WHITE);
                DrawTextureEx(
                    energyTexture,
                    //(Rectangle){ 0, SCREEN_HEIGHT/2, energyTexture.width, energyTexture.height },
                    (Vector2){ 0, SCREEN_HEIGHT/2-400 }, 
                    0.0f,
                    0.2f,
                    WHITE
                );
                if (player1.stomach*20 < 180) {
                    DrawRectangle(5, SCREEN_HEIGHT/2-400, 10, player1.stomach*20, GetColor(0x00FFFFFF));
                } else {
                    DrawRectangle(5, SCREEN_HEIGHT/2-400, 10, 180, GetColor(0x00FFFFFF));
                }
            } else {
                DrawText("GAME OVER", SCREEN_WIDTH/2 - MeasureText("GAME OVER", 30)/2, SCREEN_HEIGHT/2 - 40, 30, RED);
                DrawText(TextFormat("FINAL SCORE: %d", score), SCREEN_WIDTH/2 - MeasureText(TextFormat("FINAL SCORE: %d", score), 20)/2, SCREEN_HEIGHT/2, 20, WHITE);
                DrawText("Press [R] to Replay", SCREEN_WIDTH/2 - MeasureText("Press [R] to Replay", 16)/2, SCREEN_HEIGHT/2 + 40, 16, WHITE);
            }
            
        EndDrawing();
    }

    // --- VRAM CLEANUP UNLOADERS ---
    //UnloadTexture(background); 
    UnloadTexture(headTexture);
    UnloadTexture(bodyTexture);
    UnloadRenderTexture(gridTexture);
   // UnloadTexture(appleTexture);
    //UnloadTexture(grapeTexture);
   // UnloadTexture(watermelonTexture);
    //UnloadTexture(orangeTexture);
    //UnloadTexture(cheeseTexture);
    UnloadSound(eatSound);
    UnloadSound(hurtSound);
    UnloadSound(growSound);
    UnloadSound(leftSound);
    UnloadSound(rightSound);
    UnloadSound(upSound);
    UnloadSound(downSound);
    UnloadSound(energyLevel);
    UnloadSound(levelUpSound);
    UnloadMusicStream(backgroundSound);
    CloseAudioDevice();
    //UnloadMusic(backgroundSound);
    CloseWindow();
    return 0;
}
