#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include "raylib.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "time.h"
#include "float.h"
#include "math.h"

#define UI_BORDER_OFFSET 20
#define FRUIT_COUNT 120
#define SCREEN_WIDTH (1000+UI_BORDER_OFFSET)
#define SCREEN_HEIGHT (1000+UI_BORDER_OFFSET)
#define PLAYER_SIZE 20
#define MAX_P_LENGTH ((SCREEN_WIDTH/PLAYER_SIZE) * (SCREEN_HEIGHT/PLAYER_SIZE))
#define POISON_COLOR 0x005500FF
#define NORMAL_COLOR 0x000000FF
#define MAX_PARTICLES 1000
#define MAX_GROUND_PARTICLES 400
struct Particle groundParticles[MAX_GROUND_PARTICLES] = { 0 };
#define MAX_SPLATS 300
#define MAX_SPLAT_BLOBS 12
#define MAX_UI 500
#define PLAYER_H_DEFAULT_R 0
#define PLAYER_H_DEFAULT_G 255
#define PLAYER_H_DEFAULT_B 0
#define PLAYER_H_DEFAULT_A 255
#define PLAYER_B_DEFAULT_R 0
#define PLAYER_B_DEFAULT_G 255
#define PLAYER_B_DEFAULT_B 255
#define PLAYER_B_DEFAULT_A 255
#define RIPPLE_SPEED 0.04f
#define BODY_SIZE PLAYER_SIZE
#define RIPPLE_SIZE 8.0f
#define POWER_UP_COUNT 50
#define PLYERS 4

#define MAX_EFFECT_FRAMES 8
#define MAX_EFFECT_STACK 2
#define EFFECT_FRAME_SIZE 18.0f
#define EFFECT_FRAME_GAP 3.0f
#define EFFECT_FRAME_START_Y 1.0f
#define EFFECT_FRAME_RIGHT_MARGIN 6.0f
#define EFFECT_FRAME_START_X (SCREEN_WIDTH - EFFECT_FRAME_RIGHT_MARGIN - (MAX_EFFECT_FRAMES * (EFFECT_FRAME_SIZE + EFFECT_FRAME_GAP) - EFFECT_FRAME_GAP))

#define ENERGY_BAR_X 5
#define ENERGY_BAR_Y (SCREEN_HEIGHT / 2 - 400)
#define ENERGY_BAR_WIDTH 10
#define ENERGY_BAR_MAX_HEIGHT 180
#define ENERGY_BAR_SCALE 20.0f

extern struct TextButton buttons[MAX_UI];
extern struct Box boxes[MAX_UI];
extern struct TextLabel labels[MAX_UI];
extern struct Bar bars[MAX_UI];
extern struct Food powerUps[POWER_UP_COUNT];
extern struct Food foods[FRUIT_COUNT];
extern struct Player player1;
extern struct Player bodies[MAX_P_LENGTH];
extern struct Particle particles[MAX_PARTICLES];
extern struct Splat splats[MAX_SPLATS];
extern struct TextLabel tempLabels[MAX_UI];
extern struct EffectFrame effectFrames[MAX_EFFECT_FRAMES];
extern struct Food timeSnapshot[FRUIT_COUNT];

//extern int score;
//extern int level;
extern int fruitsTotalThisLevel;
extern int statsLabelX;
extern bool timeTravel_active;
extern float timeTravelTimer;
extern int timeStartX;
extern int timeStartY;
extern char *timeStartDirection;
extern int timeFruitTotal;
extern bool clearMoveTimer;
extern float scaredTimer;
extern float jumpscareTimer;
extern const float JUMPSCARE_DURATION;
extern const float SCARED_DURATION;
extern float multTimer;
extern bool mult_active;
extern bool sprint_active;
extern float magnetTimer;
extern bool magnet_active;
extern float holyTimer;
extern bool holy_active;
extern float wardTimer;
extern bool ward_active;
extern bool warp_active;
extern float warpStartX;
extern float warpStartY;
extern float warpTargetX;
extern float warpTargetY;
extern float warpTimer;
extern bool giant_active;
extern float giantTimer;
extern Color boardColor;
extern Color boardColorDark;
#define MAGNET_DURATION 8.0f
// Timed ability buffs stack up to this many seconds of total active time;
// further pickups while already stacked just refresh back toward the cap.
#define MAX_BUFF_STACK 12.0f
#define HOLY_DURATION 6.0f
#define MAGNET_RADIUS 150.0f
#define MAGNET_SPEED 120.0f
#define WARP_DURATION 0.9f
#define GIANT_DURATION 8.0f
#define GIANT_MOUTH_SCALE 1.4f
#define GIANT_BODY_SCALE 1.25f
#define GIANT_SUCK_RADIUS 175.0f
#define GIANT_SUCK_SPEED 260.0f
#define GIANT_CONE_COS 0.70f
#define GIANT_AURA_RADIUS 34.0f

Color boardColor = { 0, 0, 0, 0 };
Color boardColorDark = { 0, 0, 0, 0 };
int fruitID = 0;
int gridColor1 = 0x0000FFFF;
int gridColor2 = 0x000088FF;
char *direction = "right";
float rotation = 0.0f;
bool game = true;
int score = 0;
float foodTick = 0;
float tickIncr = 0.005;
int poisonIncr = 0;
int poisonTick = 0;
int poisonColorAlt = false;
bool poisoned = false;
bool poisonDebounce = false; 
int minStomachCap = 10;

// --- Balance tuning -------------------------------------------------------
// Base hunger burn while idle (energy per second).
#define BASE_METABOLISM 0.35f
// Extra hunger burn per snake segment (bigger snake = hungrier).
#define METABOLISM_PER_SEGMENT 0.02f
// Cost of holding left-shift to sprint (energy per second).
#define SPRINT_BURN 2.0f
// Energy required to grow one segment: base + a slowly increasing tax so
// runaway growth flattens instead of snowballing.
#define GROWTH_BASE 10
#define GROWTH_SCALE 0.12f
int level = 1;
float food_mult = 1.0f;
int fruitsTotalThisLevel = 0;
#define STATS_LABEL_GAP 16
int statsLabelX = 0; // Left-trunning cursor used to auto-grid the stats labels

// --- Rock of Time: time-travel ability ------------------------------------
// While warping back to the level start the snake glows cyan-green and its
// eye spins; when it lands, the board reverts to how it was when the level
// began (see timeSnapshot).
bool timeTravel_active = false;
float timeTravelTimer = 0.0f;
int timeStartX = 0;
int timeStartY = 0;
char *timeStartDirection = "right";
int timeFruitTotal = 0;
// Set when a warp lands so the main loop resets its move timer — otherwise
// the timer keeps running during the flight and the snake sprints several
// tiles in a row right after landing.
bool clearMoveTimer = false;
#define TIME_ROCK_SPIN 110.0f   // deg/sec, spins counter-clockwise
#define TIME_FLASH_RATE 20.0f   // green/black flash speed while rewinding
#define MULT_DURATION 9.0f
float multTimer = 0.0f;
bool mult_active = false;
bool sprint_active = false;
float magnetTimer = 0.0f;
bool magnet_active = false;
float holyTimer = 0.0f;
bool holy_active = false;
float wardTimer = 0.0f;
bool ward_active = false;
bool warp_active = false;
float warpStartX = 0.0f;
float warpStartY = 0.0f;
float warpTargetX = 0.0f;
float warpTargetY = 0.0f;
float warpTimer = 0.0f;
bool giant_active = false;
float giantTimer = 0.0f;
#define HOLY_DURATION 6.0f
#define WARD_SHIELD_DURATION 6.0f
int eyeTargetFruit = -1;
bool scared = false;
float scaredTimer = 0.0f;
bool jumpscareActive = false;
float jumpscareTimer = 0.0f;
bool mainUIHiden = true;
bool paused = false;
char *cur_btn = "";
char *prev_btn = "";
bool quitRequested = false;
float rippleTimer = 0.0f;
bool rippleActive = false;
int multR = 255,
    multG = 200,
    multB = 0,
    multA = 255;

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
RenderTexture2D snakeCanvas;
Sound hurtSound;
Sound eatSound;
Sound leftSound;
Sound rightSound;
Sound downSound;
Sound upSound;
Sound growSound;
Sound energyLevel;
Sound levelUpSound;
Sound jumpScareSound;
Sound UI_Hover_Sfx;
Sound mult_sound;
Music backgroundSound;

// Predefine Functions

void spawnEnergyPopup(float energy, bool isAbility);
void updatePlayerLength(void);

int addEffectFrame(char *ability, Color fruitColor, Color accentColor, float duration, bool clickable);
void updateEffectFrames(void);
void renderEffectFrames(void);
void clearEffectFrames(void);
void updateBars(int i);
float energyBarFillHeight(void);

void spawnFruitParticles(float x, float y, Color color);
void spawnFruitSplat(float x, float y, Color color);
void spawnGreedSparkle(float x, float y, Color baseColor);
void updateGreedAura(void);
void updateEnergyBarSparkles(void);
void onMultiplier(void);
void onMagnet(void);
void updateMagnetEffects(void);
void triggerHealAbility(void);
void updateHealAura(void);
void updateHolyGlow(void);
void renderHolyAura(float headX, float headY);
void updateHangryEffect(void);
void renderHangryAura(float headX, float headY);
void updateWardShield(void);
void renderWardShield(float headX, float headY);
void updateWardAura(void);
void triggerWardAbility(void);
bool wardSave(void);
void reverseDirection(void);
void clampPlayerToBoard(void);
void updateWarp(void);
float warpOffsetX(void);
float warpOffsetY(void);
void onGiant(void);
void updateGiantSuck(void);
void renderGiantAura(float headX, float headY);
void updateGiantAura(void);
void spawnWindStreak(float x, float y, float vx, float vy, Color color);
extern char *lastPressedDir;
void clearInputQueue(void);

void activateScaredMode(void);
void triggerJumpscare(void);

void isTouchingEdge(void); //Ends game if player touches edge
void loadFoods(void); //Handles loading the food on a grid
void renderFoods(void); //Renders the loaded foods
bool isEatingObj(int x1, int x2, int y1, int y2, int l1, int l2, int w1, int w2); //Fires successfully when player is touching any object
void createBody(int directionMode); //Handles the creation of the players body segments
void updatePlayerLength(void); //Updates player length after stomach capacity has been reached therefore converting the energy in the stomach to +1 length
void onPlayerEatingFood(float energyConsumed, bool isAbility); //Handles what happens after the player eats the fruit
void renderPlayerBodies(float lerpFactor, float headDrawX, float headDrawY); //Loads the player body segments
void isEatingFood(void); //Fires when isEatingObject is successeful and checks if the object is a fruit
void checkSelfCollision(void); //Ends game if player touches themselves
void resetGame(void); 
void renderBackground(void); //Renders a background grid pattern
void Background(void); //Uses renderBackground to create a unified background texture
bool isTileOccupied(int x, int y); //Checks if a object is already occupying a certain tile
char* getRandomFruit(); //Gets a random fruit based off of rarity
void bodySpawnAnimation(struct Player *body, float maxSize, float minSize); //Handles the spawn animation for when a new body segment is spawned
void starve(void); //Starves the player slowly
void spawnFruitParticles(float x, float y, Color color);
void updateParticles(void);
void renderParticles(void);
void updateGroundParticles(void);
void renderGroundParticles(void);
void spawnFruitSplat(float x, float y, Color color);
void renderSplats(void);
void activateScaredMode(void);
void updateScaredMode(void);
void triggerJumpscare(void);
void updateJumpscare(void);
void whilePoisoned(void);
void updateBodyRipple(void);
void renderEye(float headDrawX, float headDrawY);

#endif
