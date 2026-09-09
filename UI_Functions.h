#ifndef UI_FUNCS_H
#define UI_FUNCS_H

#include "raylib.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "time.h"
#include "float.h"
#include "math.h"
#include "Definitions.h"

float LerpFloat(float start, float end, float amount) {
    return start + (end - start) * amount;
}

float energyBarFillHeight(void)
{
    float barHeight = player1.stomach * ENERGY_BAR_SCALE;

    if (barHeight > ENERGY_BAR_MAX_HEIGHT)
        barHeight = ENERGY_BAR_MAX_HEIGHT;

    if (barHeight < 0.0f)
        barHeight = 0.0f;

    return barHeight;
}

void updateBars(int i)
{
    if (!bars[i].visible)
        return;

    // Wispy background texture behind the bar
    DrawTextureEx(
        energyTexture,
        (Vector2){
            bars[i].x - ENERGY_BAR_X, // texture was drawn at origin in old layout
            (float)bars[i].y
        },
        0.0f,
        0.2f,
        WHITE
    );

    Color borderColor;
    Color topColor;
    Color bottomColor;

    if (sprint_active)
    {
        borderColor = (Color){255, 150, 40, bars[i].ba};
        topColor = (Color){255, 220, 80, bars[i].ba};
        bottomColor = (Color){255, 70, 20, bars[i].ba};
    }
    else if (mult_active)
    {
        borderColor = (Color){multR, multG, multB, multA};
        topColor = (Color){multR, multG, multB, multA};
        bottomColor = (Color){
            (unsigned char)(multR * 0.75f),
            (unsigned char)(multG * 0.75f),
            (unsigned char)(multB * 0.75f),
            multA
        };
    }
    else
    {
        borderColor = (Color){bars[i].br, bars[i].bg, bars[i].bb, bars[i].ba};
        topColor = (Color){bars[i].topR, bars[i].topG, bars[i].topB, bars[i].topA};
        bottomColor = (Color){bars[i].btmR, bars[i].btmG, bars[i].btmB, bars[i].btmA};
    }

    // Border frame
    DrawRectangle(
        bars[i].x - 2,
        bars[i].y - 2,
        bars[i].w + 4,
        bars[i].h + 4,
        borderColor
    );

    // Empty slot
    DrawRectangle(
        bars[i].x,
        bars[i].y,
        bars[i].w,
        bars[i].h,
        (Color){0, 0, 0, 80}
    );

    float barHeight = energyBarFillHeight();

    if (barHeight > 0.0f)
    {
        DrawRectangleGradientV(
            bars[i].x,
            (int)(bars[i].y + bars[i].h - barHeight),
            bars[i].w,
            (int)barHeight,
            topColor,
            bottomColor
        );

        // Flickering heat glow over the fill while burning energy
        if (sprint_active)
        {
            float flicker = 0.2f + (float)GetRandomValue(0, 60) / 100.0f;
            DrawRectangle(
                bars[i].x,
                (int)(bars[i].y + bars[i].h - barHeight),
                bars[i].w,
                (int)barHeight,
                (Color){255, 160, 60, (unsigned char)(150.0f * flicker)}
            );
        }
    }
}

void updateButtons(int i, int mouseX, int mouseY) {
        if (!buttons[i].visible) {
            return;
        }

        Rectangle baseRec = {
            buttons[i].x,
            buttons[i].y,
            buttons[i].w,
            buttons[i].h
        };

        bool hovering = CheckCollisionPointRec(
            (Vector2){ mouseX, mouseY },
            baseRec
        );

        if (hovering) {
            buttons[i].targetScale = 1.10f;
        } else {
            buttons[i].targetScale = 1.0f;
        }
        
        buttons[i].scale = LerpFloat(
            buttons[i].scale,
            buttons[i].targetScale,
            10.0f * GetFrameTime()
        );

        float scaledW = buttons[i].w * buttons[i].scale;
        float scaledH = buttons[i].h * buttons[i].scale;

        float drawX =
            buttons[i].x + (buttons[i].w - scaledW) / 2.0f;

        float drawY =
            buttons[i].y + (buttons[i].h - scaledH) / 2.0f;

        Rectangle rec = {
            roundf(drawX),
            roundf(drawY),
            roundf(scaledW),
            roundf(scaledH)
        };


        DrawRectangleGradientV(
            (int)rec.x,
            (int)rec.y,
            (int)rec.width,
            (int)rec.height,
            (Color){
                buttons[i].topR,
                buttons[i].topG,
                buttons[i].topB,
                buttons[i].topA
            },
            (Color){
                buttons[i].btmR,
                buttons[i].btmG,
                buttons[i].btmB,
                buttons[i].btmA
            }
        );

        if (buttons[i].bThickness > 0) {
            DrawRectangleLinesEx(
                rec,
                buttons[i].bThickness,
                (Color){
                    buttons[i].br,
                    buttons[i].bg,
                    buttons[i].bb,
                    buttons[i].ba
                }
            );
        }

        int textWidth = MeasureText(
            buttons[i].text,
            buttons[i].f_size
        );

        DrawText(
            buttons[i].text,
            drawX + (scaledW - textWidth) / 2,
            drawY + (scaledH - buttons[i].f_size) / 2,
            buttons[i].f_size,
            (Color){
                buttons[i].tr,
                buttons[i].tg,
                buttons[i].tb,
                buttons[i].ta
            }
        );
}

void updateBoxes(int i) {
    if (boxes[i].visible == true) {
        DrawRectangle(boxes[i].x, boxes[i].y, boxes[i].w, boxes[i].h, (Color){boxes[i].r, boxes[i].g, boxes[i].b, boxes[i].a});
    }
}

void updateTextLabels(int i)
{
    if (!labels[i].visible)
        return;

    if (strcmp(labels[i].name, "score") == 0)
        snprintf(labels[i].text, sizeof(labels[i].text), "SCORE: %d", score);
    else if (strcmp(labels[i].name, "level") == 0)
        snprintf(labels[i].text, sizeof(labels[i].text), "LEVEL: %d", level);

    // Background
    DrawRectangleGradientV(
        labels[i].x - labels[i].w / 2,
        labels[i].y,
        labels[i].w,
        labels[i].h,
        (Color){
            labels[i].topR,
            labels[i].topG,
            labels[i].topB,
            labels[i].topA
        },
        (Color){
            labels[i].btmR,
            labels[i].btmG,
            labels[i].btmB,
            labels[i].btmA
        }
    );

    int textWidth = MeasureText(labels[i].text, labels[i].f_size);

    // Center text around labels[i].x
    int textX = labels[i].x - textWidth / 2;
    int textY = labels[i].y + (labels[i].h - labels[i].f_size) / 2;

    DrawText(
        labels[i].text,
        textX,
        textY,
        labels[i].f_size,
        (Color){
            labels[i].tr,
            labels[i].tg,
            labels[i].tb,
            labels[i].ta
        }
    );
}

void updateUI(void)
{
    int mouseX = GetMouseX();
    int mouseY = GetMouseY();

    for (int layer = 0; layer < MAX_UI; layer++)
    {
        for (int i = 0; i < MAX_UI; i++)
        {
            if (buttons[i].visible &&
                buttons[i].layer == layer)
            {
                updateButtons(i, mouseX, mouseY);
            }

            if (boxes[i].visible &&
                boxes[i].layer == layer)
            {
                updateBoxes(i);
            }

            if (labels[i].visible &&
                labels[i].layer == layer)
            {
                updateTextLabels(i);
            }

            if (bars[i].visible &&
                bars[i].layer == layer)
            {
                updateBars(i);
            }
        }
    }
}

void showUI(char *type) {
    for (int i = 0; i < MAX_UI; i++) {
        if (buttons[i].type != NULL && strcmp(buttons[i].type, type) == 0) {
            buttons[i].visible = true;
        }
        if (boxes[i].type != NULL && strcmp(boxes[i].type, type) == 0) {
            boxes[i].visible = true;
        }
    }
}

void hideUI(char *type) {
    for (int i = 0; i < MAX_UI; i++) {
        if (buttons[i].type != NULL && strcmp(buttons[i].type, type) == 0) {
            buttons[i].visible = false;
        }
        if (boxes[i].type != NULL && strcmp(boxes[i].type, type) == 0) {
            boxes[i].visible = false;
        }
    }
}

char *isHoveringOverBtn(int mouse_x, int mouse_y)
{
    for (int i = 0; i < MAX_UI; i++)
    {
        if (buttons[i].type != NULL && buttons[i].visible)
        {
            Rectangle rec = {
                buttons[i].x,
                buttons[i].y,
                buttons[i].w,
                buttons[i].h
            };

            if (CheckCollisionPointRec((Vector2){mouse_x, mouse_y},rec)) {
                return buttons[i].name;
            }
        }
    }

    return "";
}

int addEffectFrame(char *ability, Color fruitColor, Color accentColor, float duration, bool clickable)
{
    // One frame per ability — reusing refreshes the timer
    for (int i = 0; i < MAX_EFFECT_FRAMES; i++)
    {
        if (effectFrames[i].active && strcmp(effectFrames[i].ability, ability) == 0)
        {
            // Collectables stack; buffs just refresh their timer
            if (clickable)
                effectFrames[i].count++;

            effectFrames[i].timer = (clickable && duration <= 0.0f) ? 1.0f : duration;
            effectFrames[i].maxTimer = (effectFrames[i].timer > 0.0f) ? effectFrames[i].timer : 1.0f;
            effectFrames[i].scale = 1.8f;
            effectFrames[i].targetScale = 1.0f;
            return i;
        }
    }

    for (int i = 0; i < MAX_EFFECT_FRAMES; i++)
    {
        if (effectFrames[i].active)
            continue;

        effectFrames[i].active = true;
        effectFrames[i].clickable = clickable;
        effectFrames[i].hovering = false;
        effectFrames[i].ability = ability;
        effectFrames[i].timer = (clickable && duration <= 0.0f) ? 1.0f : duration;
        effectFrames[i].maxTimer = (effectFrames[i].timer > 0.0f) ? effectFrames[i].timer : 1.0f;
        effectFrames[i].count = 1;
        effectFrames[i].scale = 1.8f;
        effectFrames[i].targetScale = 1.0f;

        effectFrames[i].r = fruitColor.r;
        effectFrames[i].g = fruitColor.g;
        effectFrames[i].b = fruitColor.b;
        effectFrames[i].a = fruitColor.a;

        effectFrames[i].accentR = accentColor.r;
        effectFrames[i].accentG = accentColor.g;
        effectFrames[i].accentB = accentColor.b;
        effectFrames[i].accentA = accentColor.a;

        return i;
    }

    return -1;
}

void useEffectFrame(int i)
{
    if (i < 0 || i >= MAX_EFFECT_FRAMES)
        return;
    if (!effectFrames[i].active)
        return;
    if (!effectFrames[i].clickable)
        return;

    if (strcmp(effectFrames[i].ability, "Heal") == 0)
    {
        triggerHealAbility();
    }
    else if (strcmp(effectFrames[i].ability, "Ward") == 0)
    {
        triggerWardAbility();
    }

    effectFrames[i].count--;
    effectFrames[i].scale = 1.8f;
    effectFrames[i].targetScale = 1.0f;

    if (effectFrames[i].count <= 0)
        effectFrames[i].active = false;
}

void updateEffectFrames(void)
{
    float dt = GetFrameTime();
    int mouseX = GetMouseX();
    int mouseY = GetMouseY();

    // Auto-consume a held heal apple the moment the snake is poisoned
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

    // Activate collectables with number keys (1-8, matching the tray order)
    for (int i = 0; i < MAX_EFFECT_FRAMES; i++)
    {
        if (IsKeyPressed(KEY_ONE + i))
            useEffectFrame(i);
    }

    for (int i = 0; i < MAX_EFFECT_FRAMES; i++)
    {
        if (!effectFrames[i].active)
            continue;

        float x = EFFECT_FRAME_START_X + i * (EFFECT_FRAME_SIZE + EFFECT_FRAME_GAP);
        float y = EFFECT_FRAME_START_Y;

        // Buff frames mirror the authoritative effect timers so they never
        // desync; clickable collectables persist until clicked.
        if (strcmp(effectFrames[i].ability, "Mult") == 0)
        {
            effectFrames[i].timer = mult_active ? multTimer : 0.0f;
        }
        else if (strcmp(effectFrames[i].ability, "Magnet") == 0)
        {
            effectFrames[i].timer = magnet_active ? magnetTimer : 0.0f;
        }
        else if (strcmp(effectFrames[i].ability, "Giant") == 0)
        {
            effectFrames[i].timer = giant_active ? giantTimer : 0.0f;
        }
        else if (!effectFrames[i].clickable)
        {
            effectFrames[i].timer -= dt;
        }

        if (effectFrames[i].timer <= 0.0f && !effectFrames[i].clickable)
        {
            effectFrames[i].timer = 0.0f;
            effectFrames[i].active = false;
            continue;
        }

        effectFrames[i].scale = LerpFloat(
            effectFrames[i].scale,
            effectFrames[i].targetScale,
            10.0f * dt
        );

        effectFrames[i].hovering =
            CheckCollisionPointRec(
                (Vector2){ mouseX, mouseY },
                (Rectangle){ x, y, EFFECT_FRAME_SIZE, EFFECT_FRAME_SIZE }
            );

        if (effectFrames[i].clickable &&
            effectFrames[i].hovering &&
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            useEffectFrame(i);
        }
    }
}

#define MAX_EFFECT_TOOLTIP_LINES 5

void drawEffectTooltip(int frameIndex);

void renderEffectFrames(void)
{
    int hoveredFrame = -1;

    for (int i = 0; i < MAX_EFFECT_FRAMES; i++)
    {
        if (!effectFrames[i].active)
            continue;

        float x = EFFECT_FRAME_START_X + i * (EFFECT_FRAME_SIZE + EFFECT_FRAME_GAP);
        float y = EFFECT_FRAME_START_Y;

        float scale = effectFrames[i].scale;
        float w = EFFECT_FRAME_SIZE * scale;
        float h = EFFECT_FRAME_SIZE * scale;
        float dx = x + (EFFECT_FRAME_SIZE - w) / 2.0f;
        float dy = y + (EFFECT_FRAME_SIZE - h) / 2.0f;

        Color accent = {
            (unsigned char)effectFrames[i].accentR,
            (unsigned char)effectFrames[i].accentG,
            (unsigned char)effectFrames[i].accentB,
            (unsigned char)effectFrames[i].accentA
        };

        if (effectFrames[i].clickable && effectFrames[i].hovering)
            accent = (Color){255, 255, 255, 255};

        // Frame background (pure blue)
        DrawRectangle(
            (int)dx,
            (int)dy,
            (int)w,
            (int)h,
            (Color){0, 0, 255, 235}
        );

        // Frame border
        DrawRectangleLinesEx(
            (Rectangle){ dx, dy, w, h },
            2.0f,
            accent
        );

        // Fruit icon
        float fruitPad = 4.0f;
        float fruitSize = w - fruitPad * 2.0f;

        Color fruitColor = {
            (unsigned char)effectFrames[i].r,
            (unsigned char)effectFrames[i].g,
            (unsigned char)effectFrames[i].b,
            (unsigned char)effectFrames[i].a
        };

        DrawRectangle(
            (int)(dx + fruitPad),
            (int)(dy + fruitPad),
            (int)fruitSize,
            (int)fruitSize,
            fruitColor
        );

        // Stack count badge (collectables)
        if (effectFrames[i].clickable && effectFrames[i].count > 1)
        {
            char stackText[16];
            snprintf(stackText, sizeof(stackText), "x%d", effectFrames[i].count);

            DrawText(
                stackText,
                (int)(dx + w - MeasureText(stackText, 9)) - 1,
                (int)(dy + h - 9) - 1,
                9,
                (Color){0, 0, 0, 255}
            );

            DrawText(
                stackText,
                (int)(dx + w - MeasureText(stackText, 9)),
                (int)(dy + h - 9),
                9,
                (Color){255, 255, 255, 255}
            );
        }

        // Activation key number (collectables)
        if (effectFrames[i].clickable)
        {
            char keyText[8];
            snprintf(keyText, sizeof(keyText), "%d", i + 1);

            DrawText(keyText, (int)dx + 1, (int)dy + 1, 9, (Color){0, 0, 0, 255});
            DrawText(keyText, (int)dx + 2, (int)dy + 2, 9, (Color){255, 255, 255, 255});
        }

        // Countdown bar along the bottom of the frame (buffs only —
        // clickable collectables persist until used)
        if (!effectFrames[i].clickable)
        {
            float ratio = effectFrames[i].timer / effectFrames[i].maxTimer;
            if (ratio > 1.0f)
                ratio = 1.0f;

            DrawRectangle(
                (int)dx,
                (int)(dy + h - 2.0f),
                (int)(w * ratio),
                2,
                accent
            );

            // Seconds remaining, centered over the icon — bold white with a strong
            // outline for readability
            char timerText[16];
            snprintf(timerText, sizeof(timerText), "%d", (int)ceilf(effectFrames[i].timer));

            int fontSize = 11;
            int textWidth = MeasureText(timerText, fontSize);

            int textX = (int)(dx + w / 2.0f - textWidth / 2.0f);
            int textY = (int)(dy + (h - fontSize) / 2.0f);

            DrawText(timerText, textX - 1, textY, fontSize, (Color){0, 0, 0, 255});
            DrawText(timerText, textX + 1, textY, fontSize, (Color){0, 0, 0, 255});
            DrawText(timerText, textX, textY - 1, fontSize, (Color){0, 0, 0, 255});
            DrawText(timerText, textX, textY + 1, fontSize, (Color){0, 0, 0, 255});
            DrawText(timerText, textX, textY, fontSize, (Color){255, 255, 255, 255});
        }

        if (effectFrames[i].hovering)
            hoveredFrame = i;
    }

    if (hoveredFrame >= 0)
        drawEffectTooltip(hoveredFrame);
}

void drawEffectTooltip(int frameIndex)
{
    struct EffectFrame *f = &effectFrames[frameIndex];

    int arralen = sizeof(powerUps) / sizeof(powerUps[0]);
    const char *name = f->ability;
    const char *desc = f->ability;

    for (int j = 0; j < arralen; j++)
    {
        if (strcmp(powerUps[j].ability, f->ability) == 0)
        {
            name = powerUps[j].name;
            desc = powerUps[j].description;
            break;
        }
    }

    int titleSize = 13;
    int bodySize = 11;
    int lineSpacing = 14;
    int maxWrap = 300;

    // Word-wrap the description into lines
    char wrapped[MAX_EFFECT_TOOLTIP_LINES][512];
    int lineCount = 0;
    char buffer[1024];
    strncpy(buffer, desc, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    char currentLine[512] = "";
    char *token = strtok(buffer, " ");

    while (token != NULL && lineCount < MAX_EFFECT_TOOLTIP_LINES)
    {
        char trial[512];

        snprintf(trial, sizeof(trial), "%s", currentLine);

        if (trial[0] != '\0')
            strncat(trial, " ", sizeof(trial) - strlen(trial) - 1);

        strncat(trial, token, sizeof(trial) - strlen(trial) - 1);

        if (MeasureText(trial, bodySize) > maxWrap)
        {
            snprintf(wrapped[lineCount++], sizeof(wrapped[lineCount]), "%s", currentLine);
            snprintf(currentLine, sizeof(currentLine), "%s", token);
        }
        else
        {
            snprintf(currentLine, sizeof(currentLine), "%s", trial);
        }

        token = strtok(NULL, " ");
    }

    if (lineCount < MAX_EFFECT_TOOLTIP_LINES && currentLine[0] != '\0')
        snprintf(wrapped[lineCount++], sizeof(wrapped[lineCount]), "%s", currentLine);

    // Remaining / usage line
    char statusLine[128];

    if (f->clickable)
        snprintf(statusLine, sizeof(statusLine), "Click to use — held: %d", f->count);
    else
        snprintf(statusLine, sizeof(statusLine), "Active for %ds", (int)ceilf(f->timer));

    // Measure the box
    int titleWidth = MeasureText(name, titleSize);
    int boxWidth = (titleWidth > maxWrap) ? titleWidth : maxWrap;
    boxWidth += 14;

    int boxHeight = 10 + titleSize + 6 + lineCount * lineSpacing + 16;
    if (boxHeight < 44)
        boxHeight = 44;

    // Anchor below the hovered frame, right-aligned to it (tray sits top-right)
    float frameX = EFFECT_FRAME_START_X + frameIndex * (EFFECT_FRAME_SIZE + EFFECT_FRAME_GAP);
    float frameY = EFFECT_FRAME_START_Y;

    float boxX = (frameX + EFFECT_FRAME_SIZE) - boxWidth;
    float boxY = frameY + EFFECT_FRAME_SIZE + 3.0f;

    if (boxX < 2.0f)
        boxX = 2.0f;

    // Box background + border (pure 0,0,255 blue)
    DrawRectangle(
        (int)boxX,
        (int)boxY,
        boxWidth,
        boxHeight,
        (Color){0, 0, 220, 240}
    );

    DrawRectangleLinesEx(
        (Rectangle){ boxX, boxY, (float)boxWidth, (float)boxHeight },
        1.5f,
        (Color){0, 0, 255, 255}
    );

    int textX = (int)boxX + 7;
    int textY = (int)boxY + 5;

    // Crisp neon cyan text
    Color neon = (Color){0, 255, 255, 255};

    DrawText(name, textX, textY, titleSize, neon);
    textY += titleSize + 5;

    for (int l = 0; l < lineCount; l++)
    {
        DrawText(wrapped[l], textX, textY, bodySize, neon);
        textY += lineSpacing;
    }

    DrawText(statusLine, textX, textY, bodySize, neon);
}

void triggerWardAbility(void)
{
    // Golden glow raises a shield around the snake (stacks its remaining time)
    wardTimer += WARD_SHIELD_DURATION;
    ward_active = true;

    // The player flies to the center of the grid
    if (!warp_active)
    {
        warp_active = true;
        warpStartX = (float)player1.x;
        warpStartY = (float)player1.y;

        int cx = ((SCREEN_WIDTH - PLAYER_SIZE) / 2) / PLAYER_SIZE * PLAYER_SIZE;
        int cy = ((SCREEN_HEIGHT - PLAYER_SIZE) / 2) / PLAYER_SIZE * PLAYER_SIZE;

        warpTargetX = (float)cx;
        warpTargetY = (float)cy;
        warpTimer = 0.0f;
        clearInputQueue();
    }

    float cx = player1.x + player1.size / 2.0f;
    float cy = player1.y + player1.size / 2.0f;

    for (int s = 0; s < 60; s++)
    {
        spawnGreedSparkle(
            cx,
            cy,
            (Color){255, 225, 110, 255}
        );
    }

    PlaySound(levelUpSound);
}

bool wardSave(void)
{
    // Already shielded — this hit is absorbed
    if (ward_active)
        return true;

    // Otherwise burn one holy banana from the inventory
    for (int i = 0; i < MAX_EFFECT_FRAMES; i++)
    {
        if (effectFrames[i].active &&
            effectFrames[i].clickable &&
            effectFrames[i].count >= 1 &&
            strcmp(effectFrames[i].ability, "Ward") == 0)
        {
            useEffectFrame(i);
            return true;
        }
    }

    return false;
}

void clearEffectFrames(void)
{
    for (int i = 0; i < MAX_EFFECT_FRAMES; i++)
        effectFrames[i].active = false;
}

Color energyToColor(float amount)
{
    // Gradient across the energy range (lowest -> highest):
    // blue, cyan, green, yellow, orange, red, pink, purple, white
    static const float stops[9] = {
        0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f
    };
    static const Color stopsColor[9] = {
        { 40, 90, 255, 255 },    // blue
        { 0, 220, 255, 255 },    // cyan
        { 60, 255, 120, 255 },   // green
        { 255, 235, 80, 255 },   // yellow
        { 255, 150, 40, 255 },   // orange
        { 255, 60, 40, 255 },    // red
        { 255, 120, 200, 255 },  // pink
        { 180, 60, 255, 255 },   // purple
        { 255, 255, 255, 255 }   // white
    };

    float t = amount;
    Color out = stopsColor[8];

    if (t <= stops[0])
    {
        out = stopsColor[0];
    }
    else
    {
        for (int i = 0; i < 8; i++)
        {
            if (t >= stops[i] && t <= stops[i + 1])
            {
                float f = (t - stops[i]) / (stops[i + 1] - stops[i]);
                out.r = (unsigned char)(stopsColor[i].r + (stopsColor[i + 1].r - stopsColor[i].r) * f);
                out.g = (unsigned char)(stopsColor[i].g + (stopsColor[i + 1].g - stopsColor[i].g) * f);
                out.b = (unsigned char)(stopsColor[i].b + (stopsColor[i + 1].b - stopsColor[i].b) * f);
                break;
            }
        }
    }

    return out;
}

void spawnEnergyPopup(float energy)
{
    if (energy <= 0.0f)
        return;

    for (int i = 0; i < MAX_UI; i++)
    {
        if (!tempLabels[i].visible)
        {
            // Actual energy added to stomach
            float actualEnergy = energy * (float)food_mult;

            tempLabels[i].visible = true;

            tempLabels[i].lifetime = 0.0f;
            tempLabels[i].maxLifetime = 1.0f;
            // Give it a unique name
            tempLabels[i].name = "energy_popup";

            // Text
            snprintf(tempLabels[i].text, sizeof(tempLabels[i].text), "+%.1f", actualEnergy);

            // Remove .0 from whole numbers
            if (fabsf(actualEnergy - roundf(actualEnergy)) < 0.01f)
            {
                    snprintf(
                        tempLabels[i].text,
                        sizeof(tempLabels[i].text),
                        "+%.0f",
                        actualEnergy
                    );
            }

            // Start at player's head
            tempLabels[i].visualX =
                player1.x + player1.size / 2.0f;

            tempLabels[i].visualY =
                player1.y;

            tempLabels[i].x = (int)tempLabels[i].visualX;
            tempLabels[i].y = (int)tempLabels[i].visualY;

            // Text size — bigger for bigger energy hits
            float sizeScale = 18.0f + actualEnergy * 4.0f;
            if (sizeScale > 42.0f)
                sizeScale = 42.0f;

            tempLabels[i].f_size = (int)sizeScale;

            // Starting scale
            tempLabels[i].scale = 2.5f;
            tempLabels[i].targetScale = 1.0f;

            // Text color — ramps by how much energy was consumed
            Color popupColor = energyToColor(actualEnergy);
            tempLabels[i].tr = popupColor.r;
            tempLabels[i].tg = popupColor.g;
            tempLabels[i].tb = popupColor.b;
            tempLabels[i].ta = 255;

            // No background
            tempLabels[i].topA = 0;
            tempLabels[i].btmA = 0;

            // No border
            tempLabels[i].bThickness = 0;

            // Put it on a high UI layer
            tempLabels[i].layer = 100;

            // Used by the animation
            tempLabels[i].br = 0;
            tempLabels[i].bg = 0;
            tempLabels[i].bb = 0;
            tempLabels[i].ba = 0;

            return;
        }
    }
}

void updateEnergyPopups(void)
{
    float dt = GetFrameTime();

    float barX = ENERGY_BAR_X;
    float barY = ENERGY_BAR_Y;
    float barWidth = ENERGY_BAR_WIDTH;
    float maxHeight = ENERGY_BAR_MAX_HEIGHT;

    float targetX = barX + barWidth / 2.0f;
    float targetY = barY + maxHeight / 2.0f;

    for (int i = 0; i < MAX_UI; i++)
    {
        if (!tempLabels[i].visible)
            continue;

        tempLabels[i].lifetime += dt;

       // if (tempLabels[i].lifetime >= tempLabels[i].maxLifetime)
      //  {
       //     tempLabels[i].visible = false;
      //      continue;
      //  }

        float dx = targetX - tempLabels[i].visualX;
        float dy = targetY - tempLabels[i].visualY;

        float distance = sqrtf(dx * dx + dy * dy);

        if (distance < 5.0f)
        {
            tempLabels[i].visible = false;
            continue;
        }
        if (distance > 0.001f)
        {
            dx /= distance;
            dy /= distance;
        }

        
        float speed = 300.0f;

        tempLabels[i].visualX += dx * speed * dt;
        tempLabels[i].visualY += dy * speed * dt;

        tempLabels[i].x = (int)tempLabels[i].visualX;
        tempLabels[i].y = (int)tempLabels[i].visualY;

        tempLabels[i].scale = LerpFloat(
            tempLabels[i].scale,
            0.3f,
            8.0f * dt
        );
    }
}

void renderTempLabels(void)
{
    for (int i = 0; i < MAX_UI; i++)
    {
        if (!tempLabels[i].visible)
            continue;

        // Only render energy popups
        if (strcmp(tempLabels[i].name, "energy_popup") != 0)
            continue;

        char *text = tempLabels[i].text;

        // Apply animated scale
        int fontSize = (int)(
            tempLabels[i].f_size *
            tempLabels[i].scale
        );

        if (fontSize < 1)
            fontSize = 1;

        int textWidth =
            MeasureText(text, fontSize);

        int textX =
            (int)(tempLabels[i].visualX -
                  textWidth / 2.0f);

        int textY =
            (int)(tempLabels[i].visualY -
                  fontSize / 2.0f);

        float progress =
            tempLabels[i].lifetime /
            tempLabels[i].maxLifetime;

        // Fade out during last 35%
        float fade = 1.0f;

        if (progress > 0.65f)
        {
            fade =
                1.0f -
                (progress - 0.65f) / 0.35f;
        }

        if (fade < 0.0f)
            fade = 0.0f;

        // --------------------------------
        // Outer glow — soft halo outside the glyphs so the text stays sharp
        // --------------------------------

        float glowAlphas[4] = { 38.0f, 26.0f, 15.0f, 7.0f };
        int glowOffsets[4] = { 4, 7, 11, 15 };

        for (int pass = 0; pass < 4; pass++)
        {
            int off = glowOffsets[pass];

            float passAlpha = glowAlphas[pass] * fade;
            if (passAlpha < 0.0f)
                passAlpha = 0.0f;

            Color glowPass = {
                tempLabels[i].tr,
                tempLabels[i].tg,
                tempLabels[i].tb,
                (unsigned char)passAlpha
            };

            DrawText(text, textX - off, textY, fontSize, glowPass);
            DrawText(text, textX + off, textY, fontSize, glowPass);
            DrawText(text, textX, textY - off, fontSize, glowPass);
            DrawText(text, textX, textY + off, fontSize, glowPass);
            DrawText(text, textX - off, textY - off, fontSize, glowPass);
            DrawText(text, textX + off, textY + off, fontSize, glowPass);
            DrawText(text, textX + off, textY - off, fontSize, glowPass);
            DrawText(text, textX - off, textY + off, fontSize, glowPass);
        }

        // --------------------------------
        // Main text
        // --------------------------------

        Color textColor = {
            tempLabels[i].tr,
            tempLabels[i].tg,
            tempLabels[i].tb,
            (unsigned char)(255.0f * fade)
        };

        DrawText(
            text,
            textX,
            textY,
            fontSize,
            textColor
        );
    }
}

#endif