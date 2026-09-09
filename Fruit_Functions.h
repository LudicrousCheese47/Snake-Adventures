#ifndef FOOD_FUNCS_H
#define FOOD_FUNCS_H

#include "raylib.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "time.h"
#include "float.h"
#include "math.h"
#include "Definitions.h"

// The player's level needed before a fruit of this threat is allowed to
// spawn at all. threat 0 fruits are always available.
int threatLevelUnlock(int threat)
{
    if (threat >= 3) return 8;   // ghost apple — severe
    if (threat == 2) return 5;   // poison — dangerous
    return 3;                    // threat 1 — risky
}

char* getRandomFruit() {
    int arralen = sizeof(powerUps) / sizeof(powerUps[0]);
    double weights[arralen];
    memset(weights, 0, sizeof(weights));
    char* lootNames[arralen];
    memset(lootNames, 0, sizeof(lootNames));
    double totalWeight = 0;
    int weightedCount = 0;
    for (int i = 0; i < arralen; i++) {
        // Threat-gated: dangerous fruits are locked out entirely until the
        // player's level reaches their unlock, so higher levels progressively
        // introduce harsher fruit into the pool.
        if (powerUps[i].threatLevel > 0 &&
            level < threatLevelUnlock(powerUps[i].threatLevel))
            continue;

        weights[i] = powerUps[i].rarity;
        lootNames[i] = powerUps[i].name;
        totalWeight += powerUps[i].rarity;
        weightedCount++;
    }

    if (weightedCount == 0)
        return "apple"; // never happens (safe fruit is always unlocked)
    
    // Generate random number up to the total weight
    double randomRoll = ((double)rand() / RAND_MAX) * totalWeight;
    double currentWeightThreshold = 0;
    
    // Loop through and find which item the roll landed on
    for (int i = 0; i < arralen; i++) {
        if (weights[i] <= 0) continue;
        currentWeightThreshold += weights[i];
        if (randomRoll < currentWeightThreshold) {
            return lootNames[i];
        }
    }
    // Clear arrays with proper loop iteration
    for (int i = 0; i < arralen; i++) {
        weights[i] = 0;
        lootNames[i] = NULL;
    }
    
    return "apple";
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
            do {
                item.x = UI_BORDER_OFFSET+GetRandomValue(0, ((SCREEN_WIDTH - UI_BORDER_OFFSET*2) / PLAYER_SIZE) - 1) * PLAYER_SIZE;
                item.y = UI_BORDER_OFFSET+GetRandomValue(0, ((SCREEN_HEIGHT - UI_BORDER_OFFSET*2) / PLAYER_SIZE) - 1) * PLAYER_SIZE;
            } while (isTileOccupied(item.x, item.y));
            foods[i] = (struct Food)item;
            timeSnapshot[i] = foods[i];
         //   printf("%s -> %s\n", item.name, item.ability);
            fruitID++;
        }
        // Remember how many plain fruits this level started with
        fruitsTotalThisLevel = 0;
        for (int i = 0; i < FRUIT_COUNT; i++)
        {
            if (foods[i].active && strcmp(foods[i].ability, "Fruit") == 0)
                fruitsTotalThisLevel++;
        }
        // Remember where the snake was when this level's board spawned, so the
        // rock of time can rewind everything back to exactly this moment.
        timeStartX = player1.x;
        timeStartY = player1.y;
        timeStartDirection = direction;
        timeFruitTotal = fruitsTotalThisLevel;
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
            } else if (strcmp(foods[i].ability, "Ghost") == 0) {
                    int offset = (PLAYER_SIZE - foods[i].size) / 2;

                    Color ghostColor = {
                        (unsigned char)foods[i].r,
                        (unsigned char)foods[i].g,
                        (unsigned char)foods[i].b,
                        150
                    };

                    DrawRectangle(
                        foods[i].x + offset,
                        foods[i].y + offset,
                        foods[i].size,
                        foods[i].size,
                        ghostColor
                    );

                    Rectangle rec = {
                        (float)foods[i].x + offset,
                        (float)foods[i].y + offset,
                        (float)foods[i].size,
                        (float)foods[i].size
                    };

                    DrawRectangleLinesEx(
                        rec,
                        2.0f,
                        (Color){255, 255, 255, 20}
                    );
                } else if (strcmp(foods[i].ability, "Mult") == 0) {
                    int offset = (PLAYER_SIZE - foods[i].size) / 2;
                    DrawRectangle(foods[i].x + offset, foods[i].y + offset, foods[i].size, foods[i].size, (Color){foods[i].r, foods[i].g, foods[i].b, foods[i].a});
                    Rectangle rec = (struct Rectangle){(float)foods[i].x + offset, foods[i].y + offset, (float)foods[i].size, (float)foods[i].size};
                    DrawRectangleLinesEx(rec, 2.0, (Color){foods[i].r, foods[i].g, foods[i].b, 255});
                } else if (strcmp(foods[i].ability, "Magnet") == 0) {
                    int offset = (PLAYER_SIZE - foods[i].size) / 2;
                    DrawRectangle(foods[i].x + offset, foods[i].y + offset, foods[i].size, foods[i].size, (Color){foods[i].r, foods[i].g, foods[i].b, foods[i].a});
                    Rectangle rec = (struct Rectangle){(float)foods[i].x + offset, foods[i].y + offset, (float)foods[i].size, (float)foods[i].size};
                    DrawRectangleLinesEx(rec, 2.0, (Color){0, 120, 255, 255});
                } else if (strcmp(foods[i].ability, "Heal") == 0) {
                    int offset = (PLAYER_SIZE - foods[i].size) / 2;

                    // Verdant healing glow
                    float hcx = foods[i].x + PLAYER_SIZE / 2.0f;
                    float hcy = foods[i].y + PLAYER_SIZE / 2.0f;

                    DrawCircleGradient(
                        (Vector2){ hcx, hcy },
                        22.0f,
                        (Color){0, 255, 150, 45},
                        (Color){0, 180, 80, 0}
                    );

                    DrawCircleGradient(
                        (Vector2){ hcx, hcy },
                        12.0f,
                        (Color){120, 255, 190, 60},
                        (Color){0, 200, 100, 0}
                    );

                    DrawRectangle(foods[i].x + offset, foods[i].y + offset, foods[i].size, foods[i].size, (Color){foods[i].r, foods[i].g, foods[i].b, foods[i].a});
                    Rectangle rec = (struct Rectangle){(float)foods[i].x + offset, foods[i].y + offset, (float)foods[i].size, (float)foods[i].size};
                    DrawRectangleLinesEx(rec, 2.0, (Color){180, 255, 220, 255});
                } else if (strcmp(foods[i].ability, "Ward") == 0) {
                    // Holy banana — golden glow, gentle bob, floating halo
                    float cx = foods[i].x + PLAYER_SIZE / 2.0f;
                    float cy = foods[i].y + PLAYER_SIZE / 2.0f +
                               sinf(GetTime() * 3.0f) * 2.0f;

                    // Soft golden aura under the fruit
                    DrawCircleGradient(
                        (Vector2){ cx, cy },
                        24.0f,
                        (Color){255, 225, 90, 45},
                        (Color){255, 200, 40, 0}
                    );

                    // Holy cross — golden cross with a light highlight
                    float barW = foods[i].size * 0.30f;
                    float barH = foods[i].size * 0.90f;
                    float armW = foods[i].size * 0.85f;
                    float armH = foods[i].size * 0.28f;

                    // Vertical beam
                    DrawRectangleRounded(
                        (Rectangle){ cx - barW / 2.0f, cy - barH / 2.0f, barW, barH },
                        0.30f,
                        8,
                        (Color){foods[i].r, foods[i].g, foods[i].b, 255}
                    );

                    // Horizontal beam, arms slightly above the middle
                    DrawRectangleRounded(
                        (Rectangle){ cx - armW / 2.0f, cy - barH / 2.0f + barH * 0.22f, armW, armH },
                        0.30f,
                        8,
                        (Color){foods[i].r, foods[i].g, foods[i].b, 255}
                    );

                    // Light highlight down the vertical beam
                    DrawRectangleRounded(
                        (Rectangle){ cx - barW * 0.16f, cy - barH * 0.38f, barW * 0.32f, barH * 0.76f },
                        0.30f,
                        4,
                        (Color){255, 245, 190, 220}
                    );

                    // Halo: a thin donut viewed from the side, wobbling like a coin
                    float haloY = cy - PLAYER_SIZE * 0.62f;
                    float wob = GetTime() * 4.5f;
                    float tilt = sinf(wob);                 // rocking toward/away
                    float vRadius = 1.0f + fabsf(tilt) * 3.0f;
                    float sway = sinf(wob * 0.5f) * 1.5f;   // top edge sways
                    float coinCX = cx + sway;

                    unsigned char edgeA =
                        (unsigned char)(150 + (int)(95.0f * fabsf(tilt)));

                    // Three offset outlines fake the hoop wall thickness
                    DrawEllipseLinesV(
                        (Vector2){ coinCX, haloY },
                        6.5f,
                        vRadius,
                        (Color){255, 240, 170, edgeA}
                    );

                    DrawEllipseLinesV(
                        (Vector2){ coinCX - 0.6f, haloY },
                        6.5f,
                        vRadius,
                        (Color){255, 245, 200, (unsigned char)(120 + (int)(80.0f * fabsf(tilt)))}
                    );

                    DrawEllipseLinesV(
                        (Vector2){ coinCX + 0.6f, haloY },
                        6.5f,
                        vRadius,
                        (Color){255, 220, 120, (unsigned char)(100 + (int)(70.0f * fabsf(tilt)))}
                    );

                    // Face glints when it catches the light
                    if (fabsf(tilt) > 0.55f)
                    {
                        DrawEllipseLinesV(
                            (Vector2){ coinCX, haloY },
                            6.5f,
                            vRadius,
                            (Color){255, 255, 235, (unsigned char)(80 + (int)(80.0f * fabsf(tilt)))}
                        );
                    }

                    // Soft glow bleeding off the spinning hoop
                    DrawCircleGradient(
                        (Vector2){ coinCX, haloY },
                        9.0f,
                        (Color){255, 245, 200, (unsigned char)(45.0f + 25.0f * fabsf(tilt))},
                        (Color){255, 230, 120, 0}
                    );
                } else if (strcmp(foods[i].ability, "Giant") == 0) {
                    // A big purple grape — same simple square look as the rest
                    int offset = (PLAYER_SIZE - foods[i].size) / 2;

                    // Pulsing grape glow
                    float gcx = foods[i].x + PLAYER_SIZE / 2.0f;
                    float gcy = foods[i].y + PLAYER_SIZE / 2.0f;
                    float gPulse = 0.8f + 0.2f * sinf(GetTime() * 6.0f);

                    DrawCircleGradient(
                        (Vector2){ gcx, gcy },
                        26.0f * gPulse,
                        (Color){120, 0, 255, 70},
                        (Color){60, 0, 200, 0}
                    );

                    DrawCircleGradient(
                        (Vector2){ gcx, gcy },
                        16.0f * gPulse,
                        (Color){190, 70, 255, 110},
                        (Color){110, 0, 255, 0}
                    );

                    DrawRectangle(
                        foods[i].x + offset,
                        foods[i].y + offset,
                        foods[i].size,
                        foods[i].size,
                        (Color){foods[i].r, foods[i].g, foods[i].b, foods[i].a}
                    );

                    Rectangle rec = (struct Rectangle){
                        (float)foods[i].x + offset,
                        (float)foods[i].y + offset,
                        (float)foods[i].size,
                        (float)foods[i].size
                    };

                    DrawRectangleLinesEx(rec, 2.0, (Color){170, 90, 255, 255});
                } else if (strcmp(foods[i].ability, "Time") == 0) {
                    // Rock of time — a normal apple-style square colored a
                    // bright neon green-yellow-cyan, radiating a pulsing glow,
                    // spinning counter-clockwise.
                    float cx = foods[i].x + PLAYER_SIZE / 2.0f;
                    float cy = foods[i].y + PLAYER_SIZE / 2.0f;
                    float pulse = 0.5f + 0.5f * sinf(GetTime() * 5.0f);

                    // Cycle the hue through yellow -> green -> cyan
                    float hue =
                        60.0f + 120.0f * (0.5f + 0.5f * sinf(GetTime() * 2.0f));
                    int R = 0, G = 0, B = 0;
                    {
                        float hh = fmodf(hue, 360.0f) / 60.0f;
                        int sect = (int)hh;
                        float f = hh - sect;
                        float q = 1.0f - f;
                        float tr = 0.0f, tg = 0.0f, tb = 0.0f;
                        switch (sect % 6)
                        {
                            case 0: tr = 1.0f; tg = f;  break;
                            case 1: tr = q;   tg = 1.0f; break;
                            case 2: tg = 1.0f; tb = f;  break;
                            case 3: tg = q;   tb = 1.0f; break;
                            case 4: tr = f;   tb = 1.0f; break;
                            default: tr = 1.0f; tb = q;  break;
                        }
                        R = (int)(tr * 255.0f);
                        G = (int)(tg * 255.0f);
                        B = (int)(tb * 255.0f);
                    }

                    // Radiative glow (pulsing radioactive aura)
                    DrawCircleGradient(
                        (Vector2){ cx, cy },
                        24.0f + 8.0f * pulse,
                        (Color){(unsigned char)R, (unsigned char)G, (unsigned char)B, 80},
                        (Color){0, 0, 0, 0}
                    );
                    DrawCircleGradient(
                        (Vector2){ cx, cy },
                        13.0f + 5.0f * pulse,
                        (Color){(unsigned char)R, (unsigned char)G, (unsigned char)B, 130},
                        (Color){0, 0, 0, 0}
                    );

                    // The apple-style square, rotating counter-clockwise
                    float sq = (float)foods[i].size;
                    float spin = fmodf(GetTime() * TIME_ROCK_SPIN, 360.0f);

                    DrawRectanglePro(
                        (Rectangle){
                            cx - sq * 0.5f,
                            cy - sq * 0.5f,
                            sq,
                            sq
                        },
                        (Vector2){ sq * 0.5f, sq * 0.5f },
                        spin,
                        (Color){(unsigned char)R, (unsigned char)G, (unsigned char)B, 255}
                    );

                    // Thin neon square outline (rotated corners) so the spin
                    // reads clearly
                    float rad = sq * 0.7071f;
                    Vector2 p[4];
                    for (int k = 0; k < 4; k++)
                    {
                        float ang = spin * DEG2RAD
                                  + (0.7853981f + k * 1.5707963f);
                        p[k].x = cx + cosf(ang) * rad;
                        p[k].y = cy + sinf(ang) * rad;
                    }
                    for (int k = 0; k < 4; k++)
                    {
                        DrawLineEx(
                            p[k],
                            p[(k + 1) % 4],
                            1.5f,
                            (Color){
                                (unsigned char)((R + 120 > 255) ? 255 : R + 120),
                                (unsigned char)((G + 120 > 255) ? 255 : G + 120),
                                (unsigned char)((B + 120 > 255) ? 255 : B + 120),
                                255
                            }
                        );
                    }
                }
            }
        }
}

bool isEatingObj(int x1, int x2, int y1, int y2, int l1, int l2, int w1, int w2) {
    if (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + l2 && y1 + l1 > y2) return true;
    return false;
}

void onPlayerEatingFood(float energyConsumed, bool isAbility)
{
    if (energyConsumed > 0)
    {
        // Create popup BEFORE changing the stomach
        spawnEnergyPopup(energyConsumed, isAbility);

        player1.stomach +=
            energyConsumed * (float)food_mult;
    }
    else if (energyConsumed < 0)
    {
        player1.prevlen = player1.length;

        player1.length += energyConsumed;
    }

    updatePlayerLength();
}

void onMultiplier(void) {
    if (!mult_active) return;

    multTimer -= GetFrameTime();

    if (multTimer <= 0.0f)
    {
        multTimer = 0.0f;
        mult_active = false;
        food_mult = 1.0f;
    }
}

void onMagnet(void)
{
    if (!magnet_active) return;

    magnetTimer -= GetFrameTime();

    if (magnetTimer <= 0.0f)
    {
        magnetTimer = 0.0f;
        magnet_active = false;
        return;
    }

    float px = player1.x + player1.size / 2.0f;
    float py = player1.y + player1.size / 2.0f;

    for (int i = 0; i < FRUIT_COUNT; i++)
    {
        if (!foods[i].active)
            continue;

        // Only magnetize plain fruit and multiplier fruit,
        // never hazards (Poison/Ghost) or other Magnet apples
        if (strcmp(foods[i].ability, "Fruit") != 0
            && strcmp(foods[i].ability, "Mult") != 0)
            continue;

        float fx = foods[i].x + foods[i].size / 2.0f;
        float fy = foods[i].y + foods[i].size / 2.0f;

        float dx = px - fx;
        float dy = py - fy;
        float dist = sqrtf(dx * dx + dy * dy);

        if (dist > MAGNET_RADIUS || dist < 0.1f)
            continue;

        float step = MAGNET_SPEED * GetFrameTime();
        if (dist < step) step = dist; // Don't overshoot the player

        foods[i].x += (int)(dx / dist * step);
        foods[i].y += (int)(dy / dist * step);

        // Keep magnetized fruit inside the playable area
        if (foods[i].x < UI_BORDER_OFFSET) foods[i].x = UI_BORDER_OFFSET;
        if (foods[i].y < UI_BORDER_OFFSET) foods[i].y = UI_BORDER_OFFSET;
        if (foods[i].x > SCREEN_WIDTH - UI_BORDER_OFFSET - foods[i].size)
            foods[i].x = SCREEN_WIDTH - UI_BORDER_OFFSET - foods[i].size;
        if (foods[i].y > SCREEN_HEIGHT - UI_BORDER_OFFSET - foods[i].size)
            foods[i].y = SCREEN_HEIGHT - UI_BORDER_OFFSET - foods[i].size;
    }
}

void onGiant(void)
{
    if (!giant_active) return;

    giantTimer -= GetFrameTime();

    if (giantTimer <= 0.0f)
    {
        giantTimer = 0.0f;
        giant_active = false;
    }
}

void updateGiantSuck(void)
{
    if (!giant_active)
        return;

    // Mouth looks wherever the snake is facing
    float facingX = 0.0f, facingY = 0.0f;
    if (strcmp(direction, "right") == 0)      { facingX = 1.0f;  facingY = 0.0f; }
    else if (strcmp(direction, "left") == 0)  { facingX = -1.0f; facingY = 0.0f; }
    else if (strcmp(direction, "up") == 0)    { facingX = 0.0f;  facingY = -1.0f; }
    else if (strcmp(direction, "down") == 0)  { facingX = 0.0f;  facingY = 1.0f; }

    float px = player1.x + player1.size / 2.0f + facingX * player1.size * 0.8f;
    float py = player1.y + player1.size / 2.0f + facingY * player1.size * 0.8f;

    for (int i = 0; i < FRUIT_COUNT; i++)
    {
        if (!foods[i].active)
            continue;

        // Inhale everything edible, never poison or the haunted apple
        if (strcmp(foods[i].ability, "Poison") == 0
            || strcmp(foods[i].ability, "Ghost") == 0)
            continue;

        float fx = foods[i].x + foods[i].size / 2.0f;
        float fy = foods[i].y + foods[i].size / 2.0f;

        float dx = px - fx;
        float dy = py - fy;
        float dist = sqrtf(dx * dx + dy * dy);

        if (dist > GIANT_SUCK_RADIUS || dist < 0.1f)
            continue;

        // Only fruit in front of the mouth gets inhaled
        float dot = dx * facingX + dy * facingY;
        if (dot < 0.0f)
            continue;

        float cosAng = dot / dist;
        if (cosAng < GIANT_CONE_COS)
            continue;

        // Fast, accelerating inhalation — fruit speeds up as it nears the mouth
        float closeness = 1.0f - dist / GIANT_SUCK_RADIUS;
        float step = GIANT_SUCK_SPEED * GetFrameTime() * (0.5f + closeness * 2.5f);
        if (dist < step) step = dist;

        foods[i].x += (int)(dx / dist * step);
        foods[i].y += (int)(dy / dist * step);

        // Dust streaks trail off the fruit being inhaled
        if (GetRandomValue(0, 100) <= 45)
        {
            spawnWindStreak(
                fx,
                fy,
                dx / dist * 130.0f,
                dy / dist * 130.0f,
                (Color){170, 40, 255, 160}
            );
        }

        // Keep sucked fruit inside the playable area
        if (foods[i].x < UI_BORDER_OFFSET) foods[i].x = UI_BORDER_OFFSET;
        if (foods[i].y < UI_BORDER_OFFSET) foods[i].y = UI_BORDER_OFFSET;
        if (foods[i].x > SCREEN_WIDTH - UI_BORDER_OFFSET - foods[i].size)
            foods[i].x = SCREEN_WIDTH - UI_BORDER_OFFSET - foods[i].size;
        if (foods[i].y > SCREEN_HEIGHT - UI_BORDER_OFFSET - foods[i].size)
            foods[i].y = SCREEN_HEIGHT - UI_BORDER_OFFSET - foods[i].size;
    }
}

void isEatingFood(void)
{
    int activeFruitsCount = 0;
    for (int i = 0; i < FRUIT_COUNT; i++)
    {
        if (foods[i].active == true)
        {
            // Only plain fruits count toward finishing the level — every
            // ability fruit (mult/magnet/heal/giant/wizard/poison/ghost) is
            // optional and doesn't gate progression.
            if (strcmp(foods[i].ability, "Fruit") == 0)
            {
                activeFruitsCount++;
            }

            // Giant mouths eat a bigger footprint, hoovering more fruit at once
            int eatW = giant_active
                ? (int)(player1.size * GIANT_MOUTH_SCALE)
                : (int)player1.size;
            int eatH = eatW;

            bool touchingFood = isEatingObj(
                player1.x,
                foods[i].x,
                player1.y,
                foods[i].y,
                eatW,
                foods[i].size,
                eatH,
                foods[i].size
            );

            if (touchingFood == true)
            {
                // Save fruit information before deactivating it
                float fruitX = foods[i].x + foods[i].size / 2.0f;
                float fruitY = foods[i].y + foods[i].size / 2.0f;

                Color fruitColor = {
                    (unsigned char)foods[i].r,
                    (unsigned char)foods[i].g,
                    (unsigned char)foods[i].b,
                    255
                };

                // Spawn particles
                spawnFruitParticles(
                    fruitX,
                    fruitY,
                    fruitColor
                );

                spawnFruitSplat(
                    fruitX,
                    fruitY,
                    fruitColor
                );

                foods[i].active = false;

                // Marks whether this fruit granted an ability (vs plain food)
                bool fruitIsAbility = strcmp(foods[i].ability, "Fruit") != 0;

                onPlayerEatingFood(foods[i].energy, fruitIsAbility);

                score = player1.length;
                
                rippleActive = true;
                rippleTimer = 0.0f;

                if (strcmp(foods[i].ability, "Fruit") == 0)
                {
                    activeFruitsCount--;

                    PlaySound(eatSound);
                    PlaySound(energyLevel);
                }
                else if (strcmp(foods[i].ability, "Mult") == 0)
                {
                   // activeFruitsCount--;

                    PlaySound(eatSound);
                    PlaySound(energyLevel);
                    PlaySound(mult_sound);
                    multTimer += MULT_DURATION;
                    if (multTimer > MAX_BUFF_STACK)
                        multTimer = MAX_BUFF_STACK;
                    mult_active = true;
                    food_mult = foods[i].energy;
                    multR = foods[i].r;
                    multG = foods[i].g;
                    multB = foods[i].b;
                    multA = foods[i].a;

                    addEffectFrame(
                        "Mult",
                        (Color){(unsigned char)multR, (unsigned char)multG, (unsigned char)multB, 255},
                        (Color){(unsigned char)multR, (unsigned char)multG, (unsigned char)multB, 255},
                        MULT_DURATION,
                        false
                    );

                    // Golden sparkle burst when eaten
                    for (int s = 0; s < 60; s++)
                    {
                        spawnGreedSparkle(
                            foods[i].x + foods[i].size / 2.0f,
                            foods[i].y + foods[i].size / 2.0f,
                            (Color){(unsigned char)foods[i].r, (unsigned char)foods[i].g, (unsigned char)foods[i].b, 255}
                        );
                    }
                }
                else if (strcmp(foods[i].ability, "Poison") == 0)
                {
                    PlaySound(hurtSound);

                    // Poison only ever applies ONE dose: a second apple while
                    // already poisoned won't stack another chunk on top, so
                    // the damage can't add up over time.
                    if (!poisoned)
                    {
                        poisonTick = 0;
                        poisoned = true;

                        // "A poison apple will cause 4 snake segments to die
                        // slowly" — a FIXED dose, never a fraction of the
                        // snake, so it can't compound into a stack. Never
                        // wipes the snake down to zero.
                        int lostSegments = 4;
                        if (lostSegments > player1.length - 1)
                            lostSegments = player1.length - 1;
                        if (lostSegments < 1)
                            lostSegments = 1;
                        poisonIncr = lostSegments;
                    }
                }
                else if (strcmp(foods[i].ability, "Ghost") == 0)
                {
                    if (wardSave())
                    {
                        // Shielded — the haunt bounces right off
                        PlaySound(levelUpSound);

                        float gx = foods[i].x + foods[i].size / 2.0f;
                        float gy = foods[i].y + foods[i].size / 2.0f;

                        for (int s = 0; s < 60; s++)
                        {
                            spawnGreedSparkle(
                                gx,
                                gy,
                                (Color){255, 225, 110, 255}
                            );
                        }
                    }
                    else
                    {
                        PlaySound(hurtSound);
                        activateScaredMode();
                        triggerJumpscare();
                    }
                }
                else if (strcmp(foods[i].ability, "Magnet") == 0)
                {
                    PlaySound(eatSound);
                    PlaySound(energyLevel);
                    PlaySound(mult_sound);
                    magnetTimer += MAGNET_DURATION;
                    if (magnetTimer > MAX_BUFF_STACK)
                        magnetTimer = MAX_BUFF_STACK;
                    magnet_active = true;

                    addEffectFrame(
                        "Magnet",
                        (Color){255, 0, 0, 255},      // red apple
                        (Color){0, 120, 255, 255},    // blue magnet
                        MAGNET_DURATION,
                        false
                    );

                    // Blue sparkle burst when eaten (red apple + blue magnet)
                    for (int s = 0; s < 60; s++)
                    {
                        spawnGreedSparkle(
                            foods[i].x + foods[i].size / 2.0f,
                            foods[i].y + foods[i].size / 2.0f,
                            (Color){0, 120, 255, 255}
                        );
                    }
                }
                else if (strcmp(foods[i].ability, "Heal") == 0)
                {
                    PlaySound(eatSound);
                    PlaySound(levelUpSound);

                    // Grants a clickable collectable in the effect tray
                    addEffectFrame(
                        "Heal",
                        (Color){0, 255, 150, 255},    // neon apple
                        (Color){255, 230, 140, 255},  // holy gold
                        0.0f,
                        true
                    );

                    // Verdant sparkle burst
                    for (int s = 0; s < 60; s++)
                    {
                        spawnGreedSparkle(
                            foods[i].x + foods[i].size / 2.0f,
                            foods[i].y + foods[i].size / 2.0f,
                            (Color){0, 255, 150, 255}
                        );
                    }
                }
                else if (strcmp(foods[i].ability, "Ward") == 0)
                {
                    PlaySound(eatSound);
                    PlaySound(levelUpSound);

                    // Grants a clickable "soulguard" in the effect tray
                    addEffectFrame(
                        "Ward",
                        (Color){255, 220, 60, 255},    // holy yellow banana
                        (Color){255, 255, 205, 255},   // halo light
                        0.0f,
                        true
                    );

                    // Golden sparkle burst
                    for (int s = 0; s < 60; s++)
                    {
                        spawnGreedSparkle(
                            foods[i].x + foods[i].size / 2.0f,
                            foods[i].y + foods[i].size / 2.0f,
                            (Color){255, 225, 110, 255}
                        );
                    }
                }
                else if (strcmp(foods[i].ability, "Giant") == 0)
                {
                    PlaySound(eatSound);
                    PlaySound(levelUpSound);
                    PlaySound(mult_sound);

                    giantTimer += GIANT_DURATION;
                    if (giantTimer > MAX_BUFF_STACK)
                        giantTimer = MAX_BUFF_STACK;
                    giant_active = true;

                    addEffectFrame(
                        "Giant",
                        (Color){150, 0, 255, 255},    // purple grape
                        (Color){255, 80, 255, 255},   // magenta glow
                        GIANT_DURATION,
                        false
                    );

                    // Purple sparkle burst
                    for (int s = 0; s < 60; s++)
                    {
                        spawnGreedSparkle(
                            foods[i].x + foods[i].size / 2.0f,
                            foods[i].y + foods[i].size / 2.0f,
                            (Color){150, 0, 255, 255}
                        );
                    }
                }
                else if (strcmp(foods[i].ability, "Time") == 0)
                {
                    PlaySound(levelUpSound);

                    // One rock of time per level: eating one makes every other
                    // rock on the board crumble away too.
                    for (int j = 0; j < FRUIT_COUNT; j++)
                    {
                        if (j != i &&
                            foods[j].active &&
                            strcmp(foods[j].ability, "Time") == 0)
                            foods[j].active = false;
                    }

                    // Rewind: fly back to where the level began. While the
                    // snake travels it glows cyan-green; landing restores the
                    // original board (updateWarp does the restore).
                    warpStartX = (float)player1.x;
                    warpStartY = (float)player1.y;
                    warpTargetX = (float)timeStartX;
                    warpTargetY = (float)timeStartY;
                    warp_active = true;
                    warpTimer = 0.0f;
                    timeTravel_active = true;
                    timeTravelTimer = WARP_DURATION;

                    // Green sparkle burst
                    for (int s = 0; s < 60; s++)
                    {
                        spawnGreedSparkle(
                            foods[i].x + foods[i].size / 2.0f,
                            foods[i].y + foods[i].size / 2.0f,
                            (Color){0, 255, 160, 255}
                        );
                    }
                }
            }
        }
    }

    if (activeFruitsCount == 0)
    {
        loadFoods();
        level++;
    }
}

#endif
