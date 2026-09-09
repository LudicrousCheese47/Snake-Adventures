#ifndef PLAYER_FUNCTIONS_H
#define PLAYER_FUNCTIONS_H

#include "Definitions.h"

void reverseDirection(void)
{
    if (strcmp(direction, "right") == 0) direction = "left";
    else if (strcmp(direction, "left") == 0) direction = "right";
    else if (strcmp(direction, "up") == 0) direction = "down";
    else if (strcmp(direction, "down") == 0) direction = "up";

    // Cancel any queued turns so the reversal sticks
    clearInputQueue();

    if (lastPressedDir != NULL)
        lastPressedDir = direction;
}

void clampPlayerToBoard(void)
{
    if (player1.x < UI_BORDER_OFFSET)
        player1.x = UI_BORDER_OFFSET;
    else if (player1.x >= SCREEN_WIDTH - UI_BORDER_OFFSET)
        player1.x -= PLAYER_SIZE;

    if (player1.y < UI_BORDER_OFFSET)
        player1.y = UI_BORDER_OFFSET;
    else if (player1.y >= SCREEN_HEIGHT - UI_BORDER_OFFSET)
        player1.y -= PLAYER_SIZE;

    player1.visualX = player1.x;
    player1.visualY = player1.y;
}

void isTouchingEdge(void) {
    if (player1.x < 0+UI_BORDER_OFFSET || player1.x >= SCREEN_WIDTH-UI_BORDER_OFFSET || player1.y < 0+UI_BORDER_OFFSET || player1.y >= SCREEN_HEIGHT-UI_BORDER_OFFSET) {
        if (wardSave()) {
            // Saved! Turn around and pull back onto the board
            reverseDirection();
            clampPlayerToBoard();
        } else {
            game = false;
            PlaySound(hurtSound);
        }
    }
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
        PLAYER_B_DEFAULT_R, PLAYER_B_DEFAULT_G, PLAYER_B_DEFAULT_B, PLAYER_B_DEFAULT_A, 
        prevX, prevY, 
        (float)prevX, (float)prevY, // Spawns directly in place visually
        0,
        10.0f
    };
    
    bodies[player1.length] = newSegment;
}

void updatePlayerLength(void) {
    if (player1.length < player1.prevlen && player1.length > 0) {
        for (int i = player1.length; i < player1.prevlen; i++) {
            memset(&bodies[i], 0, sizeof(struct Player));
        }
        player1.prevlen = player1.length;
    } else if (player1.length < 0) {
        if (wardSave()) {
            // Saved! Claw back to 1 segment and clear the poison
            player1.length = 1;
            player1.prevlen = 0;

            poisoned = false;
            poisonIncr = 0;
            poisonTick = 0;
            poisonDebounce = false;
            poisonColorAlt = false;

            player1.r = PLAYER_H_DEFAULT_R;
            player1.g = PLAYER_H_DEFAULT_G;
            player1.b = PLAYER_H_DEFAULT_B;
            player1.a = PLAYER_H_DEFAULT_A;
        } else {
            game = false;
        }
    }
    
    while (player1.stomach >= 1)
    {
        // Growth gets costlier the longer the snake gets, so one board full
        // of fruit can't snowball into a board-spanning super snake.
        int growthCap = GROWTH_BASE
                      + (int)(GROWTH_SCALE * (float)player1.length);
        if (player1.stomach < growthCap) break;

        player1.stomach -= growthCap;
        player1.prevlen = player1.length;
        if (strcmp(direction, "right") == 0) createBody(0);
        else if (strcmp(direction, "left") == 0) createBody(1);
        else if (strcmp(direction, "up") == 0) createBody(2);
        else if (strcmp(direction, "down") == 0) createBody(3);
        PlaySound(growSound);
        player1.length++;
    }
}

Color hangryTint(Color c)
{
    // Greed mode = pure angry red
    c.r = 255;
    c.g = 0;
    c.b = 0;
    return c;
}

// Dark deeper red for the body segments — charred ember tone.
// The base body color is green, so barely any of it is kept or the
// green/grey dominates and the result reads as grey.
Color hangryTintDark(Color c)
{
    c.r = 255;
    c.g = 0;
    c.b = 0;
    return c;
}

// Radiating vibrant purple for the giant grape buff
Color giantTint(Color c)
{
    c.r = (unsigned char)(c.r * 0.20f + 50.0f * 0.80f);
    c.g = (unsigned char)(c.g * 0.20f + 0.0f * 0.80f);
    c.b = (unsigned char)(c.b * 0.20f + 255.0f * 0.80f);
    return c;
}

// Slightly deeper vibrant purple for the body segments
Color giantTintDark(Color c)
{
    c.r = (unsigned char)(c.r * 0.12f + 50.0f * 0.88f);
    c.g = (unsigned char)(c.g * 0.12f + 0.0f * 0.88f);
    c.b = (unsigned char)(c.b * 0.12f + 255.0f * 0.88f);
    return c;
}

// Flashes bright neon (0,255,0) green / black in sync with the map
Color timeTravelTint(Color c)
{
    if (sinf(GetTime() * TIME_FLASH_RATE) <= 0.0f)
    {
        c.r = 0;
        c.g = 0;
        c.b = 0;
    }
    else
    {
        c.r = 0;
        c.g = 255;
        c.b = 0;
    }
    return c;
}

// Same (0,255,0) green / black flash for the body segments mid-rewind
Color timeTravelTintDark(Color c)
{
    if (sinf(GetTime() * TIME_FLASH_RATE) <= 0.0f)
    {
        c.r = 0;
        c.g = 0;
        c.b = 0;
    }
    else
    {
        c.r = (unsigned char)(c.r * 0.10f);
        c.g = 255;
        c.b = (unsigned char)(c.b * 0.08f);
    }
    return c;
}

float warpOffsetX(void)
{
    if (!warp_active)
        return 0.0f;

    float t = warpTimer / WARP_DURATION;
    if (t > 1.0f) t = 1.0f;

    float e = t * t * (3.0f - 2.0f * t);

    return (warpTargetX - warpStartX) * e;
}

float warpOffsetY(void)
{
    if (!warp_active)
        return 0.0f;

    float t = warpTimer / WARP_DURATION;
    if (t > 1.0f) t = 1.0f;

    float e = t * t * (3.0f - 2.0f * t);

    return (warpTargetY - warpStartY) * e;
}

void updateWarp(void)
{
    if (!warp_active)
        return;

    warpTimer += GetFrameTime();

    if (warpTimer < WARP_DURATION)
        return;

    float dx = warpTargetX - warpStartX;
    float dy = warpTargetY - warpStartY;

    player1.x = (int)warpTargetX;
    player1.y = (int)warpTargetY;
    player1.visualX = (float)player1.x;
    player1.visualY = (float)player1.y;

    for (int i = 0; i < player1.length; i++)
    {
        bodies[i].x += (int)dx;
        bodies[i].y += (int)dy;
        bodies[i].visualX = (float)bodies[i].x;
        bodies[i].visualY = (float)bodies[i].y;
    }

    // Keep the ENTIRE snake inside the playable field — a long body can poke
    // out past the border after a warp even though the head lands cleanly.
    // Rigidly shift the whole chain back in so nothing clips the map edge.
    int minSX = player1.x, maxSX = player1.x;
    int minSY = player1.y, maxSY = player1.y;
    for (int i = 0; i < player1.length; i++)
    {
        if (bodies[i].x < minSX) minSX = bodies[i].x;
        if (bodies[i].x > maxSX) maxSX = bodies[i].x;
        if (bodies[i].y < minSY) minSY = bodies[i].y;
        if (bodies[i].y > maxSY) maxSY = bodies[i].y;
    }

    int fieldMin = UI_BORDER_OFFSET;
    int fieldMax = SCREEN_WIDTH - UI_BORDER_OFFSET - PLAYER_SIZE;
    int shiftX = 0, shiftY = 0;
    if (minSX < fieldMin) shiftX = fieldMin - minSX;
    if (maxSX > fieldMax) shiftX = fieldMax - maxSX;
    if (minSY < fieldMin) shiftY = fieldMin - minSY;
    if (maxSY > fieldMax) shiftY = fieldMax - maxSY;

    if (shiftX != 0 || shiftY != 0)
    {
        player1.x += shiftX;
        player1.y += shiftY;
        player1.visualX = (float)player1.x;
        player1.visualY = (float)player1.y;

        for (int i = 0; i < player1.length; i++)
        {
            bodies[i].x += shiftX;
            bodies[i].y += shiftY;
            bodies[i].visualX = (float)bodies[i].x;
            bodies[i].visualY = (float)bodies[i].y;
        }
    }

    // Landed at the level start — the whole board rewinds with us.
    if (timeTravel_active)
    {
        // Time rocks that were already eaten stay eaten: everything else
        // (including fruits that were consumed) comes back.
        bool rockEaten[FRUIT_COUNT];
        for (int i = 0; i < FRUIT_COUNT; i++)
        {
            rockEaten[i] =
                strcmp(timeSnapshot[i].ability, "Time") == 0 &&
                !foods[i].active;
        }

        for (int i = 0; i < FRUIT_COUNT; i++)
            foods[i] = timeSnapshot[i];

        for (int i = 0; i < FRUIT_COUNT; i++)
        {
            if (rockEaten[i])
                foods[i].active = false;
        }

        direction = timeStartDirection;
        fruitsTotalThisLevel = timeFruitTotal;

        timeTravel_active = false;
        timeTravelTimer = 0.0f;
    }

    // The warp consumed ~a second of wall-clock time while movement was
    // frozen; don't let the snake sprint those steps off after landing.
    clearMoveTimer = true;

    clearInputQueue();
    warp_active = false;
    warpTimer = 0.0f;
}

void renderPlayerBodies(float lerpFactor, float headDrawX, float headDrawY) {

    // Unified body: every segment wears the head sprite, sized to the step
    // (giant = head size), and rotates to follow the segment ahead so the
    // whole snake leans through corners as one continuous chain.
    float centersX[MAX_P_LENGTH];
    float centersY[MAX_P_LENGTH];
    Color segColor[MAX_P_LENGTH];

    for (int i = 0; i < player1.length; i++)
    {
        if (bodies[i].growing)
        {
            const float growSpeed = 0.8f;

            bodies[i].size += growSpeed;

            if (bodies[i].size >= PLAYER_SIZE)
            {
                bodies[i].size = PLAYER_SIZE;
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

        float drawX = bodies[i].visualX +
            (bodies[i].x - bodies[i].visualX) * lerpFactor;
        float drawY = bodies[i].visualY +
            (bodies[i].y - bodies[i].visualY) * lerpFactor;

        drawX += warpOffsetX();
        drawY += warpOffsetY();

        // Centered on the tile, matching the head's anchor
        centersX[i] = drawX + PLAYER_SIZE / 2.0f;
        centersY[i] = drawY + PLAYER_SIZE / 2.0f;

        segColor[i] = (Color){
            (unsigned char)bodies[i].r,
            (unsigned char)bodies[i].g,
            (unsigned char)bodies[i].b,
            (unsigned char)bodies[i].a
        };

        if (timeTravel_active)
            segColor[i] = timeTravelTintDark(segColor[i]);
        else if (mult_active)
            segColor[i] = hangryTintDark(segColor[i]);
        else if (giant_active)
            segColor[i] = giantTintDark(segColor[i]);
    }

    // Giant keeps a spine underneath so the bigger chain reads continuous
    if (giant_active)
    {
        float spineW = PLAYER_SIZE * GIANT_BODY_SCALE;

        for (int i = 0; i + 1 < player1.length; i++)
        {
            DrawLineEx(
                (Vector2){ centersX[i], centersY[i] },
                (Vector2){ centersX[i + 1], centersY[i + 1] },
                spineW,
                segColor[i]
            );
        }

        if (player1.length >= 1)
        {
            DrawLineEx(
                (Vector2){ centersX[0], centersY[0] },
                (Vector2){ headDrawX + PLAYER_SIZE / 2.0f,
                           headDrawY + PLAYER_SIZE / 2.0f },
                spineW,
                segColor[0]
            );
        }
    }

    for (int i = 0; i < player1.length; i++)
    {
        float prevX = centersX[i];
        float prevY = centersY[i];

        if (i > 0) { prevX = centersX[i - 1]; prevY = centersY[i - 1]; }
        else if (player1.length >= 1)
        {
            prevX = headDrawX + PLAYER_SIZE / 2.0f;
            prevY = headDrawY + PLAYER_SIZE / 2.0f;
        }

        // Face the chain ahead of this segment, like the head faces its neck
        float segRot = 0.0f;
        float dx = prevX - centersX[i];
        float dy = prevY - centersY[i];

        if (dx * dx + dy * dy > 1.0f)
            segRot = atan2f(dy, dx) * RAD2DEG;

        float segSize = giant_active
            ? PLAYER_SIZE * GIANT_MOUTH_SCALE
            : bodies[i].size;

        DrawTexturePro(
            headTexture,
            (Rectangle){
                0,
                0,
                (float)headTexture.width,
                (float)headTexture.height
            },
            (Rectangle){
                centersX[i],
                centersY[i],
                segSize,
                segSize
            },
            (Vector2){
                segSize / 2.0f,
                segSize / 2.0f
            },
            segRot,
            segColor[i]
        );
    }
}

void updateBodyRipple(void)
{
    if (!rippleActive)
        return;

    rippleTimer += GetFrameTime();

    for (int i = 0; i < player1.length; i++)
    {
        float delay = i * RIPPLE_SPEED;
        float time = rippleTimer - delay;

        if (time >= 0.0f && time < 0.15f)
        {
            float progress = time / 0.15f;

            // Grow then shrink
            if (progress < 0.5f)
            {
                float t = progress / 0.5f;
                bodies[i].size = BODY_SIZE + RIPPLE_SIZE * t;
            }
            else
            {
                float t = (progress - 0.5f) / 0.5f;
                bodies[i].size = BODY_SIZE + RIPPLE_SIZE * (1.0f - t);
            }
        }
        else if (time >= 0.15f)
        {
            bodies[i].size = BODY_SIZE;
        }
    }

    // End after the ripple passed the entire body
    if (rippleTimer > player1.length * RIPPLE_SPEED + 0.15f)
    {
        rippleActive = false;

        for (int i = 0; i < player1.length; i++)
            bodies[i].size = BODY_SIZE;
    }
}

void activateScaredMode(void)
{
    scared = true;
    scaredTimer = SCARED_DURATION;
}

void updateScaredMode(void)
{
    if (!scared)
        return;

    scaredTimer -= GetFrameTime();

    if (scaredTimer <= 0.0f)
    {
        scaredTimer = 0.0f;
        scared = false;
        return;
    }
}

void triggerJumpscare(void)
{
    jumpscareActive = true;
    jumpscareTimer = JUMPSCARE_DURATION;
    PlaySound(jumpScareSound);
}

void updateJumpscare(void)
{
    if (!jumpscareActive)
        return;

    jumpscareTimer -= GetFrameTime();

    if (jumpscareTimer <= 0.0f)
    {
        jumpscareTimer = 0.0f;
        jumpscareActive = false;
        StopSound(jumpScareSound);
    }
}

int getEyeTargetFruit(void)
{
    const float MIN_LOOK_DISTANCE = 60.0f;
    const float SWITCH_MARGIN = 30.0f;

    // Current eye/head position
    float eyeX = player1.x + player1.size / 2.0f;
    float eyeY = player1.y + player1.size / 2.0f;

    // Check whether our current target is still valid
    bool currentTargetValid = false;
    float currentDistanceSq = FLT_MAX;

    if (eyeTargetFruit >= 0 &&
        eyeTargetFruit < FRUIT_COUNT &&
        foods[eyeTargetFruit].active &&
        strcmp(foods[eyeTargetFruit].ability, "Fruit") == 0)
    {
        float targetX = foods[eyeTargetFruit].x + foods[eyeTargetFruit].size / 2.0f;
        float targetY = foods[eyeTargetFruit].y + foods[eyeTargetFruit].size / 2.0f;

        float dx = targetX - eyeX;
        float dy = targetY - eyeY;

        currentDistanceSq = dx * dx + dy * dy;
        currentTargetValid = true;
    }

    // Find the closest fruit
    int closestFruit = -1;
    float closestDistanceSq = FLT_MAX;

    for (int i = 0; i < FRUIT_COUNT; i++)
    {
        if (!foods[i].active)
            continue;

        if (strcmp(foods[i].ability, "Fruit") != 0)
            continue;

        float targetX = foods[i].x + foods[i].size / 2.0f;
        float targetY = foods[i].y + foods[i].size / 2.0f;

        float dx = targetX - eyeX;
        float dy = targetY - eyeY;

        float distanceSq = dx * dx + dy * dy;

        if (distanceSq < closestDistanceSq)
        {
            closestDistanceSq = distanceSq;
            closestFruit = i;
        }
    }

    // No fruit
    if (closestFruit == -1)
    {
        eyeTargetFruit = -1;
        return -1;
    }

    // If there is no current target, choose the closest fruit
    if (!currentTargetValid)
    {
        if (sqrtf(closestDistanceSq) >= MIN_LOOK_DISTANCE)
        {
            eyeTargetFruit = closestFruit;
            return eyeTargetFruit;
        }

        eyeTargetFruit = -1;
        return -1;
    }

    // Stay locked unless another fruit is significantly closer
    float switchMarginSq = SWITCH_MARGIN * SWITCH_MARGIN;

    if (closestDistanceSq + switchMarginSq < currentDistanceSq)
    {
        eyeTargetFruit = closestFruit;
    }

    // Stop looking if current target gets too close
    if (currentDistanceSq < MIN_LOOK_DISTANCE * MIN_LOOK_DISTANCE)
    {
        eyeTargetFruit = -1;
    }

    return eyeTargetFruit;
}

void renderEye(float headDrawX, float headDrawY)
{
    float eyeX = headDrawX + player1.size / 2.0f;
    float eyeY = headDrawY + player1.size / 2.0f;

    float eyeRadius = player1.size * 0.25f;
    float pupilRadius = player1.size * 0.10f;

    DrawCircle(
        eyeX,
        eyeY,
        eyeRadius,
        WHITE
    );

    float pupilX = eyeX;
    float pupilY = eyeY;

    if (timeTravel_active)
    {
        // Dizzy spin — the pupil orbits the eye while the snake rewinds
        float spin = GetTime() * 15.0f;
        float dizzyOffset = eyeRadius * 0.45f;
        pupilX = eyeX + cosf(spin) * dizzyOffset;
        pupilY = eyeY + sinf(spin) * dizzyOffset;
    }
    else
    {
        int fruitIndex = getEyeTargetFruit();

        if (fruitIndex >= 0)
        {
            float targetX = foods[fruitIndex].x +
                            foods[fruitIndex].size / 2.0f;

            float targetY = foods[fruitIndex].y +
                            foods[fruitIndex].size / 2.0f;

            float dx = targetX - eyeX;
            float dy = targetY - eyeY;

            float distance = sqrtf(dx * dx + dy * dy);

            if (distance > 0.001f)
            {
                dx /= distance;
                dy /= distance;

                float maxOffset = eyeRadius - pupilRadius;

                pupilX += dx * maxOffset;
                pupilY += dy * maxOffset;
            }
        }
    }

    float eyeBorder = 2.0f;

    DrawCircle(
        eyeX,
        eyeY,
        eyeRadius + eyeBorder,
        BLACK
    );

    DrawCircle(
        eyeX,
        eyeY,
        eyeRadius,
        WHITE
    );
    DrawCircle(
        pupilX,
        pupilY,
        pupilRadius,
        BLACK
    );
}

void checkSelfCollision(void) {
    for (int i = 0; i < player1.length; i++) {
        if (player1.x == bodies[i].x && player1.y == bodies[i].y) {
            if (wardSave()) {
                // Saved! U-turn right out of the way
                reverseDirection();
            } else {
                game = false;
                PlaySound(hurtSound);
            }
            break;
        }
    }
}



void starve(void) {
    if (paused == true) return;
    if (player1.stomach <= 0.0f) return;

    // Frame-rate independent hunger: base metabolism plus a tax per segment,
    // so a long snake has to keep feeding to stay full.
    float rate = BASE_METABOLISM
               + METABOLISM_PER_SEGMENT * (float)player1.length;

    player1.stomach -= rate * GetFrameTime();

    if (player1.stomach < 0.0f)
        player1.stomach = 0.0f;
}

void triggerHealAbility(void)
{
    // Cure poison status
    poisoned = false;
    poisonIncr = 0;
    poisonTick = 0;
    poisonDebounce = false;
    poisonColorAlt = false;

    // Restore healthy colors
    player1.r = PLAYER_H_DEFAULT_R;
    player1.g = PLAYER_H_DEFAULT_G;
    player1.b = PLAYER_H_DEFAULT_B;
    player1.a = PLAYER_H_DEFAULT_A;

    for (int i = 0; i < MAX_P_LENGTH; i++)
    {
        bodies[i].r = PLAYER_B_DEFAULT_R;
        bodies[i].g = PLAYER_B_DEFAULT_G;
        bodies[i].b = PLAYER_B_DEFAULT_B;
        bodies[i].a = PLAYER_B_DEFAULT_A;
    }

    // Small energy bonus as the "healing" bump
    player1.stomach += 2.0f;
    spawnEnergyPopup(2.0f, false);

    // Holy glow envelops the player for a while
    holyTimer += HOLY_DURATION;
    holy_active = true;

    // Verdant burst around the player
    for (int s = 0; s < 40; s++)
    {
        spawnGreedSparkle(
            player1.x + player1.size / 2.0f,
            player1.y + player1.size / 2.0f,
            (Color){0, 255, 150, 255}
        );
    }

    PlaySound(levelUpSound);
}

void whilePoisoned(void) {
    // A held heal auto-fires BEFORE any damage tick lands, so healing fully
    // negates a dose instead of only stopping a single tick of it.
    if (poisoned)
    {
        for (int i = 0; i < MAX_EFFECT_FRAMES; i++)
        {
            if (effectFrames[i].active &&
                effectFrames[i].clickable &&
                effectFrames[i].count >= 1 &&
                strcmp(effectFrames[i].ability, "Heal") == 0)
            {
                useEffectFrame(i);
                break;
            }
        }
    }

    // Actively poisoned: apply ONE segment of damage per tick, dragging out
    // the dose on a small internal clock so it drains gradually.
    if (poisoned && poisonIncr >= 1)
    {
        if (!poisonDebounce)
        {
            poisonDebounce = true;
            poisonIncr--;
            onPlayerEatingFood(-1, false);
            PlaySound(hurtSound);
            score = player1.length;

            if (!poisonColorAlt) {
                poisonColorAlt = true;
                player1.r = 0;
                player1.g = 155;
                player1.b = 0;
                player1.a = 255;
                for (int i = 0; i < MAX_P_LENGTH; i++) {
                    bodies[i].r = 0;
                    bodies[i].g = 255;
                    bodies[i].b = 0;
                    bodies[i].a = 255;
                }
            } else {
                poisonColorAlt = false;
                player1.r = 0;
                player1.g = 95;
                player1.b = 0;
                player1.a = 255;
                for (int i = 0; i < MAX_P_LENGTH; i++) {
                    bodies[i].r = 0;
                    bodies[i].g = 155;
                    bodies[i].b = 0;
                    bodies[i].a = 255;
                }
            }
        }
        else
        {
            if (poisonTick < 30) {
                poisonTick++;
            } else {
                poisonTick = 0;
                poisonDebounce = false;
            }
        }
        return;
    }

    // Dose fully drained — clear the poison status completely so the snake is
    // healthy again and any LATER poison apple is a fresh single dose (not
    // stacked on top of a stuck one).
    if (poisonIncr <= 0)
    {
        poisonTick = 0;
        poisonDebounce = false;
        poisonColorAlt = false;
        poisoned = false;

        player1.r = PLAYER_H_DEFAULT_R;
        player1.g = PLAYER_H_DEFAULT_G;
        player1.b = PLAYER_H_DEFAULT_B;
        player1.a = PLAYER_H_DEFAULT_A;
        for (int i = 0; i < MAX_P_LENGTH; i++) {
            bodies[i].r = PLAYER_B_DEFAULT_R;
            bodies[i].g = PLAYER_B_DEFAULT_G;
            bodies[i].b = PLAYER_B_DEFAULT_B;
            bodies[i].a = PLAYER_B_DEFAULT_A;
        }
    }
}

#endif