#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "raylib.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "time.h"
#include "float.h"
#include "math.h"
#define MAX_SPLAT_BLOBS 12
//#include "Definitions.h"

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
    
    float mvSpd;//
};

struct Food {
    char *name;
    int x, y;
    float size; 
    int r, g, b, a;

    float energy; //Determines how much energy the fruit gives to the players stomach
    double rarity; //How rare it is for the item to spawn
    bool active; //Helps indicate if the fruit was eaten
    char *ability; //Special quirk of the food
    int threatLevel; //0 = harmless/beneficial, higher = more dangerous. Locks the
                     //fruit out of the spawn pool until the player's level is high enough.
    char *description;
};


struct Particle
{
    bool active;

    float x;
    float y;

    float velocityX;
    float velocityY;

    float size;
    float lifetime;
    float maxLifetime;

    Color color;

    bool noGravity; // Streaks fly straight (wind), not under gravity
};

struct Splat
{
    bool active;

    float x;
    float y;

    float size;
    float lifetime;

    Color color;

    int blobCount;

    // Stored positions/sizes so the splatter does not move
    float blobX[MAX_SPLAT_BLOBS];
    float blobY[MAX_SPLAT_BLOBS];
    float blobSize[MAX_SPLAT_BLOBS];
};

//UI Structures/Blueprints

struct TextButton
{
    char *name;
    int x, y;
    int w, h;
    float scale;
    float targetScale;
    int topR, topG, topB, topA;
    int btmR, btmG, btmB, btmA;
    int layer;
    float bThickness;
    int f_size;
    int br, bg, bb, ba;
    int tr, tg, tb, ta;
    bool visible;
    bool active;
    char *text;
    char *type;
};

struct Box
{
    char *name;
    int x, y;
    int w, h;
    int r, g, b, a;
    int layer;
    bool visible;
    char *type;
};

struct TextLabel {
    char *name;
    int x, y;
    int w, h;
    float scale;
    float targetScale;
    float visualX, visualY;
    int topR, topG, topB, topA;
    int btmR, btmG, btmB, btmA;
    int layer;
    float bThickness;
    int f_size;
    int br, bg, bb, ba;
    int tr, tg, tb, ta;
    float lifetime;
    float maxLifetime;
    bool visible;
    char text[256];
    char *type;
};

// Effect/inventory frame: a small grid slot at the top of the screen that
// holds a fruit icon + countdown timer. Can be a passive buff (Mult/Magnet)
// or, later, a collectable the player clicks to trigger (clickable).
struct EffectFrame {
    bool active;
    bool clickable;
    bool hovering;

    char *ability;

    float scale;
    float targetScale;

    float timer;      // Seconds remaining
    float maxTimer;   // Starting duration

    int count;        // Stack count (collectables): number held

    int r, g, b, a;        // Fruit icon color
    int accentR, accentG, accentB, accentA; // Frame border color
};

// Vertical status bar (e.g. the player energy bar). Fill ratio is taken from
// game state (player1.stomach) at render time; colors may be overridden by
// active effect states (mult/sprint) in updateBars.
struct Bar {
    char *name;
    int x, y;      // Top-left of the fill area
    int w, h;      // Fill width + max fill height
    int topR, topG, topB, topA; // Fill gradient top (resting)
    int btmR, btmG, btmB, btmA; // Fill gradient bottom (resting)
    int br, bg, bb, ba;         // Border/frame color
    int layer;
    bool visible;
    bool active;
    char *type;
};

//UI Structures/Blueprints

#endif
