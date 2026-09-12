#ifndef PARTICLE_FUNCS_H
#define PARTICLE_FUNCS_H

#include "Definitions.h"

unsigned char clampColor(int value)
{
    if (value < 0)
        return 0;

    if (value > 255)
        return 255;

    return (unsigned char)value;
}

void spawnFruitParticles(float x, float y, Color color)
{
    const int PARTICLE_COUNT = 40;

    for (int p = 0; p < PARTICLE_COUNT; p++)
    {
        for (int i = 0; i < MAX_PARTICLES; i++)
        {
            if (!particles[i].active)
            {
                particles[i].active = true;

                // Small random offset so everything doesn't start
                // from exactly the same pixel.
                particles[i].x =
                    x + (float)GetRandomValue(-4, 4);

                particles[i].y =
                    y + (float)GetRandomValue(-4, 4);

                particles[i].velocityX =
                    (float)GetRandomValue(-450, 450) / 10.0f;

                particles[i].velocityY =
                    (float)GetRandomValue(-500, 150) / 10.0f;

                // A mix of tiny droplets and larger chunks
                particles[i].size =
                    (float)GetRandomValue(2, 9);

                particles[i].maxLifetime =
                    (float)GetRandomValue(30, 80) / 100.0f;

                particles[i].lifetime =
                    particles[i].maxLifetime;

                particles[i].color = color;

                // Slightly vary the color of each droplet
                int rVariation = GetRandomValue(-20, 20);
                int gVariation = GetRandomValue(-20, 20);
                int bVariation = GetRandomValue(-20, 20);

                particles[i].color.r =
                    clampColor(particles[i].color.r + rVariation);

                particles[i].color.g =
                    clampColor(particles[i].color.g + gVariation);

                particles[i].color.b =
                    clampColor(particles[i].color.b + bVariation);

                particles[i].color.a = 255;

                break;
            }
        }
    }
}

void spawnGreedSparkle(float x, float y, Color baseColor)
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (particles[i].active)
            continue;

        particles[i].active = true;

        // Small corner-of-apple offset
        particles[i].x = x + (float)GetRandomValue(-8, 8);
        particles[i].y = y + (float)GetRandomValue(-8, 8);

        // Drift upward slowly (beats gravity for a moment),
        // with a little sideways wiggle
        particles[i].velocityX = (float)GetRandomValue(-20, 20);
        particles[i].velocityY = (float)GetRandomValue(-60, -30);

        particles[i].size = (float)GetRandomValue(2, 4);

        particles[i].maxLifetime = (float)GetRandomValue(40, 70) / 100.0f;
        particles[i].lifetime = particles[i].maxLifetime;

        // Source fruit color, so fruit, particles and bar all match
        particles[i].color = (Color){
            baseColor.r,
            baseColor.g,
            baseColor.b,
            255
        };
        break;
    }
}

void updateGreedAura(void)
{
    for (int i = 0; i < FRUIT_COUNT; i++)
    {
        if (foods[i].active && strcmp(foods[i].ability, "Mult") == 0)
        {
            spawnGreedSparkle(
                foods[i].x + foods[i].size / 2.0f,
                foods[i].y + foods[i].size / 2.0f,
                (Color){
                    (unsigned char)foods[i].r,
                    (unsigned char)foods[i].g,
                    (unsigned char)foods[i].b,
                    255
                }
            );
        }
    }
}

void updateEnergyBarSparkles(void)
{
    if (!mult_active) return;

    float barX = ENERGY_BAR_X;
    float barY = ENERGY_BAR_Y;
    float barWidth = ENERGY_BAR_WIDTH;
    float maxHeight = ENERGY_BAR_MAX_HEIGHT;

    float barHeight = player1.stomach * ENERGY_BAR_SCALE;
    if (barHeight > maxHeight) barHeight = maxHeight;
    if (barHeight <= 0.0f) return;

    // Sparkles rise off the top edge of the current fill
    spawnGreedSparkle(
        barX + (float)GetRandomValue(0, (int)barWidth),
        barY + maxHeight - barHeight,
        (Color){(unsigned char)multR, (unsigned char)multG, (unsigned char)multB, (unsigned char)multA}
    );
}

void spawnFireEmber(float x, float y)
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (particles[i].active)
            continue;

        particles[i].active = true;

        particles[i].x = x + (float)GetRandomValue(-2, 2);
        particles[i].y = y + (float)GetRandomValue(-2, 2);

        // Embers rise, then shared gravity slowly pulls them back down
        particles[i].velocityX = (float)GetRandomValue(-15, 15);
        particles[i].velocityY = (float)GetRandomValue(-90, -40);

        particles[i].size = (float)GetRandomValue(2, 6);

        particles[i].maxLifetime = (float)GetRandomValue(25, 55) / 100.0f;
        particles[i].lifetime = particles[i].maxLifetime;

        // Fire palette: fire red embers
        static const Color flameColors[] = {
            {180, 15, 10, 255},    // deep red
            {220, 25, 10, 255},    // flame red
            {255, 40, 10, 255},    // fire red
            {255, 70, 15, 255},    // red-orange
            {255, 110, 25, 255},   // bright ember
            {255, 170, 60, 255},   // hot orange
            {255, 220, 130, 255},  // yellow-white heat
            {255, 255, 240, 255}   // white-hot core
        };
        particles[i].color = flameColors[GetRandomValue(0, 7)];

        break;
    }
}

void spawnSprintTrail(float x, float y)
{
    // Two embers per frame at the player gives a visible flame trail
    spawnFireEmber(x, y);
    spawnFireEmber(x, y);
}

void updateSprintEffects(void)
{
    if (!sprint_active)
        return;

    // Speed-boost flame trail at the player head
    spawnSprintTrail(
        player1.x + PLAYER_SIZE / 2.0f,
        player1.y + PLAYER_SIZE / 2.0f
    );

    // Wind streaks kicked up behind the head along the travel direction —
    // burst speed reads physically instead of just feeling faster.
    float hx = player1.x + PLAYER_SIZE / 2.0f;
    float hy = player1.y + PLAYER_SIZE / 2.0f;
    float offX = 0.0f;
    float offY = 0.0f;
    if (strcmp(direction, "right") == 0)       offX = -16.0f;
    else if (strcmp(direction, "left") == 0)   offX =  16.0f;
    else if (strcmp(direction, "up") == 0)     offY =  16.0f;
    else if (strcmp(direction, "down") == 0)   offY = -16.0f;

    for (int s = 0; s < 2; s++)
    {
        spawnGreedSparkle(
            hx + offX + (float)GetRandomValue(-3, 3),
            hy + offY + (float)GetRandomValue(-3, 3),
            (Color){ 235, 235, 240, 150 }
        );
    }

    // Fire rising off the top of the energy bar fill (burning energy)
    float barX = ENERGY_BAR_X;
    float barY = ENERGY_BAR_Y;
    float barWidth = ENERGY_BAR_WIDTH;
    float maxHeight = ENERGY_BAR_MAX_HEIGHT;

    float barHeight = player1.stomach * ENERGY_BAR_SCALE;
    if (barHeight > maxHeight) barHeight = maxHeight;
    if (barHeight <= 0.0f) return;

    spawnFireEmber(
        barX + (float)GetRandomValue(0, (int)barWidth),
        barY + maxHeight - barHeight
    );
}

void updateMagnetEffects(void)
{
    // Blue aura drifting off the magnet apple (red fruit + blue magnet)
    for (int i = 0; i < FRUIT_COUNT; i++)
    {
        if (foods[i].active && strcmp(foods[i].ability, "Magnet") == 0)
        {
            spawnGreedSparkle(
                foods[i].x + foods[i].size / 2.0f,
                foods[i].y + foods[i].size / 2.0f,
                (Color){0, 120, 255, 255}
            );
        }
    }

    // Flicker around the player while the field is active
    if (magnet_active)
    {
        spawnGreedSparkle(
            player1.x + player1.size / 2.0f,
            player1.y + player1.size / 2.0f,
            (Color){0, 120, 255, 255}
        );
    }
}

void updateHealAura(void)
{
    // Verdant sparkles drifting off every healing apple
    for (int i = 0; i < FRUIT_COUNT; i++)
    {
        if (foods[i].active && strcmp(foods[i].ability, "Heal") == 0)
        {
            spawnGreedSparkle(
                foods[i].x + foods[i].size / 2.0f,
                foods[i].y + foods[i].size / 2.0f,
                (Color){0, 255, 150, 255}
            );
        }
    }
}

#define HOLY_GLOW_PAD 10.0f

void spawnTornDebris(float x, float y)
{
    for (int i = 0; i < MAX_GROUND_PARTICLES; i++)
    {
        if (groundParticles[i].active)
            continue;

        groundParticles[i].active = true;

        groundParticles[i].x = x + (float)GetRandomValue(-8, 8);
        groundParticles[i].y = y + (float)GetRandomValue(-4, 4);

        // Torn-up ground chunks — kicked up and out in every direction
        groundParticles[i].velocityX = (float)GetRandomValue(-220, 220) / 10.0f;
        groundParticles[i].velocityY = (float)GetRandomValue(-320, -40) / 10.0f;

        groundParticles[i].size = (float)GetRandomValue(3, 8);

        groundParticles[i].maxLifetime = (float)GetRandomValue(35, 70) / 100.0f;
        groundParticles[i].lifetime = groundParticles[i].maxLifetime;

        // Same color as the ground: mix of the board color and its
        // dark checker sibling with a little natural jitter
        Color base = (GetRandomValue(0, 100) <= 50) ? boardColor : boardColorDark;

        int rVar = GetRandomValue(-25, 25);
        int gVar = GetRandomValue(-25, 25);
        int bVar = GetRandomValue(-25, 25);

        groundParticles[i].color = (Color){
            clampColor(base.r + rVar),
            clampColor(base.g + gVar),
            clampColor(base.b + bVar),
            255
        };

        break;
    }
}

void updateHangryEffect(void)
{
    if (!mult_active)
        return;

    // The ground under the greedy snake gets torn up and flung everywhere
    spawnTornDebris(
        player1.x + player1.size / 2.0f,
        player1.y + player1.size / 2.0f
    );

    spawnTornDebris(
        player1.x + player1.size / 2.0f,
        player1.y + player1.size / 2.0f
    );

    // Chunks rip out from under the tail as it drags along
    int tailIndex = player1.length - 1;
    if (tailIndex >= 1)
    {
        if (GetRandomValue(0, 100) <= 70)
            spawnTornDebris(
                bodies[tailIndex].x + bodies[tailIndex].size / 2.0f,
                bodies[tailIndex].y + bodies[tailIndex].size / 2.0f
            );
    }

    // Middle body occasionally gouges the ground too
    if (GetRandomValue(0, 100) <= 35)
    {
        int mid = player1.length / 2;
        if (mid >= 1 && mid < player1.length)
        {
            spawnTornDebris(
                bodies[mid].x + bodies[mid].size / 2.0f,
                bodies[mid].y + bodies[mid].size / 2.0f
            );
        }
    }
}

void renderHangryAura(float headX, float headY)
{
    if (!mult_active)
        return;

    // Flickering pressure-cooker glow — too hungry to think straight
    float flicker = 0.5f + 0.5f * (float)GetRandomValue(0, 100) / 100.0f;
    float pulse = 0.6f + 0.4f * sinf(GetTime() * 11.0f);
    float intensity = pulse * flicker;

    for (int i = 0; i < player1.length; i++)
    {
        float cx = bodies[i].x + bodies[i].size / 2.0f;
        float cy = bodies[i].y + bodies[i].size / 2.0f;

        DrawCircleGradient(
            (Vector2){ cx, cy },
            16.0f + 4.0f * flicker,
            (Color){255, 60, 25, (unsigned char)(20 + (int)(32.0f * intensity))},
            (Color){180, 10, 0, 0}
        );

        DrawCircleGradient(
            (Vector2){ cx, cy },
            9.0f + 2.0f * flicker,
            (Color){255, 130, 60, (unsigned char)(28 + (int)(30.0f * intensity))},
            (Color){255, 40, 0, 0}
        );
    }

    float hx = headX + player1.size / 2.0f;
    float hy = headY + player1.size / 2.0f;

    DrawCircleGradient(
        (Vector2){ hx, hy },
        26.0f + 6.0f * flicker,
        (Color){255, 80, 30, (unsigned char)(40 + (int)(38.0f * intensity))},
        (Color){210, 15, 0, 0}
    );

    DrawCircleGradient(
        (Vector2){ hx, hy },
        13.0f + 3.0f * flicker,
        (Color){255, 200, 120, (unsigned char)(55 + (int)(30.0f * intensity))},
        (Color){255, 80, 0, 0}
    );
}

void updateHolyGlow(void)
{
    if (!holy_active)
        return;

    holyTimer -= GetFrameTime();

    if (holyTimer <= 0.0f)
    {
        holyTimer = 0.0f;
        holy_active = false;
        return;
    }

    // Rising verdant sparkles along the player's body
    for (int i = 0; i < player1.length; i++)
    {
        if (GetRandomValue(0, 100) <= 18)
        {
            spawnGreedSparkle(
                bodies[i].x + bodies[i].size / 2.0f,
                bodies[i].y + bodies[i].size / 2.0f,
                (Color){0, 255, 150, 255}
            );
        }
    }
}

void spawnWindStreak(float x, float y, float vx, float vy, Color color)
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (particles[i].active)
            continue;

        particles[i].active = true;
        particles[i].x = x;
        particles[i].y = y;
        particles[i].velocityX = vx;
        particles[i].velocityY = vy;
        particles[i].size = (float)GetRandomValue(2, 6);
        particles[i].maxLifetime = (float)GetRandomValue(25, 50) / 100.0f;
        particles[i].lifetime = particles[i].maxLifetime;
        particles[i].color = color;
        particles[i].noGravity = true;
        break;
    }
}

void updateGiantAura(void)
{
    if (!giant_active)
        return;

    float facingX = 0.0f, facingY = 0.0f;
    if (strcmp(direction, "right") == 0)      { facingX = 1.0f;  facingY = 0.0f; }
    else if (strcmp(direction, "left") == 0)  { facingX = -1.0f; facingY = 0.0f; }
    else if (strcmp(direction, "up") == 0)    { facingX = 0.0f;  facingY = -1.0f; }
    else if (strcmp(direction, "down") == 0)  { facingX = 0.0f;  facingY = 1.0f; }

    float mx = player1.x + player1.size / 2.0f;
    float my = player1.y + player1.size / 2.0f;

    // Gusts sweeping into the mouth from the front cone — visible wind
    for (int s = 0; s < 3; s++)
    {
        float len = (float)GetRandomValue(20, (int)GIANT_SUCK_RADIUS);
        float baseAng = atan2f(facingY, facingX) + GetRandomValue(-55, 55) * DEG2RAD;

        float gx = mx + cosf(baseAng) * len;
        float gy = my + sinf(baseAng) * len;

        float dx = mx - gx;
        float dy = my - gy;
        float dist = sqrtf(dx * dx + dy * dy);
        if (dist < 0.1f)
            continue;

        float speed = (float)GetRandomValue(80, 180);

        spawnWindStreak(
            gx,
            gy,
            dx / dist * speed,
            dy / dist * speed,
            (Color){
                (unsigned char)GetRandomValue(130, 210),
                40,
                255,
                190
            }
        );
    }

    // Purple wisps drifting off the body
    if (GetRandomValue(0, 100) <= 30)
    {
        spawnGreedSparkle(
            player1.x + player1.size / 2.0f,
            player1.y + player1.size / 2.0f,
            (Color){170, 40, 255, 255}
        );
    }
}

void renderGiantAura(float headX, float headY)
{
    if (!giant_active)
        return;

    // Deep vibrant purple, pulsing hard on the head and every segment
    float pulse = 0.65f + 0.35f * sinf(GetTime() * 7.0f);

    for (int i = 0; i < player1.length; i++)
    {
        float cx = bodies[i].x + bodies[i].size / 2.0f;
        float cy = bodies[i].y + bodies[i].size / 2.0f;

        DrawCircleGradient(
            (Vector2){ cx, cy },
            GIANT_AURA_RADIUS * (0.8f + 0.4f * pulse),
            (Color){50, 0, 255, (unsigned char)(40 + (int)(28.0f * pulse))},
            (Color){25, 0, 180, 0}
        );

        DrawCircleGradient(
            (Vector2){ cx, cy },
            22.0f * (0.8f + 0.4f * pulse),
            (Color){110, 30, 255, (unsigned char)(70 + (int)(40.0f * pulse))},
            (Color){50, 0, 255, 0}
        );
    }

    // Brighter pulsing bloom over the head
    float hx = headX + player1.size / 2.0f;
    float hy = headY + player1.size / 2.0f;

    DrawCircleGradient(
        (Vector2){ hx, hy },
        (GIANT_AURA_RADIUS + 10.0f) * (0.8f + 0.4f * pulse),
        (Color){80, 10, 255, (unsigned char)(55 + (int)(35.0f * pulse))},
        (Color){30, 0, 170, 0}
    );

    DrawCircleGradient(
        (Vector2){ hx, hy },
        26.0f * (0.8f + 0.4f * pulse),
        (Color){150, 60, 255, (unsigned char)(90 + (int)(45.0f * pulse))},
        (Color){70, 10, 255, 0}
    );

    // Visible suction funnel: a cone stretching out in front of the mouth
    float facingX = 0.0f, facingY = 0.0f;
    if (strcmp(direction, "right") == 0)      { facingX = 1.0f;  facingY = 0.0f; }
    else if (strcmp(direction, "left") == 0)  { facingX = -1.0f; facingY = 0.0f; }
    else if (strcmp(direction, "up") == 0)    { facingX = 0.0f;  facingY = -1.0f; }
    else if (strcmp(direction, "down") == 0)  { facingX = 0.0f;  facingY = 1.0f; }

    float mouthLen = player1.size * GIANT_MOUTH_SCALE * 0.5f;
    float tipX = hx + facingX * mouthLen;
    float tipY = hy + facingY * mouthLen;

    float halfAng = acosf(GIANT_CONE_COS);
    float halfW = tanf(halfAng) * GIANT_SUCK_RADIUS;
    float perpX = -facingY;
    float perpY = facingX;

    float e1x = tipX + perpX * halfW;
    float e1y = tipY + perpY * halfW;
    float e2x = tipX - perpX * halfW;
    float e2y = tipY - perpY * halfW;

    unsigned char funnelA = (unsigned char)(16 + (int)(10.0f * pulse));

    DrawTriangle(
        (Vector2){ tipX, tipY },
        (Vector2){ e1x, e1y },
        (Vector2){ e2x, e2y },
        (Color){150, 0, 255, funnelA}
    );

    DrawTriangleLines(
        (Vector2){ tipX, tipY },
        (Vector2){ e1x, e1y },
        (Vector2){ e2x, e2y },
        (Color){200, 80, 255, (unsigned char)(90.0f * pulse)}
    );
}

void updateWardAura(void)
{
    // Holy gold sparks drifting off every holy banana
    for (int i = 0; i < FRUIT_COUNT; i++)
    {
        if (foods[i].active && strcmp(foods[i].ability, "Ward") == 0)
        {
            spawnGreedSparkle(
                foods[i].x + foods[i].size / 2.0f,
                foods[i].y + foods[i].size / 2.0f,
                (Color){255, 225, 110, 255}
            );
        }
    }

    // Rising sparks off the shielded snake
    if (ward_active)
    {
        spawnGreedSparkle(
            player1.x + player1.size / 2.0f,
            player1.y + player1.size / 2.0f,
            (Color){255, 225, 110, 255}
        );
    }
}

void updateWardShield(void)
{
    if (!ward_active)
        return;

    wardTimer -= GetFrameTime();

    if (wardTimer <= 0.0f)
    {
        wardTimer = 0.0f;
        ward_active = false;
    }
}

void renderWardShield(float headX, float headY)
{
    if (!ward_active)
        return;

    float pulse = 0.6f + 0.4f * sinf(GetTime() * 7.0f);

    // Golden bubble aura hugging the whole body
    for (int i = 0; i < player1.length; i++)
    {
        float cx = bodies[i].x + bodies[i].size / 2.0f;
        float cy = bodies[i].y + bodies[i].size / 2.0f;

        DrawCircleGradient(
            (Vector2){ cx, cy },
            22.0f + 3.0f * pulse,
            (Color){255, 230, 120, (unsigned char)(30 + (int)(26.0f * pulse))},
            (Color){255, 200, 50, 0}
        );

        DrawCircleGradient(
            (Vector2){ cx, cy },
            12.0f + 2.0f * pulse,
            (Color){255, 250, 200, (unsigned char)(55 + (int)(35.0f * pulse))},
            (Color){255, 225, 110, 0}
        );
    }

    // A bright halo floating above the head — a thin donut viewed from
    // the side, wobbling like a coin spinning on a table
    float hx = headX + player1.size / 2.0f;
    float hy = headY + player1.size / 2.0f;

    float haloY = hy - player1.size - 4.0f;
    float wob = GetTime() * 4.5f;
    float tilt = sinf(wob);
    float vRadius = 1.2f + fabsf(tilt) * 3.4f;
    float sway = sinf(wob * 0.5f) * 1.5f;
    float coinCX = hx + sway;

    unsigned char edgeA =
        (unsigned char)(150 + (int)(95.0f * (0.5f + 0.5f * pulse)));

    DrawEllipseLinesV(
        (Vector2){ coinCX, haloY },
        7.0f,
        vRadius,
        (Color){255, 240, 170, edgeA}
    );

    DrawEllipseLinesV(
        (Vector2){ coinCX - 0.6f, haloY },
        7.0f,
        vRadius,
        (Color){255, 245, 200, (unsigned char)(edgeA - 30)}
    );

    DrawEllipseLinesV(
        (Vector2){ coinCX + 0.6f, haloY },
        7.0f,
        vRadius,
        (Color){255, 220, 120, (unsigned char)(edgeA - 50)}
    );

    if (fabsf(tilt) > 0.55f)
    {
        DrawEllipseLinesV(
            (Vector2){ coinCX, haloY },
            7.0f,
            vRadius,
            (Color){255, 255, 235, edgeA}
        );
    }

    DrawCircleGradient(
        (Vector2){ coinCX, haloY },
        12.0f,
        (Color){255, 245, 200, (unsigned char)(45.0f + 25.0f * fabsf(tilt))},
        (Color){255, 230, 120, 0}
    );
}

void renderHolyAura(float headX, float headY)
{
    if (!holy_active)
        return;

    float pulse = 0.55f + 0.45f * sinf(GetTime() * 6.0f);

    // Body: three layered radial glows per segment blend into one soft aura
    for (int i = 0; i < player1.length; i++)
    {
        float cx = bodies[i].x + bodies[i].size / 2.0f;
        float cy = bodies[i].y + bodies[i].size / 2.0f;

        // Outer soft halo
        DrawCircleGradient(
            (Vector2){ cx, cy },
            27.0f + 2.0f * pulse,
            (Color){0, 255, 150, (unsigned char)(30 + (int)(20.0f * pulse))},
            (Color){0, 180, 80, 0}
        );

        // Mid glow
        DrawCircleGradient(
            (Vector2){ cx, cy },
            18.0f + 2.0f * pulse,
            (Color){130, 255, 195, (unsigned char)(62 + (int)(36.0f * pulse))},
            (Color){0, 200, 100, 0}
        );

        // Hot inner core
        DrawCircleGradient(
            (Vector2){ cx, cy },
            10.0f + 1.5f * pulse,
            (Color){220, 255, 235, (unsigned char)(115 + (int)(45.0f * pulse))},
            (Color){80, 255, 165, 0}
        );
    }

    // Head: larger halo + bright bloom
    float hx = headX + player1.size / 2.0f;
    float hy = headY + player1.size / 2.0f;

    DrawCircleGradient(
        (Vector2){ hx, hy },
        38.0f + 3.0f * pulse,
        (Color){0, 255, 150, (unsigned char)(38 + (int)(24.0f * pulse))},
        (Color){0, 180, 80, 0}
    );

    DrawCircleGradient(
        (Vector2){ hx, hy },
        24.0f + 2.0f * pulse,
        (Color){160, 255, 210, (unsigned char)(90 + (int)(40.0f * pulse))},
        (Color){0, 220, 120, 0}
    );

    DrawCircleGradient(
        (Vector2){ hx, hy },
        14.0f + 1.5f * pulse,
        (Color){255, 255, 255, (unsigned char)(155 + (int)(60.0f * pulse))},
        (Color){255, 245, 180, 0}
    );
}

void updateParticleArray(struct Particle *arr, int count)
{
    float dt = GetFrameTime();

    for (int i = 0; i < count; i++)
    {
        if (!arr[i].active)
            continue;

        arr[i].lifetime -= dt;

        if (arr[i].lifetime <= 0.0f)
        {
            arr[i].active = false;
            continue;
        }

        // Move particle
        arr[i].x += arr[i].velocityX * dt;
        arr[i].y += arr[i].velocityY * dt;

        // Gravity
        if (!arr[i].noGravity)
            arr[i].velocityY += 180.0f * dt;
    }
}

void updateParticles(void)
{
    updateParticleArray(particles, MAX_PARTICLES);
}

void updateGroundParticles(void)
{
    updateParticleArray(groundParticles, MAX_GROUND_PARTICLES);
}

void renderParticleArray(struct Particle *arr, int count)
{
    for (int i = 0; i < count; i++)
    {
        if (!arr[i].active)
            continue;

        float lifePercent =
            arr[i].lifetime / arr[i].maxLifetime;

        float particleSize =
            arr[i].size * lifePercent;

        Color particleColor = arr[i].color;
        particleColor.a = (unsigned char)(255.0f * lifePercent);

        // ----------------------------------------
        // Outer glow
        // ----------------------------------------
        Color glow1 = arr[i].color;
        glow1.a = (unsigned char)(20.0f * lifePercent);

        float glowSize1 = particleSize * 5.0f;

        DrawRectangle(
            (int)(arr[i].x - glowSize1 / 2.0f),
            (int)(arr[i].y - glowSize1 / 2.0f),
            (int)glowSize1,
            (int)glowSize1,
            glow1
        );

        // ----------------------------------------
        // Middle glow
        // ----------------------------------------
        Color glow2 = arr[i].color;
        glow2.a = (unsigned char)(40.0f * lifePercent);

        float glowSize2 = particleSize * 3.5f;

        DrawRectangle(
            (int)(arr[i].x - glowSize2 / 2.0f),
            (int)(arr[i].y - glowSize2 / 2.0f),
            (int)glowSize2,
            (int)glowSize2,
            glow2
        );

        // ----------------------------------------
        // Inner glow
        // ----------------------------------------
        Color glow3 = arr[i].color;
        glow3.a = (unsigned char)(80.0f * lifePercent);

        float glowSize3 = particleSize * 2.0f;

        DrawRectangle(
            (int)(arr[i].x - glowSize3 / 2.0f),
            (int)(arr[i].y - glowSize3 / 2.0f),
            (int)glowSize3,
            (int)glowSize3,
            glow3
        );

        // ----------------------------------------
        // Main particle
        // ----------------------------------------
        DrawRectangle(
            (int)(arr[i].x - particleSize / 2.0f),
            (int)(arr[i].y - particleSize / 2.0f),
            (int)particleSize,
            (int)particleSize,
            particleColor
        );
    }
}

void renderParticles(void)
{
    renderParticleArray(particles, MAX_PARTICLES);
}

void renderGroundParticles(void)
{
    renderParticleArray(groundParticles, MAX_GROUND_PARTICLES);
}

void spawnFruitSplat(float x, float y, Color color)
{
    for (int i = 0; i < MAX_SPLATS; i++)
    {
        if (!splats[i].active)
        {
            splats[i].active = true;

            splats[i].x = x;
            splats[i].y = y;

            splats[i].size =
                (float)GetRandomValue(10, 18);

            splats[i].lifetime = 1.0f;

            splats[i].color = color;

            // Darker version of the fruit color
            splats[i].color.r =
                (unsigned char)(splats[i].color.r * 0.75f);

            splats[i].color.g =
                (unsigned char)(splats[i].color.g * 0.75f);

            splats[i].color.b =
                (unsigned char)(splats[i].color.b * 0.75f);

            splats[i].color.a = 180;

            splats[i].blobCount =
                GetRandomValue(6, MAX_SPLAT_BLOBS);

            // Generate the splatter layout ONCE
            for (int j = 0; j < splats[i].blobCount; j++)
            {
                float angle =
                    GetRandomValue(0, 359) * DEG2RAD;

                float distance =
                    (float)GetRandomValue(
                        5,
                        (int)(splats[i].size * 2.0f)
                    );

                splats[i].blobX[j] =
                    cosf(angle) * distance;

                splats[i].blobY[j] =
                    sinf(angle) * distance;

                splats[i].blobSize[j] =
                    (float)GetRandomValue(2, 7);
            }

            break;
        }
    }
}

void renderSplats(void)
{
    for (int i = 0; i < MAX_SPLATS; i++)
    {
        if (!splats[i].active)
            continue;

        float alpha = splats[i].lifetime;

        Color splatColor = splats[i].color;
        splatColor.a =
            (unsigned char)(180.0f * alpha);

        // Subtle colored glow
        Color glowColor = splatColor;
        glowColor.a =
            (unsigned char)(55.0f * alpha);

        // Main glow square
        float glowSize = splats[i].size * 2.0f;

        DrawRectangle(
            (int)(splats[i].x - glowSize / 2.0f),
            (int)(splats[i].y - glowSize / 2.0f),
            (int)glowSize,
            (int)glowSize,
            glowColor
        );

        // Main splat
        float mainSize = splats[i].size * 2.0f;

        DrawRectangle(
            (int)(splats[i].x - mainSize / 2.0f),
            (int)(splats[i].y - mainSize / 2.0f),
            (int)mainSize,
            (int)mainSize,
            splatColor
        );

        // Stored square droplets
        for (int j = 0; j < splats[i].blobCount; j++)
        {
            float blobX =
                splats[i].x + splats[i].blobX[j];

            float blobY =
                splats[i].y + splats[i].blobY[j];

            float blobSize =
                splats[i].blobSize[j];

            DrawRectangle(
                (int)(blobX - blobSize / 2.0f),
                (int)(blobY - blobSize / 2.0f),
                (int)blobSize,
                (int)blobSize,
                splatColor
            );
        }
    }
}

void updateSplats(void)
{
    float dt = GetFrameTime();

    for (int i = 0; i < MAX_SPLATS; i++)
    {
        if (!splats[i].active)
            continue;

        splats[i].lifetime -= dt;

        if (splats[i].lifetime <= 0.0f)
        {
            splats[i].lifetime = 0.0f;
            splats[i].active = false;
        }
    }
} 

#endif