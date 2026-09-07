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

void updateParticles(void)
{
    float dt = GetFrameTime();

    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (!particles[i].active)
            continue;

        particles[i].lifetime -= dt;

        if (particles[i].lifetime <= 0.0f)
        {
            particles[i].active = false;
            continue;
        }

        // Move particle
        particles[i].x += particles[i].velocityX * dt;
        particles[i].y += particles[i].velocityY * dt;

        // Gravity
        particles[i].velocityY += 180.0f * dt;
    }
}

void renderParticles(void)
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (!particles[i].active)
            continue;

        float lifePercent =
            particles[i].lifetime / particles[i].maxLifetime;

        float particleSize =
            particles[i].size * lifePercent;

        Color particleColor = particles[i].color;
        particleColor.a = (unsigned char)(255.0f * lifePercent);

        // ----------------------------------------
        // Outer glow
        // ----------------------------------------
        Color glow1 = particles[i].color;
        glow1.a = (unsigned char)(20.0f * lifePercent);

        float glowSize1 = particleSize * 5.0f;

        DrawRectangle(
            (int)(particles[i].x - glowSize1 / 2.0f),
            (int)(particles[i].y - glowSize1 / 2.0f),
            (int)glowSize1,
            (int)glowSize1,
            glow1
        );

        // ----------------------------------------
        // Middle glow
        // ----------------------------------------
        Color glow2 = particles[i].color;
        glow2.a = (unsigned char)(40.0f * lifePercent);

        float glowSize2 = particleSize * 3.5f;

        DrawRectangle(
            (int)(particles[i].x - glowSize2 / 2.0f),
            (int)(particles[i].y - glowSize2 / 2.0f),
            (int)glowSize2,
            (int)glowSize2,
            glow2
        );

        // ----------------------------------------
        // Inner glow
        // ----------------------------------------
        Color glow3 = particles[i].color;
        glow3.a = (unsigned char)(80.0f * lifePercent);

        float glowSize3 = particleSize * 2.0f;

        DrawRectangle(
            (int)(particles[i].x - glowSize3 / 2.0f),
            (int)(particles[i].y - glowSize3 / 2.0f),
            (int)glowSize3,
            (int)glowSize3,
            glow3
        );

        // ----------------------------------------
        // Main particle
        // ----------------------------------------
        DrawRectangle(
            (int)(particles[i].x - particleSize / 2.0f),
            (int)(particles[i].y - particleSize / 2.0f),
            (int)particleSize,
            (int)particleSize,
            particleColor
        );
    }
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