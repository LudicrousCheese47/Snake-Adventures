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

            // Text size
            tempLabels[i].f_size = 28;

            // Starting scale
            tempLabels[i].scale = 2.5f;
            tempLabels[i].targetScale = 1.0f;

            // Text color
            tempLabels[i].tr = 255;
            tempLabels[i].tg = 255;
            tempLabels[i].tb = 255;
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

    float barX = 5.0f;
    float barY = SCREEN_HEIGHT / 2.0f - 400.0f;
    float barWidth = 10.0f;
    float maxHeight = 180.0f;

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
        // Outer glow
        // --------------------------------

        Color glow1 = {
            0,
            255,
            255,
            (unsigned char)(25.0f * fade)
        };

        DrawText(
            text,
            textX - 3,
            textY,
            fontSize,
            glow1
        );

        DrawText(
            text,
            textX + 3,
            textY,
            fontSize,
            glow1
        );

        DrawText(
            text,
            textX,
            textY - 3,
            fontSize,
            glow1
        );

        DrawText(
            text,
            textX,
            textY + 3,
            fontSize,
            glow1
        );

        // --------------------------------
        // Main text
        // --------------------------------

        Color textColor = {
            0,
            255,
            255,
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