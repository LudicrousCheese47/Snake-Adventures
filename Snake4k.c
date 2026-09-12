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
struct Banner banners[MAX_BANNERS];

const float JUMPSCARE_DURATION = 0.25f;

Texture2D jumpscareTexture;
const float SCARED_DURATION = 4.0f;

#define INPUT_QUEUE_SIZE 3
char *inputQueue[INPUT_QUEUE_SIZE];
int inputCount = 0;
char *lastPressedDir = NULL;

// Smoothed head rotation so the big head sweeps around the neck on turns
float smoothHeadRotation = 0.0f;
const float HEAD_TURN_RATE = 540.0f;

bool isReverseDirection(char *a, char *b)
{
    return (strcmp(a, "up") == 0 && strcmp(b, "down") == 0) ||
           (strcmp(a, "down") == 0 && strcmp(b, "up") == 0) ||
           (strcmp(a, "left") == 0 && strcmp(b, "right") == 0) ||
           (strcmp(a, "right") == 0 && strcmp(b, "left") == 0);
}

void queueDirection(char *dir, Sound sfx, bool playSfx)
{
    if (inputCount >= INPUT_QUEUE_SIZE)
        return;

    // Compare against the last direction that will actually be applied
    char *last = (inputCount > 0) ? inputQueue[inputCount - 1] : direction;

    // Ignore repeats and reversals
    if (strcmp(dir, last) == 0 || isReverseDirection(dir, last))
        return;

    inputQueue[inputCount++] = dir;
    if (playSfx)
        PlaySound(sfx);
}

void clearInputQueue(void)
{
    inputCount = 0;
    lastPressedDir = NULL;
}

struct Player player1 = {
    0, 0, PLAYER_SIZE,
    false,
    PLAYER_H_DEFAULT_R, PLAYER_H_DEFAULT_G, PLAYER_H_DEFAULT_B, PLAYER_H_DEFAULT_A, 
    220, 220,
    220.0f, 220.0f,
    0,
    10.0f
};

struct Food powerUps[] = {
    {"apple",0, 0, (8+PLAYER_SIZE)/2,      255, 0, 0, 255,     3, 12.00, true, "Fruit", 0,
    "A delicious apple, grants 3 energy"},
    {"purple grape",0, 0, (2+PLAYER_SIZE)/2,      150, 0, 255, 255,     1.5, 26.00, true, "Fruit", 0,
    "A juicy grape, grants 1.5 energy"},
    {"green grape",0, 0, (2+PLAYER_SIZE)/2,      0, 165, 24, 255,     2.0, 22.00, true, "Fruit", 0,
    "A sour grape, grants 2 energy, because sour grapes are better than purple grapes."},
    {"blueberry",0, 0, (1+PLAYER_SIZE)/2,      0, 35, 255, 255,     1.0, 24.00, true, "Fruit", 0,
    "A blueberry, grants 1 energy"},
    {"water melon",0, 0, (12+PLAYER_SIZE)/2,      0, 226, 0, 255,     6.0, 8.00, true, "Fruit", 0,
    "A water melon, grants 6 energy"},
    {"cheese",0, 0, (4+PLAYER_SIZE)/2,      244, 244, 0, 255,     2.5, 12.00, true, "Fruit", 0,
    "Cheesy cheese, grants 2.5 energy"},
    {"orange",0, 0, (8+PLAYER_SIZE)/2,      255, 150, 0, 255,     3.5, 9.00, true, "Fruit", 0,
    "A zesty sour orange, grants 3.5 energy"},
    {"orange of speed",0, 0, (8+PLAYER_SIZE)/2,      255, 150, 0, 255,     4.0, 3.00, true, "Speed", 0,
    "A plasma-charged orange crackling with neon blue lightning. Grants a 1.5x speed multiplier while its charge lasts"},
    {"bolt pepper",0, 0, (7+PLAYER_SIZE)/2,      255, 0, 24, 255,     3.0, 2.00, true, "Speed", 2,
    "A spicy red pepper smoking with static. Grants a 1.5x speed multiplier while its charge lasts"},
    {"comet cherry",0, 0, (4+PLAYER_SIZE)/2,      0, 220, 255, 255,     2.5, 1.50, true, "Speed", 3,
    "A cyan comet-cherry trailing stardust. Grants a 1.5x speed multiplier while its charge lasts"},
    {"banana of endurance",0, 0, (3+PLAYER_SIZE)/2,      255, 214, 0, 255,     3.5, 3.00, true, "Endurance", 1,
    "A glowing yellow banana radiating pure endurance. While its charge lasts, moving and sprinting burn HALF as much energy"},
    {"stamina melon",0, 0, (12+PLAYER_SIZE)/2,      137, 255, 87, 255,     3.0, 2.00, true, "Endurance", 2,
    "A pale stamina melon. While its charge lasts, moving and sprinting burn HALF as much energy"},
    {"marathon pear",0, 0, (8+PLAYER_SIZE)/2,      255, 183, 0, 255,     3.0, 1.50, true, "Endurance", 3,
    "A golden marathon pear. While its charge lasts, moving and sprinting burn HALF as much energy"},
    {"posion apple",0, 0, (8+PLAYER_SIZE)/2,      128, 0, 0, 255,     4, 12.00, true, "Poison", 2,
    "A poison apple will cause 4 snake segments to die slowly, do NOT eat it"},
    {"venom berry",0, 0, (3+PLAYER_SIZE)/2,      78, 0, 110, 255,     4, 4.00, true, "Poison", 2,
    "A gnarled venom berry with the same curse — 4 snake segments die slowly, do NOT eat it"},
    {"super poison apple",0, 0, (8+PLAYER_SIZE)/2,      250, 0, 0, 255,     4, 3.50, true, "SuperPoison", 4,
    "A MUTATED poison apple that bypasses the apple of healing. It eats a quarter of the snake away at a brutal rate. DO NOT EAT IT"},
    {"cursed tomato",0, 0, (10+PLAYER_SIZE)/2,      255, 23, 0, 255,     4, 3.00, true, "SuperPoison", 4,
    "A cursed rotten tomato. It bypasses the apple of healing and eats a quarter of the snake away at a brutal rate. DO NOT EAT IT"},
    {"ghost apple",0, 0, (0.5+PLAYER_SIZE)/2,      150, 150, 255, 20,     -1, 2.50, true, "Ghost", 3,
    "This ghost apple is haunted by a entity that will scare the ever living segment off of the snake, and then the snake will go crazy. Do the snake a favor by NOT eat this"},
    {"haunted plum",0, 0, (5+PLAYER_SIZE)/2,      120, 110, 255, 20,     -1, 1.50, true, "Ghost", 3,
    "A frostbitten haunted plum with the same curse — it scares the segments off the snake and sends it crazy. Do NOT eat it"},
    {"apple of greed",0, 0, (10+PLAYER_SIZE)/2,      255, 0, 0, 255,     2, 6.00, true, "Mult", 2,
    "This apple grants an energy multiplier equal to its own energy"},
    {"berry of frenzy",0, 0, (2+PLAYER_SIZE)/2,      244, 0, 73, 255,     1.5, 2.00, true, "Mult", 3,
    "A blood-red frenzy berry. Grants an energy multiplier equal to its own energy"},
    {"melon of fortune",0, 0, (12+PLAYER_SIZE)/2,      255, 45, 103, 255,     4.0, 1.50, true, "Mult", 4,
    "A striped pink fortune melon. Grants an energy multiplier equal to its own energy"},
    {"apple of magnetism",0, 0, (8+PLAYER_SIZE)/2,      255, 0, 0, 255,     3, 7.00, true, "Magnet", 0,
    "This apple magnetises nearby fruits towards the snake"},
    {"coconut of attraction",0, 0, (10+PLAYER_SIZE)/2,      146, 76, 5, 255,     2.5, 2.00, true, "Magnet", 2,
    "A hairy brown coconut that pulls nearby fruit straight into the snake's path"},
    {"fig of pull",0, 0, (6+PLAYER_SIZE)/2,      144, 0, 207, 255,     2.0, 2.00, true, "Magnet", 3,
    "A purple fig with a gravitational hum. Magnetises nearby fruits toward the snake"},
    {"apple of healing",0, 0, (8+PLAYER_SIZE)/2,      0, 255, 150, 255,     1, 5.00, true, "Heal", 0,
    "A neon holy apple. Click its icon to cure poison"},
    {"mango of mending",0, 0, (9+PLAYER_SIZE)/2,      255, 166, 0, 255,     1, 2.00, true, "Heal", 2,
    "A sun-ripened mending mango. Click its icon to cure poison"},
    {"peach of life",0, 0, (9+PLAYER_SIZE)/2,      255, 132, 161, 255,     1, 2.00, true, "Heal", 3,
    "A soft fuzzy peach with a heartbeat. Click its icon to cure poison"},
    {"holy banana",0, 0, (3+PLAYER_SIZE)/2,      255, 209, 0, 255,     1, 3.00, true, "Ward", 3,
    "A blessed banana. Saves the snake from death once. When it saves you from a crash, the snake reverses its tracks"},
    {"spirit lime",0, 0, (5+PLAYER_SIZE)/2,      88, 255, 16, 255,     1, 1.50, true, "Ward", 4,
    "A ghostly glowing lime that bars death once. On a fatal crash it saves the snake and reverses its tracks"},
    {"guardian apricot",0, 0, (8+PLAYER_SIZE)/2,      255, 161, 59, 255,     1, 1.50, true, "Ward", 4,
    "A warm guardian apricot. Saves the snake from death once, then reverses its tracks"},
    {"giant grape",0, 0, (16+PLAYER_SIZE)/2,      150, 0, 255, 255,     2, 6.00, true, "Giant", 2,
    "A colossal grape. Turns the snake giant, sucking fruits right into its maw"},
    {"mega melon",0, 0, (16+PLAYER_SIZE)/2,      0, 244, 110, 255,     3, 2.00, true, "Giant", 3,
    "A garden-sized melon. Turns the snake giant, sucking fruits right into its maw"},
    {"colossal cherry",0, 0, (14+PLAYER_SIZE)/2,      255, 0, 0, 255,     2, 1.50, true, "Giant", 4,
    "A comically giant cherry. Turns the snake giant, devouring fruit with its maw"},
    {"rock of time",0, 0, (8+PLAYER_SIZE)/2,      38, 73, 47, 255,     1.5, 3.00, true, "Time", 3,
    "A radioactive rock of time. Eat it to travel back to where you started and respawn every fruit you ate in its original spot. Radiates a green glow and spins counter-clockwise"},
    {"hourglass apple",0, 0, (8+PLAYER_SIZE)/2,      255, 185, 26, 255,     2.0, 2.00, true, "Time", 4,
    "An apple with an hourglass core of golden sand. Eat to equip a rewind that returns to the level's start"},
    {"chrono plum",0, 0, (6+PLAYER_SIZE)/2,      33, 0, 195, 255,     2.5, 1.50, true, "Time", 4,
    "A plum frozen mid-blink of time. Eat to equip a rewind back to where the level began"},
    {"lucky clover",0, 0, (5+PLAYER_SIZE)/2,      0, 255, 75, 255,     2, 4.50, true, "Lucky", 2,
    "A four-leafed clover of fortune. While lucky, every fruit eaten has a 50% chance to grant DOUBLE energy"},
    {"golden horseshoe",0, 0, (8+PLAYER_SIZE)/2,      255, 183, 0, 255,     3, 3.50, true, "Lucky", 3,
    "A gleaming golden horseshoe. While lucky, every fruit eaten has a 50% chance to grant DOUBLE energy"},
    {"seed of renewal",0, 0, (3+PLAYER_SIZE)/2,      29, 171, 29, 255,     1, 4.00, true, "Grow", 4,
    "A dormant green seed. Eat it to equip instant growth — click its icon to sprout 3 new segments on the spot"},
    {"super sprout",0, 0, (6+PLAYER_SIZE)/2,      87, 255, 20, 255,     2, 3.00, true, "Grow", 4,
    "A bursting super sprout. Eat it to equip instant growth — click its icon to grow 3 new segments instantly"},
    {"warp melon",0, 0, (10+PLAYER_SIZE)/2,      213, 0, 244, 255,     2, 3.50, true, "Warp", 2,
    "A melon warped out of reality. Eating it scrambles the snake's controls and flips its direction for a few dizzy seconds. Beware"},
    {"glitch berry",0, 0, (4+PLAYER_SIZE)/2,      1, 255, 204, 255,     1, 2.50, true, "Warp", 2,
    "A glitchy parasitic berry. Eating it flips the snake's controls upside-down for a few chaotic seconds. Beware"},
};


struct Food foods[FRUIT_COUNT] = { 0 };

struct Player bodies[MAX_P_LENGTH] = { 0 };

struct Food timeSnapshot[FRUIT_COUNT] = { 0 };

struct Particle particles[MAX_PARTICLES] = { 0 };

struct Splat splats[MAX_SPLATS] = { 0 };

struct TextLabel tempLabels[MAX_UI] = { 0 };

struct EffectFrame effectFrames[MAX_EFFECT_FRAMES] = { 0 };

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
    },
    {
        "title_play",
        (SCREEN_WIDTH - 200) / 2,
        ((SCREEN_HEIGHT - 100) / 2) - 20,
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
        true,
        true,
        "PLAY",
        "title"
    },
    {
        "title_test",
        (SCREEN_WIDTH - 200) / 2,
        ((SCREEN_HEIGHT - 100) / 2) + 130,
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
        true,
        true,
        "TEST MODE",
        "title"
    },
    {
        "title_quit",
        (SCREEN_WIDTH - 200) / 2,
        ((SCREEN_HEIGHT - 100) / 2) + 280,
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
        true,
        true,
        "QUIT",
        "title"
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
    },
    {
        "title_bg",
        0,
        0,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        0, 0, 255, 150,
        1,
        true,
        "title"
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

    {
        "fruits",
        UI_BORDER_OFFSET*2+240, 0,
        50, UI_BORDER_OFFSET-2,
        1.0f,
        1.0f,
        0.0f,
        0.0f,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0,
        0,
        10,
        0, 0, 0, 0,
        0, 255, 255, 255,
        0, 0,
        true,
        "0/0",
        "stats"
    },

    {
        "bestCombo",
        UI_BORDER_OFFSET*2+380, 0,
        60, UI_BORDER_OFFSET-2,
        1.0f,
        1.0f,
        0.0f,
        0.0f,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0,
        0,
        10,
        0, 0, 0, 0,
        255, 220, 80, 255,
        0, 0,
        true,
        "BEST x0",
        "stats"
    },
};

// UI Init

struct Bar bars[MAX_UI] = {
    {
        "energy",
        ENERGY_BAR_X, ENERGY_BAR_Y,
        ENERGY_BAR_WIDTH, ENERGY_BAR_MAX_HEIGHT,
        0, 25, 255, 255,  // fill gradient top (resting blue)
        0, 0, 255, 255,   // fill gradient bottom
        0, 0, 255, 255,   // border
        0,                // layer
        true,             // visible
        true,             // active
        "stats"           // type
    }
};

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
    eatStreak = 0;
    bestStreak = 0;
    bestFlashTimer = 0.0f;
    hitStopTimer = 0.0f;
    deathTimer = 0.0f;
    deathTriggered = false;
    timeTickPlayTimer = 0.0f;
    timeTickActive = false;
    timeReversePlayTimer = 0.0f;
    timeReverseActive = false;
    memset(seenAbility, 0, sizeof(seenAbility));
    for (int b = 0; b < MAX_BANNERS; b++)
        banners[b].visible = false;
    shakeTimer = 0.0f;
    flashTimer = 0.0f;
    scared = false;
    jumpscareActive = false;
    jumpscareTimer = 0.0f;
    scaredTimer = 0.0f;
    poisonTick = 4;
    poisoned = false;
    poisonIncr = 0;
    poisonDebounce = false;
    poisonColorAlt = false;
    superPoisonTick = 0;
    superPoisoned = false;
    superPoisonIncr = 0;
    superPoisonDebounce = false;
    superPoisonColorAlt = false;
    food_mult = 1.0f;
    mult_active = false;
    multTimer = 0.0f;
    magnet_active = false;
    magnetTimer = 0.0f;
    holy_active = false;
    holyTimer = 0.0f;
    ward_active = false;
    wardTimer = 0.0f;
    warp_active = false;
    warpTimer = 0.0f;
    timeTravel_active = false;
    timeTravelTimer = 0.0f;
    giant_active = false;
    giantTimer = 0.0f;
    speed_active = false;
    speedTimer = 0.0f;
    speed_mult = 1.0f;
    endurance_active = false;
    enduranceTimer = 0.0f;
    energy_mult = 1.0f;
    lucky_active = false;
    luckyTimer = 0.0f;
    reverse_active = false;
    reverseTimer = 0.0f;
    clearEffectFrames();
    clearInputQueue();
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

    boardColor = color1;
    boardColorDark = color2;
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
    headTexture = LoadTexture("Assets/Head2.png");
    bodyTexture = LoadTexture("Assets/Body2.png");
    jumpscareTexture = LoadTexture("Assets/jumpscare (2).png");
    headAngryTexture = LoadTexture("Assets/Head_Angry.png");
    headEatTexture = LoadTexture("Assets/Head_Eat.png");
    bodyHurtTexture = LoadTexture("Assets/Body_Hurt.png");
    energyTexture = LoadTexture("Assets/Energy.png");
    hurtSound = LoadSound("Assets/Hurt2.wav");
    eatSound = LoadSound("Assets/Eat.wav");
    leftSound = LoadSound("Assets/Left2.wav");
    rightSound = LoadSound("Assets/Right2.wav");
    downSound = LoadSound("Assets/Down2.wav");
    upSound = LoadSound("Assets/Up2.wav");
    growSound = LoadSound("Assets/Grow2.wav");
    energyLevel = LoadSound("Assets/energy_level.wav");
    levelUpSound = LoadSound("Assets/Level_Up2.wav");
    jumpScareSound = LoadSound("Assets/jump.wav");
    UI_Hover_Sfx = LoadSound("Assets/UI_Hover.wav");
    mult_sound = LoadSound("Assets/multi.wav");

    speedSound = LoadSound("Assets/speed.wav");
    healSound = LoadSound("Assets/heal.wav");
    magnetSound = LoadSound("Assets/magnet.wav");
    wardSound = LoadSound("Assets/ward.wav");
    giantSound = LoadSound("Assets/giant.wav");
    luckySound = LoadSound("Assets/lucky.wav");
    enduranceSound = LoadSound("Assets/endurance.wav");
    growAbilitySound = LoadSound("Assets/grow.wav");
    combo_Sound = LoadSound("Assets/combo.wav");
    poisonSound = LoadSound("Assets/poison.wav");
    greedSound = LoadSound("Assets/greed.wav");
    timeTickSound = LoadSound("Assets/time_tick.wav");
    timeReverseSound = LoadSound("Assets/time_reverse.wav");
    //backgroundSound = LoadMusicStream("Ahh Music.wav");

    
    loadFoods();
    
    //UI Assign
    
    
    //UI Assign

    double moveTimer = 0.0;
    double moveDelay = 1.0 / (double)player1.mvSpd; // Seconds per tile, from snakeSpeed
    
    PlayMusicStream(backgroundSound);
    
    while (!WindowShouldClose() && !quitRequested) {
        moveDelay = 1.0 / (double)player1.mvSpd; // Recompute each frame so sprint changes speed live

        // Slow-mo death: the first frame the game ends kicks off the death
        // animation once — an expanding shockwave and a settling fade.
        if (deathTimer > 0.0f)
            deathTimer -= GetFrameTime();
        if (!game && !deathTriggered && !titleScreen)
        {
            deathTriggered = true;
            deathTimer = 2.2f;
            triggerScreenShake(16.0f);
        }
        if (bestFlashTimer > 0.0f)
            bestFlashTimer -= GetFrameTime();

        if (!scared) {
            // Warp-flip: while reversed, every key maps to the OPPOSITE tile.
            char *upKey    = reverse_active ? "down"  : "up";
            char *downKey  = reverse_active ? "up"    : "down";
            char *rightKey = reverse_active ? "left"  : "right";
            char *leftKey  = reverse_active ? "right" : "left";

            if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) { lastPressedDir = downKey;  queueDirection(downKey,  downSound,  true); }
            if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))   { lastPressedDir = upKey;    queueDirection(upKey,    upSound,    true); }
            if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)){ lastPressedDir = rightKey; queueDirection(rightKey, rightSound, true); }
            if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) { lastPressedDir = leftKey;  queueDirection(leftKey,  leftSound,  true); }
            if (game == true && !paused) {
                // Hold-to-move: keep re-asserting the held direction (silent) so the
                // snake keeps turning toward it while the key stays down. Only counts
                // while it is STILL the player's most recent direction press — any
                // newer tap disarms the hold so it can't yank control back.
                char *heldDir = NULL;
                if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) heldDir = upKey;
                else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) heldDir = downKey;
                else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) heldDir = rightKey;
                else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) heldDir = leftKey;
                if (heldDir != NULL && lastPressedDir != NULL && strcmp(heldDir, lastPressedDir) == 0)
                    queueDirection(heldDir, (Sound){0}, false);
            }
            if(IsKeyPressed(KEY_ESCAPE) && !titleScreen) {
                paused = !paused;
                
                if (mainUIHiden == true) {
                    mainUIHiden = false;
                    showUI("main");
                } else {
                    mainUIHiden = true;
                    hideUI("main");
                }
            }
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
                if (game == true && !paused && player1.stomach > 0.0f) {
                    player1.mvSpd = 15.0f * speed_mult;
                    player1.stomach -= SPRINT_BURN * GetFrameTime() * energy_mult; // burst speed, frame-rate independent
                    sprint_active = true;
                } else {
                    player1.mvSpd = 10.0f * speed_mult;
                    sprint_active = false;
                }
            } else {
                player1.mvSpd = 10.0f * speed_mult;
                sprint_active = false;
            }
        }

if (game == true && !paused) {
            UpdateMusicStream(backgroundSound); 
            onMultiplier();
            onMagnet();
            onSpeed();
            onEndurance();
            onLucky();
            onGiant();
            // Reset after a warp lands so it doesn't drain its accumulated
            // time as a burst of instant movement steps.
            if (clearMoveTimer)
            {
                moveTimer = 0.0;
                clearMoveTimer = false;
            }
            // Hit-stop: while the timer is hot the world holds its breath — the move
            // register pauses too, so there's no catch-up burst after.
            if (hitStopTimer > 0.0f)
            {
                hitStopTimer -= GetFrameTime();
            }
            else
            {
                moveTimer += GetFrameTime();
            }
            if (timeReverseActive)
            {
                timeReversePlayTimer += GetFrameTime();
                if (timeReversePlayTimer >= 1.2f)
                {
                    StopSound(timeReverseSound);
                    timeReverseActive = false;
                }
                else
                {
                    float tv = timeReversePlayTimer / 1.2f;
                    SetSoundPitch(timeReverseSound, 1.8f - 1.2f * tv);
                    SetSoundVolume(timeReverseSound, 1.0f - tv * tv);
                }
            }

            if (timeTickActive)
            {
                timeTickPlayTimer += GetFrameTime();
                if (timeTickPlayTimer >= 0.9f)
                {
                    StopSound(timeTickSound);
                    timeTickActive = false;
                }
                else
                {
                    float tt = timeTickPlayTimer / 0.9f;
                    SetSoundPitch(timeTickSound, 0.6f + 1.2f * tt);
                    SetSoundVolume(timeTickSound, 1.0f - tt * tt);
                }
            }
            if (moveTimer >= moveDelay && !warp_active) {
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

                // Apply the next buffered turn (one per move tick)
                if (!scared && inputCount > 0)
                {
                    direction = inputQueue[0];
                    for (int i = 0; i < inputCount - 1; i++)
                        inputQueue[i] = inputQueue[i + 1];
                    inputCount--;
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
                isEatingFood();
                isTouchingEdge();
                checkSelfCollision();
            }
        } else {
            if (!titleScreen && IsKeyPressed(KEY_R)) {
                resetGame();
                if (testMode)
                    loadTestBoard();
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

        headDrawX += warpOffsetX();
        headDrawY += warpOffsetY();
                          
        float screenShakeX = 0.0f;
        float screenShakeY = 0.0f;

        if (shakeTimer > 0.0f)
        {
            shakeTimer -= GetFrameTime();
            float decay = shakeTimer / 0.12f;
            if (decay < 0.0f)
                decay = 0.0f;
            screenShakeX = (float)GetRandomValue(-1, 1) * shakePower * decay;
            screenShakeY = (float)GetRandomValue(-1, 1) * shakePower * decay;
        }
        
        if (scared)
        {
            headDrawX += (float)GetRandomValue(-2, 2);
            headDrawY += (float)GetRandomValue(-2, 2);
        }
        whilePoisoned();
        whileSuperPoisoned();
        updateScaredMode();
        updateReverseMode();
        updateParticles();
        updateGroundParticles();
        updateGreedAura();
        updateMagnetEffects();
        updateHealAura();
        updateHolyGlow();
        updateWardAura();
        updateWardShield();
        updateWarp();
        updateGiantSuck();
        updateGiantAura();
        updateHangryEffect();
        updateEnergyBarSparkles();
        updateSprintEffects();
        updateNearlyFullPulse();
        if (flashTimer > 0.0f)
            flashTimer -= GetFrameTime();
        updateSplats();
        updateJumpscare();
        updateEnergyPopups();
        updateBanners();
        updateEffectFrames();
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

            // Map flashes bright green / black in sync with the snake while rewinding
            Color gridTint = WHITE;
            if (timeTravel_active)
                gridTint = (sinf(GetTime() * TIME_FLASH_RATE) > 0.0f)
                    ? (Color){0, 255, 0, 255}
                    : (Color){0, 0, 0, 255};

            DrawTextureRec(
                gridTexture.texture,
                (Rectangle){
                    0,
                    0,
                    gridTexture.texture.width,
                    -gridTexture.texture.height
                },
                (Vector2){0,0},
                gridTint
            );

            // Level-up flash — a brief cold pulse across the whole board.
            if (flashTimer > 0.0f)
            {
                float flashAlpha = 200.0f *
                    (flashTimer / 0.30f);
                if (flashAlpha > 200.0f)
                    flashAlpha = 200.0f;
                DrawRectangle(
                    0,
                    0,
                    SCREEN_WIDTH,
                    SCREEN_HEIGHT,
                    (Color){
                        flashColor.r,
                        flashColor.g,
                        flashColor.b,
                        (unsigned char)flashAlpha
                    }
                );
            }
            
            if (game == true || titleScreen) {
                renderSplats();
                renderFoods();
                renderGroundParticles();
                renderHolyAura(headDrawX, headDrawY);
                renderHangryAura(headDrawX, headDrawY);
                renderWardShield(headDrawX, headDrawY);
                renderGiantAura(headDrawX, headDrawY);
                renderPlayerBodies(t, headDrawX, headDrawY);
                renderSpeedAura(headDrawX, headDrawY);
                renderLuckyAura(headDrawX, headDrawY);
                
                // Aim the head along the real neck (head toward body[0]) so the back
                // of the sprite always lands on the body, even mid-corner.
                float targetRotation = 0.0f;

                bool haveNeck = player1.length >= 2;

                if (haveNeck)
                {
                    float body0DrawX = bodies[0].visualX +
                        (bodies[0].x - bodies[0].visualX) * t +
                        warpOffsetX();

                    float body0DrawY = bodies[0].visualY +
                        (bodies[0].y - bodies[0].visualY) * t +
                        warpOffsetY();

                    float neckDX = (headDrawX - body0DrawX);
                    float neckDY = (headDrawY - body0DrawY);

                    if (neckDX * neckDX + neckDY * neckDY > 1.0f)
                        targetRotation = atan2f(neckDY, neckDX) * RAD2DEG;
                    else
                        haveNeck = false;
                }

                if (!haveNeck)
                {
                    if (strcmp(direction, "right") == 0) targetRotation = 0.0f;
                    else if (strcmp(direction, "down") == 0) targetRotation = 90.0f;
                    else if (strcmp(direction, "left") == 0) targetRotation = 180.0f;
                    else if (strcmp(direction, "up") == 0) targetRotation = 270.0f;
                }

                // Turn smoothly, always taking the shortest arc
                float rotDelta = targetRotation - smoothHeadRotation;
                while (rotDelta > 180.0f) rotDelta -= 360.0f;
                while (rotDelta < -180.0f) rotDelta += 360.0f;

                float rotStep = HEAD_TURN_RATE * GetFrameTime();

                if (fabsf(rotDelta) <= rotStep)
                    smoothHeadRotation = targetRotation;
                else
                    smoothHeadRotation += copysignf(rotStep, rotDelta);

                float headRotation = smoothHeadRotation;

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

                if (timeTravel_active)
                    headColor = timeTravelTint(headColor);
                else if (mult_active)
                    headColor = hangryTint(headColor);
                else if (giant_active)
                    headColor = giantTint(headColor);
                else if (reverse_active)
                    headColor = reverseTint(headColor);
                else if (speed_active && !poisoned && !superPoisoned)
                    headColor = speedTint(headColor);
                else if (lucky_active && !poisoned && !superPoisoned)
                    headColor = luckyTint(headColor);

                // Hangry! The greed buff puts the snake in a foul mood
                Texture2D activeHeadTexture =
                    mult_active ? headAngryTexture : headTexture;

                // Bigger mouth while giant — it devours a wider footprint
                float headVisualSize =
                    giant_active ? player1.size * GIANT_MOUTH_SCALE : player1.size;

                DrawTexturePro(
                    activeHeadTexture,
                    (Rectangle){
                        0,
                        0,
                        (float)activeHeadTexture.width,
                        (float)activeHeadTexture.height
                    },
                    (Rectangle){
                        headDrawX + player1.size / 2.0f,
                        headDrawY + player1.size / 2.0f,
                        headVisualSize,
                        headVisualSize
                    },
                    (Vector2){
                        headVisualSize / 2.0f,
                        headVisualSize / 2.0f
                    },
                    headRotation,
                    headColor
                );
                
                renderEye(headDrawX, headDrawY);
                renderParticles();
                renderEffectFrames();
                updateUI();
                renderTempLabels();
                renderBanners();
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
                    if (strcmp(cur_btn, "title_play") == 0) {
                        titleScreen = false;
                        testMode = false;
                        resetGame();
                        hideUI("title");
                        moveTimer = 0.0;
                    }
                    if (strcmp(cur_btn, "title_test") == 0) {
                        titleScreen = false;
                        testMode = true;
                        resetGame();
                        loadTestBoard();
                        hideUI("title");
                        moveTimer = 0.0;
                    }
                    if (strcmp(cur_btn, "title_quit") == 0) {
                        quitRequested = true;
                    }
                }
                if (!titleScreen)
                    starve();
                
                //DrawText(TextFormat("SCORE: %d", score), 0, 0, UI_BORDER_OFFSET-2, GREEN);
               // DrawText(TextFormat("LEVEL: %d", level), UI_BORDER_OFFSET*2+50, 0, UI_BORDER_OFFSET-2, WHITE);
            } else if (!titleScreen) {
                // Slow-mo death: the deathTimer counts down from 2.2s. The
                // world keeps drawing underneath while a shockwave ring swells
                // out from the snake and a fog settles over the board.
                float deathT = deathTimer / 2.2f;
                if (deathT < 0.0f)
                    deathT = 0.0f;

                float fx = player1.x + player1.size / 2.0f;
                float fy = player1.y + player1.size / 2.0f;

                float waveR = (1.0f - deathT) * 500.0f;
                DrawCircleLines((int)fx, (int)fy, waveR, (Color){255, 60, 60, (unsigned char)(30.0f + 200.0f * deathT)});
                DrawCircleLines((int)fx, (int)fy, waveR * 0.6f, (Color){255, 120, 120, (unsigned char)(20.0f + 160.0f * deathT)});

                DrawText("GAME OVER", SCREEN_WIDTH/2 - MeasureText("GAME OVER", 30)/2, SCREEN_HEIGHT/2 - 40, 30, RED);
                DrawText(TextFormat("FINAL SCORE: %d", score), SCREEN_WIDTH/2 - MeasureText(TextFormat("FINAL SCORE: %d", score), 20)/2, SCREEN_HEIGHT/2, 20, WHITE);
                if (deathT < 0.25f)
                    DrawText("Press [R] to Replay", SCREEN_WIDTH/2 - MeasureText("Press [R] to Replay", 16)/2, SCREEN_HEIGHT/2 + 40, 16, WHITE);
                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){5, 5, 10, (unsigned char)(120.0f * (1.0f - deathT))});
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
    UnloadSound(mult_sound);
    UnloadSound(speedSound);
    UnloadSound(healSound);
    UnloadSound(magnetSound);
    UnloadSound(wardSound);
    UnloadSound(giantSound);
    UnloadSound(luckySound);
    UnloadSound(enduranceSound);
    UnloadSound(growAbilitySound);
    UnloadSound(combo_Sound);
    UnloadSound(poisonSound);
    UnloadSound(greedSound);
    UnloadSound(timeTickSound);
    UnloadSound(timeReverseSound);
    UnloadMusicStream(backgroundSound);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
