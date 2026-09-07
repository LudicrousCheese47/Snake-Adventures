#ifndef PLAYER_FUNCTIONS_H
#define PLAYER_FUNCTIONS_H

#include "Definitions.h"

void isTouchingEdge(void) {
    if (player1.x < 0+UI_BORDER_OFFSET || player1.x >= SCREEN_WIDTH-UI_BORDER_OFFSET || player1.y < 0+UI_BORDER_OFFSET || player1.y >= SCREEN_HEIGHT-UI_BORDER_OFFSET) {
        game = false;
        PlaySound(hurtSound);
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
        0
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
        game = false;
    }
    
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
        
      //  DrawRectangle(
        //    drawX, 
        //    drawY,
        //    bodies[i].size,
         //   bodies[i].size,
         //   (Color){bodies[i].r2, bodies[i].b2, bodies[i].g2, bodies[i].a2}
       // );
        
       // Rectangle bodiesRectangle = { drawX, drawY, bodies[i].size, bodies[i].size };
                
       // DrawRectangleRec(
        //    bodiesRectangle,
        //    (Color){bodies[i].r, bodies[i].g, bodies[i].b, bodies[i].a}
       // );
                
       // DrawRectangleLinesEx(
       //     bodiesRectangle, 
        //    2.0f,
         //   (Color){0, 0, 0, 255}
        //);
        
        Color bodyColor = {
            (unsigned char)bodies[i].r,
            (unsigned char)bodies[i].g,
            (unsigned char)bodies[i].b,
            (unsigned char)bodies[i].a
        };

        DrawTexturePro(
            bodyTexture,
            (Rectangle){
                0,
                0,
                (float)bodyTexture.width,
                (float)bodyTexture.height
            },
            (Rectangle){
                drawX + bodies[i].size / 2.0f,
                drawY + bodies[i].size / 2.0f,
                bodies[i].size,
                bodies[i].size
            },
            (Vector2){
                bodies[i].size / 2.0f,
                bodies[i].size / 2.0f
            },
            0.0f,
            bodyColor
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
            game = false;
            PlaySound(hurtSound);
            break;
        }
    }
}



void starve(void) {
    if (player1.stomach <= 0 || paused == true) return;
    if (player1.stomach > 0) foodTick+=1;
    if (foodTick >= 5 && player1.stomach > 0) player1.stomach -= tickIncr; 
}

void whilePoisoned(void) {
    int prevIncr = poisonIncr;
    if (poisonIncr >= 1 && !poisonDebounce && poisoned) {
        poisonDebounce = true;
        poisonIncr--;
        onPlayerEatingFood(-1);
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
    } else if (poisonDebounce) {
        if (poisonTick < 30) {
            poisonTick++;
        } else {
            poisonTick = 0;
            poisonDebounce = false;
        }
    } else if (poisonIncr <= 0) {
        poisonIncr = prevIncr;
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