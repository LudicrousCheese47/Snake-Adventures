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

//UI Structures/Blueprints

#endif