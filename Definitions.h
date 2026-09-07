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
#define FRUIT_COUNT 150
#define SCREEN_WIDTH (1000+UI_BORDER_OFFSET)
#define SCREEN_HEIGHT (1000+UI_BORDER_OFFSET)
#define PLAYER_SIZE 20
#define MAX_P_LENGTH ((SCREEN_WIDTH/PLAYER_SIZE) * (SCREEN_HEIGHT/PLAYER_SIZE))
#define POISON_COLOR 0x005500FF
#define NORMAL_COLOR 0x000000FF
#define MAX_PARTICLES 1000
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

extern struct TextButton buttons[MAX_UI];
extern struct Box boxes[MAX_UI];
extern struct TextLabel labels[MAX_UI];
extern struct Food powerUps[POWER_UP_COUNT];
extern struct Food foods[FRUIT_COUNT];
extern struct Player player1;
extern struct Player bodies[MAX_P_LENGTH];
extern struct Particle particles[MAX_PARTICLES];
extern struct Splat splats[MAX_SPLATS];
extern struct TextLabel tempLabels[MAX_UI];

//extern int score;
//extern int level;
extern float scaredTimer;
extern float jumpscareTimer;
extern const float JUMPSCARE_DURATION;
extern const float SCARED_DURATION;

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
int level = 1;
int food_mult = 1;
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
Music backgroundSound;

// Predefine Functions

void spawnEnergyPopup(float energy);
void updatePlayerLength(void);

void spawnFruitParticles(float x, float y, Color color);
void spawnFruitSplat(float x, float y, Color color);

void activateScaredMode(void);
void triggerJumpscare(void);

void isTouchingEdge(void); //Ends game if player touches edge
void loadFoods(void); //Handles loading the food on a grid
void renderFoods(void); //Renders the loaded foods
bool isEatingObj(int x1, int x2, int y1, int y2, int l1, int l2, int w1, int w2); //Fires successfully when player is touching any object
void createBody(int directionMode); //Handles the creation of the players body segments
void updatePlayerLength(void); //Updates player length after stomach capacity has been reached therefore converting the energy in the stomach to +1 length
void onPlayerEatingFood(float energyConsumed); //Handles what happens after the player eats the fruit
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
void spawnFruitParticles(float x, float y, Color color);
void updateParticles(void);
void renderParticles(void);
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