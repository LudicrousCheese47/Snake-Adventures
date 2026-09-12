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

        // Near-full charge: the next segment is seconds away, so the whole
        // fill breathes a warm gold invitation to keep eating.
        if (isNearGrowthCap())
        {
            float pulse = 0.35f + 0.35f * sinf(GetTime() * 18.0f);
            DrawRectangle(
                bars[i].x,
                (int)(bars[i].y + bars[i].h - barHeight),
                bars[i].w,
                (int)barHeight,
                (Color){255, 215, 90, (unsigned char)(150.0f * pulse)}
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
    else if (strcmp(labels[i].name, "fruits") == 0)
    {
        // Plain fruits are the only ones that advance the level, so show
        // exactly those that are still on the board.
        int remaining = 0;
        for (int f = 0; f < FRUIT_COUNT; f++)
        {
            if (foods[f].active && strcmp(foods[f].ability, "Fruit") == 0)
                remaining++;
        }
        snprintf(labels[i].text, sizeof(labels[i].text), "%d/%d", remaining, fruitsTotalThisLevel);
    }
    else if (strcmp(labels[i].name, "bestCombo") == 0)
    {
        snprintf(labels[i].text, sizeof(labels[i].text), "BEST x%d", bestStreak);

        // New-best flash: the chip swells and flares into hot white-gold for
        // a beat so a record streak is impossible to miss.
        if (bestFlashTimer > 0.0f)
        {
            labels[i].f_size = 13;
            labels[i].tr = 255;
            labels[i].tg = 250;
            labels[i].tb = 200;
            labels[i].btmA = 190;
            labels[i].btmR = 160;
            labels[i].btmG = 70;
            labels[i].btmB = 0;
        }
        else
        {
            labels[i].f_size = 10;
            labels[i].tr = 255;
            labels[i].tg = 220;
            labels[i].tb = 80;
            labels[i].btmA = 0;
            labels[i].btmR = 0;
            labels[i].btmG = 0;
            labels[i].btmB = 0;
        }
    }

    // Stats labels auto-grid: laid out left to right against a running cursor,
    // each positioned by its own measured text width, so they never overlap
    // no matter how the numbers grow or shrink.
    if (strcmp(labels[i].type, "stats") == 0)
    {
        int textWidth = MeasureText(labels[i].text, labels[i].f_size);
        labels[i].w = textWidth + 8;
        labels[i].x = statsLabelX + textWidth / 2;
        statsLabelX += textWidth + STATS_LABEL_GAP;
    }

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

    // Start the stats label grid from the left border each frame.
    statsLabelX = UI_BORDER_OFFSET + 8;

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
            // Collectables stack per type; buffs just refresh their timer.
            // A full stack of one type never locks out a different ability.
            if (clickable)
            {
                if (effectFrames[i].count >= MAX_EFFECT_STACK)
                    return -1;
                effectFrames[i].count++;
            }

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
    else if (strcmp(effectFrames[i].ability, "Time") == 0)
    {
        triggerTimeAbility();
    }
    else if (strcmp(effectFrames[i].ability, "Grow") == 0)
    {
        triggerGrowAbility();
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

    // Note: the auto-consumption of a held heal while poisoned lives in
    // whilePoisoned() so it lands BEFORE the damage tick each frame.

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
        else if (strcmp(effectFrames[i].ability, "Speed") == 0)
        {
            effectFrames[i].timer = speed_active ? speedTimer : 0.0f;
        }
        else if (strcmp(effectFrames[i].ability, "Endurance") == 0)
        {
            effectFrames[i].timer = endurance_active ? enduranceTimer : 0.0f;
        }
        else if (strcmp(effectFrames[i].ability, "Lucky") == 0)
        {
            effectFrames[i].timer = lucky_active ? luckyTimer : 0.0f;
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

    // The player flies to the center of the grid, coiled up for the ride
    if (!warp_active)
    {
        scared = false;   // don't strand a panicking snake in the middle of the coil

        // Coil the whole snake into a spiral around its current tile first, so
        // the body flies to the centre as one compact shape instead of dragging
        // a sprawling tail that clips outside the map.
        const int minTile = UI_BORDER_OFFSET / PLAYER_SIZE;
        const int maxTile = (SCREEN_WIDTH / PLAYER_SIZE) - minTile - 1;

        int coX[MAX_P_LENGTH];
        int coY[MAX_P_LENGTH];

        // Coil the body into stacked folds sitting strictly BEHIND the head,
        // with the neck right behind it. The head starts at the coil's exit,
        // so its forward and side tiles are always clear and it never warps
        // into its own body.
        int foldW = (player1.length > 2)
            ? (int)floorf(sqrtf((float)(player1.length - 1)))
            : 2;
        if (foldW < 2) foldW = 2;

        int fx = -1, fy = 0;        // first fold starts right behind the head
        int fdx = -1;               // sweeps left first
        int placed = 0;
        while (placed < player1.length)
        {
            for (int i = 0; i < foldW && placed < player1.length;
                 i++, fx += fdx, placed++)
            {
                coX[placed] = fx;
                coY[placed] = fy;
            }
            fx -= fdx;              // land the stitch on the fold's far end
            fy -= 1;                // step the coil back another layer
            fdx = -fdx;
        }

        // Spin the folded coil so it always sits behind the direction of travel
        for (int i = 0; i < player1.length; i++)
        {
            int ix = coX[i], iy = coY[i];
            if (strcmp(direction, "left") == 0)   { coX[i] = -ix; coY[i] =  iy; }
            else if (strcmp(direction, "up") == 0)   { coX[i] =  iy; coY[i] = -ix; }
            else if (strcmp(direction, "down") == 0) { coX[i] = -iy; coY[i] =  ix; }
            // right / any other direction keeps the identity orientation
        }

        // Keep the coiled outline inside the bordered field
        int minOx = coX[0], maxOx = coX[0];
        int minOy = coY[0], maxOy = coY[0];
        for (int i = 1; i < player1.length; i++)
        {
            if (coX[i] < minOx) minOx = coX[i];
            if (coX[i] > maxOx) maxOx = coX[i];
            if (coY[i] < minOy) minOy = coY[i];
            if (coY[i] > maxOy) maxOy = coY[i];
        }

        int hx = player1.x / PLAYER_SIZE;
        int hy = player1.y / PLAYER_SIZE;
        if (hx + minOx < minTile) hx = minTile - minOx;
        if (hx + maxOx > maxTile) hx = maxTile - maxOx;
        if (hy + minOy < minTile) hy = minTile - minOy;
        if (hy + maxOy > maxTile) hy = maxTile - maxOy;

        player1.x = hx * PLAYER_SIZE;
        player1.y = hy * PLAYER_SIZE;
        player1.visualX = (float)player1.x;
        player1.visualY = (float)player1.y;

        for (int i = 0; i < player1.length; i++)
        {
            bodies[i].x = (hx + coX[i]) * PLAYER_SIZE;
            bodies[i].y = (hy + coY[i]) * PLAYER_SIZE;
            bodies[i].visualX = (float)bodies[i].x;
            bodies[i].visualY = (float)bodies[i].y;
        }

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

void triggerTimeAbility(void)
{
    // Rewind the level: fly the snake back to where the level began. While
    // travelling it glows cyan-green; landing restores the original board
    // (updateWarp does the restore) and snaps every segment back on-screen.
    warpStartX = (float)player1.x;
    warpStartY = (float)player1.y;
    warpTargetX = (float)timeStartX;
    warpTargetY = (float)timeStartY;
    warp_active = true;
    warpTimer = 0.0f;
    timeTravel_active = true;
    timeTravelTimer = WARP_DURATION;

    // Rewind whoosh: start high, sweep down low, fade out.
    SetSoundPitch(timeReverseSound, 1.8f);
    SetSoundVolume(timeReverseSound, 1.0f);
    PlaySound(timeReverseSound);
    timeReversePlayTimer = 0.0f;
    timeReverseActive = true;

    float cx = player1.x + player1.size / 2.0f;
    float cy = player1.y + player1.size / 2.0f;

    for (int s = 0; s < 60; s++)
    {
        spawnGreedSparkle(
            cx,
            cy,
            (Color){0, 255, 160, 255}
        );
    }
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
    // Sizzling, fully-saturated gradient across the energy range:
    // blue, cyan, green, yellow, orange, red, magenta, violet, white
    static const float stops[9] = {
        0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f
    };
    static const Color stopsColor[9] = {
        { 0, 0, 255, 255 },      // electric blue
        { 0, 255, 255, 255 },    // cyan
        { 0, 255, 0, 255 },      // pure green
        { 255, 255, 0, 255 },    // yellow
        { 255, 140, 0, 255 },    // blazing orange
        { 255, 0, 0, 255 },      // pure red
        { 255, 0, 255, 255 },    // magenta
        { 170, 0, 255, 255 },    // violet
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

void spawnEnergyPopup(float energy, bool isAbility)
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

            // Ability fruits get flagged so the renderer draws a badge
            tempLabels[i].type = isAbility ? "ability" : NULL;

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

            // Text size — snappy and compact, scales modestly with energy
            float sizeScale = 12.0f + actualEnergy * 2.2f;
            if (sizeScale > 26.0f)
                sizeScale = 26.0f;

            tempLabels[i].f_size = (int)sizeScale;

            // Starting scale — big pop at spawn
            tempLabels[i].scale = 1.35f;
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

            // Ability fruit pickups EXPLODE with a radial burst — that blast
            // is the "ability fruit" indicator now.
            if (isAbility)
            {
                spawnFruitParticles(
                    (float)tempLabels[i].x,
                    (float)tempLabels[i].y,
                    popupColor
                );
            }

            // Dopamine burst — sparkles burst out of the pickup; bigger hits
            // throw off more confetti. Ability fruit goes harder.
            int sparkCount = (isAbility ? 16 : 10) + (int)(actualEnergy * 6.0f);
            if (sparkCount > 48)
                sparkCount = 48;
            for (int s = 0; s < sparkCount; s++)
            {
                spawnGreedSparkle(
                    (float)tempLabels[i].x,
                    (float)tempLabels[i].y,
                    popupColor
                );
            }

            return;
        }
    }
}

// HSV to RGB — used to roll banners a guaranteed-vibrant random hue.
Color hsvToColor(float h, float s, float v)
{
    float c = v * s;
    float hp = fmodf(h / 60.0f, 6.0f);
    float x = c * (1.0f - fabsf(fmodf(hp, 2.0f) - 1.0f));
    float m = v - c;

    float r, g, b;
    if (hp < 1.0f) { r = c; g = x; b = 0.0f; }
    else if (hp < 2.0f) { r = x; g = c; b = 0.0f; }
    else if (hp < 3.0f) { r = 0.0f; g = c; b = x; }
    else if (hp < 4.0f) { r = 0.0f; g = x; b = c; }
    else if (hp < 5.0f) { r = x; g = 0.0f; b = c; }
    else { r = c; g = 0.0f; b = x; }

    return (Color){
        (unsigned char)((r + m) * 255.0f),
        (unsigned char)((g + m) * 255.0f),
        (unsigned char)((b + m) * 255.0f),
        255
    };
}

// Punch a color further from the neutral grey centre so banners read louder
// against the board — spreads the channels around their average.
Color colorSaturate(Color c, float boost)
{
    float avg =
        ((float)c.r + (float)c.g + (float)c.b) / 255.0f / 3.0f;

    float r = ((float)c.r / 255.0f - avg) * boost + avg;
    float g = ((float)c.g / 255.0f - avg) * boost + avg;
    float b = ((float)c.b / 255.0f - avg) * boost + avg;

    if (r < 0.0f) r = 0.0f;
    if (g < 0.0f) g = 0.0f;
    if (b < 0.0f) b = 0.0f;
    if (r > 1.0f) r = 1.0f;
    if (g > 1.0f) g = 1.0f;
    if (b > 1.0f) b = 1.0f;

    return (Color){
        (unsigned char)(r * 255.0f),
        (unsigned char)(g * 255.0f),
        (unsigned char)(b * 255.0f),
        c.a
    };
}

// Banners render on their own dedicated overlay layer so they never collide
// with energy popups or with each other. Rows are stacked based on how many
// live banners already occupy the same horizontal band — a fresh message drops
// into a free slot below instead of pasting on top of an outgoing one.
void spawnBannerAt(const char *text, Color color, float lifetime, float y, int fsize)
{
    // Every banner rolls a fresh vibrant hue — full saturation, near-peak
    // brightness — so no two toasts ever read the same. The passed color is
    // dropped on purpose; randomness IS the flavor here.
    (void)color;
    float hue = (float)GetRandomValue(0, 360);
    float sat = 0.85f + (float)GetRandomValue(0, 15) / 100.0f;
    float val = 0.95f + (float)GetRandomValue(0, 5) / 100.0f;
    Color vivid = hsvToColor(hue, sat, val);

    // Grab a free row; if every row is busy, recycle the oldest so a cascade
    // of toasts can't silently drop.
    int row = -1;
    for (int i = 0; i < MAX_BANNERS; i++)
    {
        if (!banners[i].visible)
        {
            row = i;
            break;
        }
    }
    if (row < 0)
    {
        int oldest = 0;
        for (int i = 1; i < MAX_BANNERS; i++)
        {
            if (banners[i].lifetime > banners[oldest].lifetime)
                oldest = i;
        }
        banners[oldest].visible = false;
        row = oldest;
    }

    // Push this row below any live banner resting in the same band so the
    // stack reads top-to-bottom instead of gluing messages together.
    int liveBelow = 0;
    for (int i = 0; i < MAX_BANNERS; i++)
    {
        if (i != row && banners[i].visible && fabsf(banners[i].y - y) < 70.0f)
            liveBelow++;
    }

    banners[row].visible = true;
    banners[row].name = "banner";
    snprintf(banners[row].text, sizeof(banners[row].text), "%s", text);
    banners[row].r = vivid.r;
    banners[row].g = vivid.g;
    banners[row].b = vivid.b;
    banners[row].a = 255;
    banners[row].f_size = fsize;
    banners[row].layer = 100 - row;
    banners[row].y = y + (float)liveBelow * BANNER_GAP;
    banners[row].scale = 1.0f;
    banners[row].lifetime = 0.0f;
    banners[row].maxLifetime = lifetime;
}

void spawnTextBanner(const char *text, Color color, float lifetime)
{
    spawnBannerAt(
        text,
        color,
        lifetime,
        player1.y,
        16
    );
}

void spawnCenterBanner(const char *text, Color color, float lifetime)
{
    spawnBannerAt(
        text,
        color,
        lifetime,
        (float)SCREEN_HEIGHT / 2.0f,
        34
    );
}

void updateBanners(void)
{
    float dt = GetFrameTime();

    for (int i = 0; i < MAX_BANNERS; i++)
    {
        if (!banners[i].visible)
            continue;

        banners[i].lifetime += dt;
        banners[i].y -= 30.0f * dt;

        if (banners[i].lifetime >= banners[i].maxLifetime)
            banners[i].visible = false;
    }
}

void renderBanners(void)
{
    for (int i = 0; i < MAX_BANNERS; i++)
    {
        if (!banners[i].visible)
            continue;

        float progress = banners[i].lifetime /
                         banners[i].maxLifetime;

        // Fade out during the last 35%
        float fade = 1.0f;
        if (progress > 0.65f)
            fade = 1.0f - (progress - 0.65f) / 0.35f;
        if (fade < 0.0f)
            fade = 0.0f;

        // Elastic pop-in — starts oversized, settles fast, then breathes.
        float pop = 1.0f - banners[i].lifetime / 0.22f;
        if (pop < 0.0f)
            pop = 0.0f;
        if (pop > 1.0f)
            pop = 1.0f;
        float heartbeat = 1.0f + 0.06f * fabsf(sinf(GetTime() * 14.0f + (float)i));
        banners[i].scale = 1.0f + pop * 0.9f;
        banners[i].scale *= heartbeat;

        int fontSize = (int)(banners[i].f_size * banners[i].scale);
        if (fontSize < 1)
            fontSize = 1;

        const char *text = banners[i].text;
        int textWidth = MeasureText(text, fontSize);

        float textX = (float)SCREEN_WIDTH / 2.0f - textWidth / 2.0f;
        float textY = banners[i].y - fontSize / 2.0f;
        float cx = textX + textWidth / 2.0f;
        float cy = textY + fontSize / 2.0f;

        // White-hot flash as it ignites
        float flash = 1.0f - banners[i].lifetime / 0.18f;
        if (flash < 0.0f)
            flash = 0.0f;
        if (flash > 1.0f)
            flash = 1.0f;
        float whiten = flash * 0.45f;

        unsigned char fr = (unsigned char)(
            banners[i].r + (255 - banners[i].r) * whiten);
        unsigned char fg = (unsigned char)(
            banners[i].g + (255 - banners[i].g) * whiten);
        unsigned char fb = (unsigned char)(
            banners[i].b + (255 - banners[i].b) * whiten);

        // Neon glow — smooth additive radial blobs strung along the glyph
        // band. No ghosted text copies, so the halo reads soft, not pixelated.
        float bandY = cy;
        float halfW = textWidth / 2.0f;
        float glowPulse = 0.55f + 0.45f * sinf(GetTime() * 44.0f + (float)i * 3.3f);

        BeginBlendMode(BLEND_ADDITIVE);

        // Wide, faint outer bloom
        float outerR = fontSize * 1.5f;
        float step = outerR * 0.55f;
        for (float bx = cx - halfW; bx <= cx + halfW + step; bx += step)
        {
            DrawCircleGradient(
                (Vector2){ bx, bandY },
                outerR,
                (Color){ fr, fg, fb, (unsigned char)(30.0f * fade * glowPulse) },
                (Color){ 0, 0, 0, 0 }
            );
        }

        // Hot neon tube hugging the letters
        float coreR = fontSize * 0.85f;
        step = coreR * 0.6f;
        for (float bx = cx - halfW; bx <= cx + halfW + step; bx += step)
        {
            DrawCircleGradient(
                (Vector2){ bx, bandY },
                coreR,
                (Color){ fr, fg, fb, (unsigned char)(150.0f * fade) },
                (Color){ 0, 0, 0, 0 }
            );
        }

        // Ignition flash — a white-hot bloom on the spawn beat
        if (whiten > 0.01f)
        {
            step = coreR * 0.6f;
            for (float bx = cx - halfW; bx <= cx + halfW + step; bx += step)
            {
                DrawCircleGradient(
                    (Vector2){ bx, bandY },
                    coreR,
                    (Color){ 255, 255, 255, (unsigned char)(130.0f * fade * whiten) },
                    (Color){ 0, 0, 0, 0 }
                );
            }
        }

        EndBlendMode();

        // Main text
        DrawText(
            text,
            textX,
            textY,
            fontSize,
            (Color){ fr, fg, fb, (unsigned char)(255.0f * fade) }
        );
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

        float dx = targetX - tempLabels[i].visualX;
        float dy = targetY - tempLabels[i].visualY;

        float distance = sqrtf(dx * dx + dy * dy);

        if (distance < 5.0f)
        {
            // Arrived at the energy bar — little reward pop as it lands
            Color arrivedColor = {
                tempLabels[i].tr,
                tempLabels[i].tg,
                tempLabels[i].tb,
                255
            };

            // Ability popups detonate on impact
            if (tempLabels[i].type != NULL &&
                strcmp(tempLabels[i].type, "ability") == 0)
            {
                spawnFruitParticles(
                    tempLabels[i].visualX,
                    tempLabels[i].visualY,
                    arrivedColor
                );
            }

            for (int s = 0; s < 5; s++)
            {
                spawnGreedSparkle(
                    tempLabels[i].visualX,
                    tempLabels[i].visualY,
                    arrivedColor
                );
            }

            tempLabels[i].visible = false;
            continue;
        }

        if (distance > 0.001f)
        {
            dx /= distance;
            dy /= distance;
        }

        // Bouncy flight path — the popup wobbles side to side across its
        // direction of travel so it feels alive instead of sliding on rails.
        float sway = 10.0f * fabsf(sinf(tempLabels[i].lifetime * 21.0f + (float)i * 1.7f));
        float sdx = -dy * sway;
        float sdy = dx * sway;

        float speed = 350.0f;

        tempLabels[i].visualX += dx * speed * dt + sdx * 26.0f * dt;
        tempLabels[i].visualY += dy * speed * dt + sdy * 26.0f * dt;

        tempLabels[i].x = (int)tempLabels[i].visualX;
        tempLabels[i].y = (int)tempLabels[i].visualY;

        // Ability popups keep exploding — sparkles continuously shower out of
        // the label the whole way to the energy bar.
        if (tempLabels[i].type != NULL &&
            strcmp(tempLabels[i].type, "ability") == 0 &&
            fmodf(tempLabels[i].lifetime, 0.08f) < dt)
        {
            Color trailColor = {
                tempLabels[i].tr,
                tempLabels[i].tg,
                tempLabels[i].tb,
                255
            };
            for (int s = 0; s < 5; s++)
            {
                spawnGreedSparkle(
                    tempLabels[i].visualX,
                    tempLabels[i].visualY,
                    trailColor
                );
            }
        }

        // Elastic pop-in with a living heartbeat throb: starts oversized,
        // rubber-bands down, then keeps pulsing subtly while it flies.
        float heartbeat =
            1.0f + 0.07f * fabsf(sinf(GetTime() * 16.0f + (float)i));

        float p = tempLabels[i].lifetime / 0.25f;
        if (p > 1.0f)
            p = 1.0f;

        float pop = 1.35f * powf(1.0f - p, 2.0f);
        tempLabels[i].scale = (1.0f + pop) * heartbeat;
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
        // White-hot flash right as it spawns — the popup ignites, then
        // settles into its energy color. Kept at 45% so the color stays
        // mouthwateringly saturated instead of washing to pale white.
        // --------------------------------

        float flash = 1.0f - tempLabels[i].lifetime / 0.18f;
        if (flash < 0.0f)
            flash = 0.0f;
        if (flash > 1.0f)
            flash = 1.0f;

        float whiten = flash * 0.45f;

        unsigned char fr = (unsigned char)(
            tempLabels[i].tr + (255 - tempLabels[i].tr) * whiten);
        unsigned char fg = (unsigned char)(
            tempLabels[i].tg + (255 - tempLabels[i].tg) * whiten);
        unsigned char fb = (unsigned char)(
            tempLabels[i].tb + (255 - tempLabels[i].tb) * whiten);

        // --------------------------------
        // Pulsing halo — a soft glowing blob breathes behind the label so it
        // reads as a bright, living reward.
        // --------------------------------

        float ringPulse =
            0.5f + 0.5f * sinf(GetTime() * 34.0f + (float)i * 2.1f);

        float ringR = fontSize * 1.2f * (1.0f + 0.22f * ringPulse);
        if (ringR > 10.0f)
        {
            DrawCircleGradient(
                (Vector2){
                    tempLabels[i].visualX,
                    tempLabels[i].visualY
                },
                ringR,
                (Color){
                    (unsigned char)(fr * 0.95f),
                    (unsigned char)(fg * 0.95f),
                    (unsigned char)(fb * 0.95f),
                    (unsigned char)(105.0f * fade * (0.45f + 0.55f * ringPulse))
                },
                (Color){ 0, 0, 0, 0 }
            );
        }

        // --------------------------------
        // Outer glow — soft halo outside the glyphs so the text stays sharp.
        // Its intensity pulses to draw the eye.
        // --------------------------------

        float glowPulse =
            0.55f + 0.45f * sinf(GetTime() * 44.0f + (float)i * 3.3f);

        float glowAlphas[4] = { 58.0f, 40.0f, 22.0f, 10.0f };
        int glowOffsets[4] = { 4, 7, 11, 15 };

        for (int pass = 0; pass < 4; pass++)
        {
            int off = glowOffsets[pass];

            float passAlpha = glowAlphas[pass] * fade * glowPulse;
            if (passAlpha < 0.0f)
                passAlpha = 0.0f;

            Color glowPass = {
                fr,
                fg,
                fb,
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
        // Pure-white hot core — a tight white copy under the glyphs makes the
        // saturated color above it sizzle twice as bright.
        // --------------------------------

        DrawText(
            text,
            textX,
            textY,
            fontSize,
            (Color){ 255, 255, 255, (unsigned char)(90.0f * fade * glowPulse * whiten) }
        );

        // --------------------------------
        // Ability popups carry no badge — the constant sparkle explosion that
        // erupts from them (spawn + flight trail + impact) IS the indicator.
        // --------------------------------

        // --------------------------------
        // Main text
        // --------------------------------

        Color textColor = {
            fr,
            fg,
            fb,
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