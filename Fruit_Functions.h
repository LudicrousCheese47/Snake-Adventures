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

// The player's level needed before a fruit of this tier is allowed to spawn
// at all. tier 0 fruits are always available. Tiers ramp up so abilities AND
// hazards are slowly introduced as the player clears boards: a couple of
// starter abilities from level 1, then fresh families at 3/5/7 and the top
// of the ladder by level 10.
int threatLevelUnlock(int threat)
{
    if (threat >= 4) return 10;  // late-game specials — super poison, grow, best variants
    if (threat >= 3) return 7;   // ward/time/ghost tier
    if (threat == 2) return 5;   // mult/giant/lucky + poison/warp tier
    return 3;                    // threat 1 — endurance/risk tier
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

// True if a PLAYER_SIZE box at (x,y) overlaps the snake's head or any live
// body segment — used to keep magnetized fruit from clipping under the snake.
bool overlapsSnake(int x, int y)
{
    Rectangle foodRect = {
        (float)x,
        (float)y,
        (float)PLAYER_SIZE,
        (float)PLAYER_SIZE
    };

    Rectangle headRect = {
        (float)player1.x,
        (float)player1.y,
        (float)PLAYER_SIZE,
        (float)PLAYER_SIZE
    };
    if (CheckCollisionRecs(foodRect, headRect))
        return true;

    for (int i = 0; i < player1.length && i < MAX_P_LENGTH; i++)
    {
        Rectangle segRect = {
            (float)bodies[i].x,
            (float)bodies[i].y,
            (float)PLAYER_SIZE,
            (float)PLAYER_SIZE
        };
        if (CheckCollisionRecs(foodRect, segRect))
            return true;
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

            // First-time toast: when a never-seen ability rolls into the pool
            // (or the level-1 opening hand deals it), name it under the level
            // banner so new toys teach themselves.
            if (strcmp(item.ability, "Fruit") != 0)
            {
                for (int j = 0; j < arralen; j++)
                {
                    if (strcmp(powerUps[j].name, item.name) == 0)
                    {
                        if (!seenAbility[j])
                        {
                            seenAbility[j] = 1;
                            spawnBannerAt(
                                TextFormat("NEW: %s!", item.name),
                                (Color){item.r, item.g, item.b, 255},
                                1.5f,
                                (float)SCREEN_HEIGHT / 2.0f + 80.0f,
                                18
                            );
                        }
                        break;
                    }
                }
            }

         //   printf("%s -> %s\n", item.name, item.ability);
            fruitID++;
        }

        // Opening hand — the very first board always deals the power fantasy
        // immediately: a magnet, a speed charge and a heal sitting on the
        // grid. Three quick wins = instant dopamine before any grinding.
        if (level == 1)
        {
            const char *hand[3] = {
                "apple of magnetism",
                "orange of speed",
                "apple of healing"
            };
            for (int h = 0; h < 3; h++)
            {
                for (int j = 0; j < arralen; j++)
                {
                    if (strcmp(powerUps[j].name, hand[h]) == 0)
                    {
                        struct Food deal = powerUps[j];
                        do {
                            deal.x = UI_BORDER_OFFSET+GetRandomValue(0, ((SCREEN_WIDTH - UI_BORDER_OFFSET*2) / PLAYER_SIZE) - 1) * PLAYER_SIZE;
                            deal.y = UI_BORDER_OFFSET+GetRandomValue(0, ((SCREEN_HEIGHT - UI_BORDER_OFFSET*2) / PLAYER_SIZE) - 1) * PLAYER_SIZE;
                        } while (isTileOccupied(deal.x, deal.y));
                        foods[h] = (struct Food)deal;
                        timeSnapshot[h] = foods[h];

                        if (!seenAbility[j])
                        {
                            seenAbility[j] = 1;
                            spawnBannerAt(
                                TextFormat("NEW: %s!", deal.name),
                                (Color){deal.r, deal.g, deal.b, 255},
                                1.5f,
                                (float)SCREEN_HEIGHT / 2.0f + 80.0f,
                                18
                            );
                        }
                        break;
                    }
                }
            }
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

void loadTestBoard(void)
{
    // Test-mode bench: place one fruit of every ability so every pickup,
    // sound, hazard, and banner can be bitten through in a couple of seconds.
    // A few plain apples keep the belly topped up between combo tests.
    int arralen = sizeof(powerUps) / sizeof(powerUps[0]);

    memset(foods, 0, sizeof(struct Food) * FRUIT_COUNT);
    for (int i = 0; i < FRUIT_COUNT; i++)
        foods[i].active = false;

    const char *testAbilities[] = {
        "Speed",      "Endurance", "Giant",     "Magnet",
        "Mult",       "Ward",      "Heal",      "Lucky",
        "Grow",       "Time",      "Ghost",     "Warp",
        "Poison",     "SuperPoison"
    };

    int next = 0;
    for (unsigned int a = 0;
         a < sizeof(testAbilities) / sizeof(testAbilities[0]) &&
         next < FRUIT_COUNT;
         a++)
    {
        struct Food pick;
        bool found = false;
        for (int j = 0; j < arralen; j++)
        {
            if (strcmp(powerUps[j].ability, testAbilities[a]) == 0)
            {
                pick = powerUps[j];
                found = true;
                break;
            }
        }
        if (!found)
            continue;

        pick.active = true;
        do {
            pick.x = UI_BORDER_OFFSET + GetRandomValue(0, ((SCREEN_WIDTH - UI_BORDER_OFFSET * 2) / PLAYER_SIZE) - 1) * PLAYER_SIZE;
            pick.y = UI_BORDER_OFFSET + GetRandomValue(0, ((SCREEN_HEIGHT - UI_BORDER_OFFSET * 2) / PLAYER_SIZE) - 1) * PLAYER_SIZE;
        } while (isTileOccupied(pick.x, pick.y) || overlapsSnake(pick.x, pick.y));

        foods[next] = pick;
        timeSnapshot[next] = foods[next];
        next++;
    }

    // Fuel: a few plain apples so the belly can keep up.
    for (int a = 0; a < 8 && next < FRUIT_COUNT; a++)
    {
        struct Food item = powerUps[0]; // "apple"
        item.active = true;
        do {
            item.x = UI_BORDER_OFFSET + GetRandomValue(0, ((SCREEN_WIDTH - UI_BORDER_OFFSET * 2) / PLAYER_SIZE) - 1) * PLAYER_SIZE;
            item.y = UI_BORDER_OFFSET + GetRandomValue(0, ((SCREEN_HEIGHT - UI_BORDER_OFFSET * 2) / PLAYER_SIZE) - 1) * PLAYER_SIZE;
        } while (isTileOccupied(item.x, item.y) || overlapsSnake(item.x, item.y));

        foods[next] = item;
        timeSnapshot[next] = item;
        next++;
    }

    fruitsTotalThisLevel = 0;
    for (int i = 0; i < FRUIT_COUNT; i++)
    {
        if (foods[i].active && strcmp(foods[i].ability, "Fruit") == 0)
            fruitsTotalThisLevel++;
    }
    timeStartX = player1.x;
    timeStartY = player1.y;
    timeStartDirection = direction;
    timeFruitTotal = fruitsTotalThisLevel;
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
            } else if (strcmp(foods[i].ability, "SuperPoison") == 0) {
                // Mutated poison apple — blazing red with a bright green stem,
                // breathing toxic green aura.
                int offset = (PLAYER_SIZE - foods[i].size) / 2;
                float cx = foods[i].x + PLAYER_SIZE / 2.0f;
                float cy = foods[i].y + PLAYER_SIZE / 2.0f;
                float pulse = 0.5f + 0.5f * sinf(GetTime() * 6.0f + (float)i * 1.9f);

                // Radiating green poison aura (two breathing rings)
                DrawCircleGradient(
                    (Vector2){ cx, cy },
                    26.0f + pulse * 8.0f,
                    (Color){ 0, 255, 0, (unsigned char)(38 + 26.0f * pulse) },
                    (Color){ 0, 220, 0, 0 }
                );
                DrawCircleGradient(
                    (Vector2){ cx, cy },
                    14.0f + pulse * 3.0f,
                    (Color){ 60, 255, 60, (unsigned char)(55 + 30.0f * pulse) },
                    (Color){ 0, 200, 0, 0 }
                );

                // Blazing red body
                DrawRectangle(foods[i].x + offset, foods[i].y + offset, foods[i].size, foods[i].size, (Color){foods[i].r, foods[i].g, foods[i].b, foods[i].a});
                Rectangle rec = (struct Rectangle){(float)foods[i].x + offset, foods[i].y + offset, (float)foods[i].size, (float)foods[i].size};
                DrawRectangleLinesEx(rec, 2.0, (Color){0, 255, 0, 255});

                // Bright green stem + leaf so it reads as the super variant
                DrawRectangleRounded(
                    (Rectangle){ cx - 2.0f, cy - foods[i].size / 2.0f - 4.0f, 4.0f, 7.0f },
                    0.5f,
                    4,
                    (Color){ 40, 255, 60, 255 }
                );
                DrawCircle(
                    cx + foods[i].size * 0.22f,
                    cy - foods[i].size / 2.0f - 3.0f,
                    3.5f,
                    (Color){ 90, 255, 110, 255 }
                );
            } else if (strcmp(foods[i].ability, "Speed") == 0) {
                // Plasma-charged orange — zesty body crackling with neon blue
                // lightning radiating outward and a breathing plasma glow.
                float cx = foods[i].x + PLAYER_SIZE / 2.0f;
                float cy = foods[i].y + PLAYER_SIZE / 2.0f;
                float pulse = 0.5f + 0.5f * sinf(GetTime() * 7.0f + (float)i * 1.3f);

                // Radiating neon blue plasma glow (two breathing rings)
                DrawCircleGradient(
                    (Vector2){ cx, cy },
                    28.0f + pulse * 9.0f,
                    (Color){ 60, 180, 255, (unsigned char)(42 + 30.0f * pulse) },
                    (Color){ 20, 90, 255, 0 }
                );
                DrawCircleGradient(
                    (Vector2){ cx, cy },
                    15.0f + pulse * 4.0f,
                    (Color){ 120, 220, 255, (unsigned char)(60 + 34.0f * pulse) },
                    (Color){ 40, 120, 255, 0 }
                );

                // Radially radiating jagged lightning bolts
                for (int bolt = 0; bolt < 5; bolt++)
                {
                    float angle = (float)(bolt / 5.0f * 360.0f) * DEG2RAD
                                + (float)GetRandomValue(-14, 14) * DEG2RAD;
                    float flare = 0.55f + 0.45f * sinf(GetTime() * 13.0f + bolt * 2.4f + (float)i);
                    float length = (18.0f + pulse * 10.0f) * flare;
                    float px = cx, py = cy;
                    int spikes = 3 + GetRandomValue(0, 2);
                    for (int s = 0; s <= spikes; s++)
                    {
                        float t = (float)s / spikes;
                        float nx = cx + cosf(angle) * length * t
                                 + (s == spikes ? 0.0f : (float)GetRandomValue(-7, 7));
                        float ny = cy + sinf(angle) * length * t
                                 + (s == spikes ? 0.0f : (float)GetRandomValue(-7, 7));
                        DrawLineEx(
                            (Vector2){ px, py },
                            (Vector2){ nx, ny },
                            2.2f,
                            (Color){ 120, 220, 255, (unsigned char)(180.0f * flare) }
                        );
                        px = nx; py = ny;
                    }
                }

                // Simple square body matching the normal orange fruit, with the
                // plasma blue kept purely as its glow + crackle.
                int offset = (PLAYER_SIZE - foods[i].size) / 2;
                DrawRectangle(
                    foods[i].x + offset,
                    foods[i].y + offset,
                    foods[i].size,
                    foods[i].size,
                    (Color){ (unsigned char)foods[i].r, (unsigned char)foods[i].g, (unsigned char)foods[i].b, 255 }
                );
                Rectangle rec = (struct Rectangle){
                    (float)foods[i].x + offset,
                    (float)foods[i].y + offset,
                    (float)foods[i].size,
                    (float)foods[i].size
                };
                DrawRectangleLinesEx(rec, 2.0, (Color){60, 180, 255, 255});
            } else if (strcmp(foods[i].ability, "Endurance") == 0) {
                // Glowing yellow banana — simple square body radiating
                // plus-shaped endurance particles and a breathing golden glow.
                int offset = (PLAYER_SIZE - foods[i].size) / 2;
                float cx = foods[i].x + PLAYER_SIZE / 2.0f;
                float cy = foods[i].y + PLAYER_SIZE / 2.0f;
                float pulse = 0.5f + 0.5f * sinf(GetTime() * 6.0f + (float)i * 1.1f);

                // Breathing golden glow (two rings)
                DrawCircleGradient(
                    (Vector2){ cx, cy },
                    26.0f + pulse * 8.0f,
                    (Color){ 255, 220, 60, (unsigned char)(40 + 30.0f * pulse) },
                    (Color){ 255, 180, 0, 0 }
                );
                DrawCircleGradient(
                    (Vector2){ cx, cy },
                    14.0f + pulse * 3.0f,
                    (Color){ 255, 245, 180, (unsigned char)(55 + 34.0f * pulse) },
                    (Color){ 255, 200, 40, 0 }
                );

                // Radiating plus-shaped endurance particles
                for (int p = 0; p < 6; p++)
                {
                    float angle = (float)(p / 6.0f * 360.0f) * DEG2RAD
                                + GetTime() * 1.2f + (float)i * 0.9f;
                    float pace = 0.5f + 0.5f * sinf(GetTime() * 9.0f + p * 2.1f);
                    float radius = (11.0f + pulse * 6.0f) + pace * 6.0f;
                    float px = cx + cosf(angle) * radius;
                    float py = cy + sinf(angle) * radius;

                    DrawText(
                        "+",
                        (int)px - 4,
                        (int)py - 7,
                        14,
                        (Color){ 255, 250, 200, (unsigned char)(170 + 85.0f * pulse) }
                    );
                }

                // Simple banana-yellow square body like the normal fruit
                DrawRectangle(foods[i].x + offset, foods[i].y + offset, foods[i].size, foods[i].size, (Color){foods[i].r, foods[i].g, foods[i].b, foods[i].a});
                Rectangle rec = (struct Rectangle){(float)foods[i].x + offset, foods[i].y + offset, (float)foods[i].size, (float)foods[i].size};
                DrawRectangleLinesEx(rec, 2.0, (Color){255, 245, 180, 255});
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
                } else if (strcmp(foods[i].ability, "Lucky") == 0) {
                    // Lucky fruit — emerald clover / gold horseshoe with a
                    // breathing coin of fortune aura and orbiting coins.
                    int offset = (PLAYER_SIZE - foods[i].size) / 2;
                    float cx = foods[i].x + PLAYER_SIZE / 2.0f;
                    float cy = foods[i].y + PLAYER_SIZE / 2.0f;
                    float pulse = 0.5f + 0.5f * sinf(GetTime() * 7.0f + (float)i * 1.2f);

                    // Breathing emerald/gold glow
                    DrawCircleGradient(
                        (Vector2){ cx, cy },
                        26.0f + pulse * 8.0f,
                        (Color){ 255, 220, 60, (unsigned char)(38 + 26.0f * pulse) },
                        (Color){ 40, 210, 90, 0 }
                    );
                    DrawCircleGradient(
                        (Vector2){ cx, cy },
                        14.0f + pulse * 3.0f,
                        (Color){ 255, 245, 150, (unsigned char)(55 + 30.0f * pulse) },
                        (Color){ 60, 230, 110, 0 }
                    );

                    // Orbiting coin sparks
                    for (int coin = 0; coin < 4; coin++)
                    {
                        float angle = (float)(coin / 4.0f * 360.0f) * DEG2RAD
                                    + GetTime() * 2.2f + (float)i * 0.7f;
                        float radius = 15.0f + pulse * 5.0f;
                        float px = cx + cosf(angle) * radius;
                        float py = cy + sinf(angle) * radius;
                        DrawCircle(
                            px, py, 2.0f + pulse,
                            (Color){ 255, 240, 130, (unsigned char)(170 + 85.0f * pulse) }
                        );
                    }

                    DrawRectangle(foods[i].x + offset, foods[i].y + offset, foods[i].size, foods[i].size, (Color){foods[i].r, foods[i].g, foods[i].b, foods[i].a});
                    Rectangle rec = (struct Rectangle){(float)foods[i].x + offset, foods[i].y + offset, (float)foods[i].size, (float)foods[i].size};
                    DrawRectangleLinesEx(rec, 2.0, (Color){255, 240, 130, 255});
                } else if (strcmp(foods[i].ability, "Grow") == 0) {
                    // Seed of renewal — verdant glow with a rising sprout glyph
                    int offset = (PLAYER_SIZE - foods[i].size) / 2;
                    float cx = foods[i].x + PLAYER_SIZE / 2.0f;
                    float cy = foods[i].y + PLAYER_SIZE / 2.0f;
                    float pulse = 0.5f + 0.5f * sinf(GetTime() * 5.0f + (float)i * 1.6f);

                    DrawCircleGradient(
                        (Vector2){ cx, cy },
                        20.0f + pulse * 7.0f,
                        (Color){ 90, 220, 110, (unsigned char)(40 + 28.0f * pulse) },
                        (Color){ 30, 120, 50, 0 }
                    );

                    // Sprout: stem + two leaves climbing out of the square
                    DrawLineEx(
                        (Vector2){ cx, cy + foods[i].size / 2.0f },
                        (Vector2){ cx, cy - foods[i].size / 2.0f - 6.0f },
                        2.5f,
                        (Color){ 160, 255, 170, 255 }
                    );
                    DrawCircle(cx + 6.0f, cy - foods[i].size / 2.0f - 6.0f, 3.5f, (Color){ 140, 255, 150, 255 });
                    DrawCircle(cx - 6.0f, cy - foods[i].size / 2.0f - 6.0f, 3.5f, (Color){ 100, 235, 120, 255 });

                    DrawRectangle(foods[i].x + offset, foods[i].y + offset, foods[i].size, foods[i].size, (Color){foods[i].r, foods[i].g, foods[i].b, foods[i].a});
                    Rectangle rec = (struct Rectangle){(float)foods[i].x + offset, foods[i].y + offset, (float)foods[i].size, (float)foods[i].size};
                    DrawRectangleLinesEx(rec, 2.0, (Color){160, 255, 170, 255});
                } else if (strcmp(foods[i].ability, "Warp") == 0) {
                    // Warp fruit — glitchy square with RGB-split echoes that
                    // jitter, so the "reversal" reads as a corrupted reality.
                    int offset = (PLAYER_SIZE - foods[i].size) / 2;
                    int jit = GetRandomValue(-3, 3);

                    // Red and blue split shadows sliding sideways
                    DrawRectangle(
                        foods[i].x + offset + jit,
                        foods[i].y + offset,
                        foods[i].size,
                        foods[i].size,
                        (Color){ 255, 0, 60, 110 }
                    );
                    DrawRectangle(
                        foods[i].x + offset - jit,
                        foods[i].y + offset,
                        foods[i].size,
                        foods[i].size,
                        (Color){ 0, 160, 255, 110 }
                    );

                    // Real body
                    DrawRectangle(foods[i].x + offset, foods[i].y + offset, foods[i].size, foods[i].size, (Color){foods[i].r, foods[i].g, foods[i].b, foods[i].a});
                    Rectangle rec = (struct Rectangle){(float)foods[i].x + offset, foods[i].y + offset, (float)foods[i].size, (float)foods[i].size};
                    DrawRectangleLinesEx(rec, 2.0, (Color){180, 60, 255, 255});
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
        // The eater blip pitches with the prize — bigger fruits land a higher
        // tone, so energy value is audible before the popup even lands.
        float blip = 1.0f + energyConsumed * 0.04f;
        if (blip > 1.35f)
            blip = 1.35f;
        SetSoundPitch(energyLevel, blip);

        // Create popup BEFORE changing the stomach
        spawnEnergyPopup(energyConsumed, isAbility);

        player1.stomach +=
            energyConsumed * (float)food_mult;

        // Lucky charge: a lucky bite lands an identical second deposit, so a
        // crit fruit pays out double. The extra popup reads as a "x2!".
        if (lucky_active && (float)GetRandomValue(0, 100) < LUCKY_CHANCE * 100.0f)
        {
            player1.stomach +=
                energyConsumed * (float)food_mult;
            spawnEnergyPopup(energyConsumed, isAbility);
        }

        // Eat-streak combo: every 5th clean bite pays a bonus splash of energy
        // and shouts about it. The pitch keeps climbing as streaks stack up.
        eatStreak++;
        if (eatStreak > bestStreak)
        {
            bestStreak = eatStreak;
            bestFlashTimer = BEST_FLASH_TIME;
        }
        if (eatStreak % COMBO_EVERY == 0)
        {
            int tier = eatStreak / COMBO_EVERY;
            if (tier > 5)
                tier = 5;
            player1.stomach += COMBO_BONUS;
            spawnEnergyPopup(COMBO_BONUS, false);
            spawnTextBanner(
                TextFormat("COMBO x%d!", eatStreak),
                (Color){ 255, 220, 80, 255 },
                1.15f
            );
            SetSoundPitch(combo_Sound, 0.9f + 0.08f * (float)tier);
            PlaySound(combo_Sound);
        }
    }
    else if (energyConsumed < 0)
    {
        player1.prevlen = player1.length;

        player1.length += energyConsumed;

        // Hazards (poison/super-poison/ghost) drain segment count — never
        // below the head, no matter how short the snake is.
        if (player1.length < 1)
            player1.length = 1;
    }

    updatePlayerLength();
}

void onLucky(void)
{
    // Lucky charge ticks down; while active, bites have a coin-flip to double.
    if (!lucky_active)
        return;

    luckyTimer -= GetFrameTime();

    if (luckyTimer <= 0.0f)
    {
        luckyTimer = 0.0f;
        lucky_active = false;
        return;
    }

    // Faint golden coin motes trail off the snake so the charge reads live
    if (GetRandomValue(0, 2) == 0 && player1.length > 0)
    {
        int idx = GetRandomValue(0, player1.length - 1);
        spawnGreedSparkle(
            bodies[idx].x + bodies[idx].size / 2.0f,
            bodies[idx].y + bodies[idx].size / 2.0f,
            (Color){ 255, 220, 80, 255 }
        );
    }
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

        int moveX = (int)(dx / dist * step);
        int moveY = (int)(dy / dist * step);

        // Never let the fruit slide under the snake — if this step would land
        // on the head or a body segment, park it right where it is instead.
        if (overlapsSnake(foods[i].x + moveX, foods[i].y + moveY))
            continue;

        foods[i].x += moveX;
        foods[i].y += moveY;

        // Keep magnetized fruit inside the playable area
        if (foods[i].x < UI_BORDER_OFFSET) foods[i].x = UI_BORDER_OFFSET;
        if (foods[i].y < UI_BORDER_OFFSET) foods[i].y = UI_BORDER_OFFSET;
        if (foods[i].x > SCREEN_WIDTH - UI_BORDER_OFFSET - foods[i].size)
            foods[i].x = SCREEN_WIDTH - UI_BORDER_OFFSET - foods[i].size;
        if (foods[i].y > SCREEN_HEIGHT - UI_BORDER_OFFSET - foods[i].size)
            foods[i].y = SCREEN_HEIGHT - UI_BORDER_OFFSET - foods[i].size;
    }
}

void onSpeed(void)
{
    // Speed charge ticks down; while active the snake runs at 1.5x speed.
    if (!speed_active)
    {
        speed_mult = 1.0f;
        return;
    }

    speedTimer -= GetFrameTime();

    if (speedTimer <= 0.0f)
    {
        speedTimer = 0.0f;
        speed_active = false;
        speed_mult = 1.0f;
        return;
    }

    speed_mult = SPEED_MULT;

    // Trailing neon-blue lightning motes spark off random body segments every
    // frame, so the snake visibly crackles while it bolts around.
    if (GetRandomValue(0, 2) == 0 && player1.length > 0)
    {
        int idx = GetRandomValue(0, player1.length - 1);
        spawnGreedSparkle(
            bodies[idx].x + bodies[idx].size / 2.0f,
            bodies[idx].y + bodies[idx].size / 2.0f,
            (Color){ 60, 180, 255, 255 }
        );
    }
}

void onEndurance(void)
{
    // Endurance charge ticks down; while active, energy burns are halved.
    if (!endurance_active)
    {
        energy_mult = 1.0f;
        return;
    }

    enduranceTimer -= GetFrameTime();

    if (enduranceTimer <= 0.0f)
    {
        enduranceTimer = 0.0f;
        endurance_active = false;
        energy_mult = 1.0f;
        return;
    }

    energy_mult = ENDURANCE_MULT;

    // Faint golden motes trail off the snake so the charge reads live
    if (GetRandomValue(0, 2) == 0 && player1.length > 0)
    {
        int idx = GetRandomValue(0, player1.length - 1);
        spawnGreedSparkle(
            bodies[idx].x + bodies[idx].size / 2.0f,
            bodies[idx].y + bodies[idx].size / 2.0f,
            (Color){ 255, 230, 110, 255 }
        );
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

        // Inhale everything edible, never poison, super poison, or the haunted apple
        if (strcmp(foods[i].ability, "Poison") == 0
            || strcmp(foods[i].ability, "SuperPoison") == 0
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
                    PlaySound(greedSound);
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
                    PlaySound(poisonSound);

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

                    // Hit-stop: the world freezes a beat as the poison lands.
                    hitStopTimer = HIT_STOP_TIME;
                    triggerScreenShake(8.0f);
                }
else if (strcmp(foods[i].ability, "SuperPoison") == 0)
                {
                    PlaySound(poisonSound);

                    // Mutated poison — ONE dose only, never stackable. It
                    // bypasses the apple of healing entirely and eats away a
                    // quarter of the snake at a fast drainage rate.
                    if (!superPoisoned)
                    {
                        superPoisonTick = 0;
                        superPoisoned = true;

                        int lostSegments = (int)(player1.length * 0.25f);
                        if (lostSegments < 1) lostSegments = 1;
                        if (lostSegments > player1.length - 1)
                            lostSegments = player1.length - 1;
                        superPoisonIncr = lostSegments;
                    }

// Vile green burst so it reads as the toxic mutation
                    for (int s = 0; s < 50; s++)
                    {
                        spawnGreedSparkle(
                            foods[i].x + foods[i].size / 2.0f,
                            foods[i].y + foods[i].size / 2.0f,
                            (Color){ 0, 255, 0, 255 }
                        );
                    }

                    // Hit-stop: the super-poison lands with real weight.
                    hitStopTimer = HIT_STOP_TIME;
                    triggerScreenShake(10.0f);
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

                        // Hit-stop: the haunt hits like a jump scare beat.
                        hitStopTimer = HIT_STOP_TIME;
                        triggerScreenShake(9.0f);
                    }
                }
                else if (strcmp(foods[i].ability, "Magnet") == 0)
                {
                    PlaySound(eatSound);
                    PlaySound(energyLevel);
                    PlaySound(magnetSound);
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
                    PlaySound(healSound);

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
                    PlaySound(wardSound);

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
                    PlaySound(energyLevel);
                    PlaySound(giantSound);

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
else if (strcmp(foods[i].ability, "Speed") == 0)
                {
                    PlaySound(eatSound);
                    PlaySound(energyLevel);
                    PlaySound(speedSound);

                    speedTimer += SPEED_DURATION;
                    if (speedTimer > MAX_BUFF_STACK)
                        speedTimer = MAX_BUFF_STACK;
                    speed_active = true;
                    speed_mult = SPEED_MULT;

                    addEffectFrame(
                        "Speed",
                        (Color){255, 150, 0, 255},     // zesty orange
                        (Color){60, 180, 255, 255},    // neon plasma blue
                        SPEED_DURATION,
                        false
                    );

                    // Crackling blue + orange burst on the bite
                    for (int s = 0; s < 60; s++)
                    {
                        spawnGreedSparkle(
                            foods[i].x + foods[i].size / 2.0f,
                            foods[i].y + foods[i].size / 2.0f,
                            (Color){60, 180, 255, 255}
                        );
                    }
                    for (int s = 0; s < 25; s++)
                    {
                        spawnGreedSparkle(
                            foods[i].x + foods[i].size / 2.0f,
                            foods[i].y + foods[i].size / 2.0f,
                            (Color){255, 170, 40, 255}
                        );
                    }
                }
else if (strcmp(foods[i].ability, "Endurance") == 0)
                {
                    PlaySound(eatSound);
                    PlaySound(energyLevel);
                    PlaySound(enduranceSound);

                    enduranceTimer += ENDURANCE_DURATION;
                    if (enduranceTimer > MAX_BUFF_STACK)
                        enduranceTimer = MAX_BUFF_STACK;
                    endurance_active = true;
                    energy_mult = ENDURANCE_MULT;

                    addEffectFrame(
                        "Endurance",
                        (Color){255, 225, 70, 255},     // glowing yellow
                        (Color){255, 245, 180, 255},    // white-hot yellow
                        ENDURANCE_DURATION,
                        false
                    );

                    // Golden burst on the bite
                    for (int s = 0; s < 60; s++)
                    {
                        spawnGreedSparkle(
                            foods[i].x + foods[i].size / 2.0f,
                            foods[i].y + foods[i].size / 2.0f,
                            (Color){255, 230, 90, 255}
                        );
                    }
                }
else if (strcmp(foods[i].ability, "Time") == 0)
                {
                    // Time ticks: sweep low -> high pitch, then fade out.
                    SetSoundPitch(timeTickSound, 0.6f);
                    SetSoundVolume(timeTickSound, 1.0f);
                    PlaySound(timeTickSound);
                    timeTickPlayTimer = 0.0f;
                    timeTickActive = true;

                    // One rock of time per level: eating one makes every other
                    // rock on the board crumble away too.
                    for (int j = 0; j < FRUIT_COUNT; j++)
                    {
                        if (j != i &&
                            foods[j].active &&
                            strcmp(foods[j].ability, "Time") == 0)
                            foods[j].active = false;
                    }

                    // Equip the rock: it sits in the effect tray ready to rewind
                    // the level when the player clicks the icon (or presses its
                    // number key). The trip itself happens in triggerTimeAbility.
                    addEffectFrame(
                        "Time",
                        (Color){40, 60, 45, 255},     // greenish radioactive stone
                        (Color){0, 255, 0, 255},      // time-travel glow
                        0.0f,
                        true
                    );

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
                else if (strcmp(foods[i].ability, "Lucky") == 0)
                {
                    PlaySound(eatSound);
                    PlaySound(energyLevel);
                    PlaySound(luckySound);

                    luckyTimer += LUCKY_DURATION;
                    if (luckyTimer > MAX_BUFF_STACK)
                        luckyTimer = MAX_BUFF_STACK;
                    lucky_active = true;

                    addEffectFrame(
                        "Lucky",
                        (Color){40, 210, 90, 255},      // emerald clover
                        (Color){255, 220, 60, 255},     // gold fortune
                        LUCKY_DURATION,
                        false
                    );

                    // Golden + emerald burst on the bite
                    for (int s = 0; s < 60; s++)
                    {
                        spawnGreedSparkle(
                            foods[i].x + foods[i].size / 2.0f,
                            foods[i].y + foods[i].size / 2.0f,
                            (Color){255, 230, 90, 255}
                        );
                    }
                }
                else if (strcmp(foods[i].ability, "Grow") == 0)
                {
                    PlaySound(growAbilitySound);

                    // Equip the seed: it sits in the effect tray; clicking
                    // it pops the growth instantly (see triggerGrowAbility).
                    addEffectFrame(
                        "Grow",
                        (Color){60, 140, 60, 255},      // dormant seed
                        (Color){140, 255, 160, 255},    // spring sprout
                        0.0f,
                        true
                    );

                    // Verdant burst on the bite
                    for (int s = 0; s < 60; s++)
                    {
                        spawnGreedSparkle(
                            foods[i].x + foods[i].size / 2.0f,
                            foods[i].y + foods[i].size / 2.0f,
                            (Color){120, 255, 140, 255}
                        );
                    }
                }
                else if (strcmp(foods[i].ability, "Warp") == 0)
                {
                    if (wardSave())
                    {
                        // Shielded — the warp bounces right off
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
                        activateReverseMode();

                        // Hit-stop: the warp scrambles the brain — freeze a beat.
                        hitStopTimer = HIT_STOP_TIME;
                        triggerScreenShake(9.0f);
                    }
                }
            }
        }
    }

    if (activeFruitsCount == 0)
    {
        loadFoods();
        level++;

        // --- Level-up moment ---------------------------------------------
        // The board is cleared: slam the new level across the screen with a
        // cool flash, a confetti storm, and a shake so the progression lands.
        spawnCenterBanner(
            TextFormat("LEVEL %d", level),
            (Color){ 190, 110, 255, 255 },    // violet level surge
            1.4f
        );
        flashTimer = 0.30f;
        flashColor = (Color){ 110, 220, 255, 255 };
        triggerScreenShake(12.0f);

        float headX = player1.x + player1.size / 2.0f;
        float headY = player1.y;
        for (int s = 0; s < 50; s++)
        {
            spawnGreedSparkle(
                headX,
                headY,
                (Color){ 255, 200, 80, 255 }
            );
        }
    }
}

#endif
