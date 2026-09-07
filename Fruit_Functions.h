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
    // Clear arrays with proper loop iteration
    for (int i = 0; i < arralen; i++) {
        weights[i] = 0;
        lootNames[i] = NULL;
    }
    
    return NULL;
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
                }
            }
        }
}

bool isEatingObj(int x1, int x2, int y1, int y2, int l1, int l2, int w1, int w2) {
    if (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + l2 && y1 + l1 > y2) return true;
    return false;
}

void onPlayerEatingFood(float energyConsumed)
{
    if (energyConsumed > 0)
    {
        // Create popup BEFORE changing the stomach
        spawnEnergyPopup(energyConsumed);

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

void isEatingFood(void)
{
    int activeFruitsCount = 0;

    for (int i = 0; i < FRUIT_COUNT; i++)
    {
        if (foods[i].active == true)
        {
            if (strcmp(foods[i].ability, "Fruit") == 0)
            {
                activeFruitsCount++;
            }

            bool touchingFood = isEatingObj(
                player1.x,
                foods[i].x,
                player1.y,
                foods[i].y,
                player1.size,
                foods[i].size,
                player1.size,
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

                onPlayerEatingFood(foods[i].energy);

                score = player1.length;
                
                rippleActive = true;
                rippleTimer = 0.0f;

                if (strcmp(foods[i].ability, "Fruit") == 0)
                {
                    activeFruitsCount--;

                    PlaySound(eatSound);
                    PlaySound(energyLevel);
                }
                else if (strcmp(foods[i].ability, "Poison") == 0)
                {
                    PlaySound(hurtSound);
                    poisonTick = 0;
                    poisoned = true;
                    poisonIncr = foods[i].energy;
                }
                else if (strcmp(foods[i].ability, "Ghost") == 0)
                {
                    PlaySound(hurtSound);

                    activateScaredMode();
                    triggerJumpscare();
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